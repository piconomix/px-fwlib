/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2014 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
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
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("cli");

#define LOG_PERIOD_MIN  30

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static px_log_fs_record_data_t record_data;
static px_rtc_date_time_t      log_date_time;
static px_rtc_date_time_t      log_alarm;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static const char* px_cli_cmd_meas_fn(uint8_t argc, char* argv[])
{
    if(!px_bmp280_read(&record_data.bmp280_temp, &record_data.bmp280_press))
    {
        return PX_PGM_STR("Error! BMP280 read failed");
    }
    PX_PRINTF_P("Temp = %ld, Pressure = %ld\n", 
             record_data.bmp280_temp, 
             record_data.bmp280_press);

    return NULL;
}

static const char* px_cli_cmd_log_start_fn(uint8_t argc, char* argv[])
{
    uint8_t  data;
    uint8_t  retry;

    // Start sampling
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
        px_rtc_util_date_time_rd(&log_date_time);
        record_data.timestamp = px_rtc_util_date_time_to_sec_since_y2k(&log_date_time);
        // Perform temperature and pressure measurement
        if(!px_bmp280_read(&record_data.bmp280_temp, &record_data.bmp280_press))
        {
            return PX_PGM_STR("Error! BMP280 read failed");
        }
        // Display TAB separated data
        PX_PRINTF_P("%02hu-%02hu-%02hu %02hu:%02hu:%02hu",
                (unsigned short)log_date_time.year,
                (unsigned short)log_date_time.month,
                (unsigned short)log_date_time.day,
                (unsigned short)log_date_time.hour,
                (unsigned short)log_date_time.min,
                (unsigned short)log_date_time.sec);
        PX_PRINTF_P("\t%ld\t%ld\n", record_data.bmp280_temp, record_data.bmp280_press);    
        // Log data
        retry = 8;
        while(px_log_fs_wr(&px_log_fs_handle, &record_data,  sizeof(record_data)) != PX_LOG_FS_ERR_NONE)
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

static const char* px_cli_cmd_log_dump_fn(uint8_t argc, char* argv[])
{
    px_log_fs_err_t    px_log_fs_err;
    px_rtc_date_time_t date_time;

    px_log_fs_err = px_log_fs_rd_first(&px_log_fs_handle, &record_data, sizeof(record_data));
    while(px_log_fs_err == PX_LOG_FS_ERR_NONE)
    {
        // Display TAB separated data
        px_rtc_util_sec_since_y2k_to_date_time(record_data.timestamp, &date_time);
        PX_PRINTF_P("%02hu-%02hu-%02hu %02hu:%02hu:%02hu",
                (unsigned short)date_time.year,
                (unsigned short)date_time.month,
                (unsigned short)date_time.day,
                (unsigned short)date_time.hour,
                (unsigned short)date_time.min,
                (unsigned short)date_time.sec);
        PX_PRINTF_P("\t%ld\t%ld\n", record_data.bmp280_temp, record_data.bmp280_press);
        // Next record
        px_log_fs_err = px_log_fs_rd_next(&px_log_fs_handle, &record_data, sizeof(record_data));
    }
    return NULL;
}

static const char* px_cli_cmd_log_del_fn(uint8_t argc, char* argv[])
{
    // Reset file system
    if(px_log_fs_reset(&px_log_fs_handle, 0, PX_AT45D_PAGES - 1) != PX_LOG_FS_ERR_NONE)
    {
        return PX_PGM_STR("Error. Unable reset file system");
    }    
    return NULL;
}

// Create CLI command structures
PX_CLI_CMD_CREATE(px_cli_cmd_log_start, "s",    0, 0,   "",                         "Start logging")
PX_CLI_CMD_CREATE(px_cli_cmd_log_dump,  "dump", 0, 0,   "",                         "Dump log")
PX_CLI_CMD_CREATE(px_cli_cmd_log_del,   "del",  0, 0,   "",                         "Delete log")

PX_CLI_GROUP_CREATE(px_cli_group_log,   "log")
    PX_CLI_CMD_ADD(px_cli_cmd_log_start,     px_cli_cmd_log_start_fn)
    PX_CLI_CMD_ADD(px_cli_cmd_log_dump,      px_cli_cmd_log_dump_fn)
    PX_CLI_CMD_ADD(px_cli_cmd_log_del,       px_cli_cmd_log_del_fn)    
PX_CLI_GROUP_END()

PX_CLI_CMD_CREATE(px_cli_cmd_meas,      "m",    0, 0,   "",                         "Measure temperature and barometric pressure.")
PX_CLI_CMD_CREATE(px_cli_cmd_help,      "help", 0, 1,   "[cmd(s) starts with...]",  "Display list of commands with help. Optionally the list can be reduced.")

// Add CLI commands to CLI list
PX_CLI_CMD_LIST_CREATE(px_cli_cmd_list)
    PX_CLI_CMD_ADD  (px_cli_cmd_meas, px_cli_cmd_meas_fn)
    PX_CLI_GROUP_ADD(px_cli_group_log)
    PX_CLI_GROUP_ADD(px_cli_group_rtc)
    PX_CLI_GROUP_ADD(px_cli_group_log_fs)
    PX_CLI_GROUP_ADD(px_cli_group_sf)
    PX_CLI_CMD_ADD  (px_cli_cmd_help,  px_cli_cmd_help_fn)
PX_CLI_CMD_LIST_END()
