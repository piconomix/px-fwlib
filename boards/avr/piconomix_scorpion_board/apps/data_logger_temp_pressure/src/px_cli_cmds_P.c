/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2014 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          CLI Commands
    Author(s):      Pieter Conradie
    Creation Date:  2008-11-01

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <string.h>
#include <avr/sleep.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_cli.h"
#include "px_pgm_P.h"
#include "px_systmr.h"
#include "px_at45d.h"
#include "px_bmp280.h"
#include "px_log_fs.h"
#include "px_board.h"
#include "px_buzzer.h"
#include "px_gpio.h"
#include "main.h"
#include "px_cli_cmds_sf.h"
#include "px_cli_cmds_rtc.h"
#include "px_cli_cmds_log_fs.h"
#include "px_dbg.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_DBG_DECL_NAME("cli");

#define LOG_PERIOD_MIN  30

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static log_data_t         log_data;
static px_rtc_date_time_t log_date_time;
static px_rtc_date_time_t log_alarm;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static const char* px_cli_cmd_meas_fn(uint8_t argc, char* argv[])
{
    if(!px_bmp280_read(&log_data.bmp280_temp, &log_data.bmp280_press))
    {
        return PX_PGM_STR("Error! BMP280 read failed");
    }

    PX_PRINTF_P("Temp = %ld, Pressure = %ld\n", 
             log_data.bmp280_temp, 
             log_data.bmp280_press);

    return NULL;
}

static const char* px_cli_cmd_log_start_fn(uint8_t argc, char* argv[])
{
    uint8_t  data;
    uint16_t nr;
    uint8_t  retry;

    // Record must be big enough to hold log data
    PX_DBG_ASSERT(PX_LOG_FS_CFG_REC_DATA_SIZE >= sizeof(log_data));

    // Get RTC time
    px_rtc_util_date_time_rd(&log_date_time);
    // Create a new log file
    if(px_log_fs_create((px_log_fs_time_stamp_t *)&log_date_time) != PX_LOG_FS_ERR_NONE)
    {
        return PX_PGM_STR("Error. Unable to create file");
    }

    // Start sampling
    nr = 0;
    while(true)
    {
        // Set alarm
        px_rtc_util_date_time_rd(&log_alarm);
        // Keep hour and minute and set other fields to zero
        log_alarm.year  = 0;
        log_alarm.month = 0;
        log_alarm.day   = 0;
        log_alarm.sec   = 0;
        // Advance one minute to make sure that alarm will definitely expire
        if(++log_alarm.min >= 60)
        {
            log_alarm.min = 0;
            if(++log_alarm.hour >= 24)
            {
                // Wrap
                log_alarm.hour = 0;
            }
        }
        // Round up minutes to nearest period
        log_alarm.min = (log_alarm.min/LOG_PERIOD_MIN + 1)*LOG_PERIOD_MIN;
        // Next hour
        if(log_alarm.min >= 60)
        {
            log_alarm.min = 0;
            if(++log_alarm.hour >= 24)
            {
                // Wrap
                log_alarm.hour = 0;
            }
        }
        px_rtc_util_alarm_flag_clear();
        px_rtc_util_alarm_wr(&log_alarm, PX_RTC_UTIL_ALARM_MASK_HOUR);        

        // Wait until alarm rings
        PX_LED_OFF();
        while(!px_rtc_util_alarm_flag_is_set())
        {
            // Sleep
            set_sleep_mode(SLEEP_MODE_IDLE); 
            sleep_mode(); 

            // Byte has been received?
            if(px_uart_rd_u8(&px_uart_handle, &data))
            {
                // Has a SPACE been pressed?
                if(data == ' ')
                {
                    // Stop logging
                    return NULL;
                }
            }
            // Button pressed?
            if(PX_PB_IS_PRESSED())
            {
                // Stop logging
                return NULL;
            }
        }
        PX_LED_ON();

        // Get RTC time
        px_rtc_util_date_time_rd(&log_data.date_time);

        // Perform temperature and pressure measurement
        if(!px_bmp280_read(&log_data.bmp280_temp, &log_data.bmp280_press))
        {
            return PX_PGM_STR("Error! BMP280 read failed");
        }

        // Display TAB separated data
        PX_PRINTF_P("%05u\t%02hu-%02hu-%02hu\t%02hu:%02hu\t%ld\t%ld\n",
                 nr++,
                 (unsigned short)log_data.date_time.year,
                 (unsigned short)log_data.date_time.month,
                 (unsigned short)log_data.date_time.day,
                 (unsigned short)log_data.date_time.hour,
                 (unsigned short)log_data.date_time.min,
                 log_data.bmp280_temp,
                 log_data.bmp280_press);
    
        // Log data
        retry = 8;
        while(px_log_fs_record_wr(&log_data,  sizeof(log_data)) != PX_LOG_FS_ERR_NONE)
        {
            if(--retry == 0)
            {
                // Stop logging
                PX_PRINTF_P("Error. Log file full or write error\n");
                break;
            }
        }        
    }

    return NULL;
}

static const char* px_cli_cmd_log_list_fn(uint8_t argc, char* argv[])
{
    px_log_fs_err_t    px_log_fs_err;
    px_log_fs_file_t   file;
    uint16_t           file_nr = 1;

    px_log_fs_err = px_log_fs_file_find_first(&file);
    while(px_log_fs_err == PX_LOG_FS_ERR_NONE)
    {
        PX_PRINTF_P("File %u [%02hu-%02hu-%02hu %02hu:%02hu:%02hu]\n",
                 file_nr++,
                 (unsigned short)file.time_stamp.year,
                 (unsigned short)file.time_stamp.month,
                 (unsigned short)file.time_stamp.day,
                 (unsigned short)file.time_stamp.hour,
                 (unsigned short)file.time_stamp.min,
                 (unsigned short)file.time_stamp.sec);
        px_log_fs_err = px_log_fs_file_find_next(&file);
    }

    return NULL;
}

static const char* px_cli_cmd_log_dump_fn(uint8_t argc, char* argv[])
{
    uint16_t         nr;
    px_log_fs_err_t  px_log_fs_err;
    px_log_fs_file_t file;
    log_data_t    record_data;

    // <number>
    if(!px_cli_util_argv_to_u16(0, 1, 65535))
    {
        return PX_PGM_STR("Error. <number> must be 1 to 65535");
    }

    nr = 0;
    px_log_fs_err = px_log_fs_file_find_first(&file);
    while(px_log_fs_err == PX_LOG_FS_ERR_NONE)
    {
        // File number found?
        if(++nr == px_cli_argv_val.u16)
        {
            break;
        }        
        px_log_fs_err = px_log_fs_file_find_next(&file);
    }
    if(px_log_fs_err != PX_LOG_FS_ERR_NONE)
    {
        return PX_PGM_STR("Error. File not found");
    }

    PX_PRINTF_P("File %d [%02hu-%02hu-%02hu %02hu:%02hu:%02hu]\n",
                 nr,
                 (unsigned short)file.time_stamp.year,
                 (unsigned short)file.time_stamp.month,
                 (unsigned short)file.time_stamp.day,
                 (unsigned short)file.time_stamp.hour,
                 (unsigned short)file.time_stamp.min,
                 (unsigned short)file.time_stamp.sec);

    if(px_log_fs_open(&file) != PX_LOG_FS_ERR_NONE)
    {
        return PX_PGM_STR("Error. Unable to open file");
    }

    nr = 0;
    px_log_fs_err = px_log_fs_record_rd_first(&record_data, sizeof(log_data_t));
    while(px_log_fs_err == PX_LOG_FS_ERR_NONE)
    {
        // Display TAB separated data
        PX_PRINTF_P("%05u\t%02hu-%02hu-%02hu\t%02hu:%02hu\t%ld\t%ld\n",
                 nr++,
                 (unsigned short)record_data.date_time.year,
                 (unsigned short)record_data.date_time.month,
                 (unsigned short)record_data.date_time.day,
                 (unsigned short)record_data.date_time.hour,
                 (unsigned short)record_data.date_time.min,
                 record_data.bmp280_temp,
                 record_data.bmp280_press);

        // Next record
        px_log_fs_err = px_log_fs_record_rd_next(&record_data, sizeof(log_data_t));
    }
    return NULL;
}

static const char* px_cli_cmd_log_del_fn(uint8_t argc, char* argv[])
{
    uint16_t         nr;
    px_log_fs_err_t  px_log_fs_err;
    px_log_fs_file_t file;

    nr = 0;
    px_log_fs_err = px_log_fs_file_find_first(&file);
    while(px_log_fs_err == PX_LOG_FS_ERR_NONE)
    {
        px_log_fs_file_delete(&file);
        px_log_fs_err = px_log_fs_file_find_first(&file);
        nr++;
    }

    PX_PRINTF_P("Deleted %u files\n", nr);
    return NULL;
}

// Create CLI command structures
PX_CLI_CMD_CREATE(px_cli_cmd_log_start, "s",    0, 0,   "",                         "Start logging")
PX_CLI_CMD_CREATE(px_cli_cmd_log_list,  "ls",   0, 0,   "",                         "List logs")
PX_CLI_CMD_CREATE(px_cli_cmd_log_dump,  "dump", 1, 1,   "<number>",                 "Dump log")
PX_CLI_CMD_CREATE(px_cli_cmd_log_del,   "del",  0, 0,   "",                         "Delete all logs")

PX_CLI_GROUP_CREATE(px_cli_group_log,   "log")
    PX_CLI_CMD_ADD(px_cli_cmd_log_start,     px_cli_cmd_log_start_fn)
    PX_CLI_CMD_ADD(px_cli_cmd_log_list,      px_cli_cmd_log_list_fn)
    PX_CLI_CMD_ADD(px_cli_cmd_log_dump,      px_cli_cmd_log_dump_fn)
    PX_CLI_CMD_ADD(px_cli_cmd_log_del,       px_cli_cmd_log_del_fn)    
PX_CLI_GROUP_END()

PX_CLI_CMD_CREATE(px_cli_cmd_meas,      "m",    0, 0,   "",                         "Measure temperature and barometric pressure.")
PX_CLI_CMD_CREATE(px_cli_cmd_help,      "help", 0, 1,   "[cmd(s) starts with...]",  "Display list of commands with help. Optionally the list can be reduced.")

// Add CLI commands to CLI list
PX_CLI_CMD_LIST_CREATE()
    PX_CLI_CMD_ADD  (px_cli_cmd_meas, px_cli_cmd_meas_fn)
    PX_CLI_GROUP_ADD(px_cli_group_log)
    PX_CLI_GROUP_ADD(px_cli_group_rtc)
    PX_CLI_GROUP_ADD(px_cli_group_log_fs)
    PX_CLI_GROUP_ADD(px_cli_group_sf)
    PX_CLI_CMD_ADD  (px_cli_cmd_help,  px_cli_cmd_help_fn)
PX_CLI_CMD_LIST_END()
