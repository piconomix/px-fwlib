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
#include "px_log_fs.h"
#include "px_rtc_util.h"
#include "px_board.h"
#include "px_buzzer.h"
#include "px_gpio.h"
#include "px_adc.h"
#include "main.h"
#include "px_cfg.h"
#include "px_cli_cmds_sf.h"
#include "px_cli_cmds_rtc.h"
#include "px_cli_cmds_log_fs.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("cli");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static float px_cli_adc_scaled_val(uint8_t channel, uint16_t adc_val)
{

    float scaled_val = adc_val*cfg.fit[channel].scale + cfg.fit[channel].offset;

    return scaled_val;
}

static float px_cli_adc_scaled_ave(uint8_t channel, uint32_t adc_sum)
{

    float scaled_val = ((float)adc_sum / (float)cfg.nr_of_samples)*cfg.fit[channel].scale + cfg.fit[channel].offset;

    return scaled_val;
}

static void px_cli_disp_record(const px_log_fs_record_data_t * record_data)
{
    uint8_t i;
    px_rtc_date_time_t date_time;

    px_rtc_util_sec_since_y2k_to_date_time(record_data->timestamp, &date_time);
    px_rtc_util_report_date_time(&date_time);
    for(i=0; i<PX_ADC_NR_OF_CHANNELS; i++)
    {
        PX_PRINTF_P("\t%f\t%f\t%f", 
                 px_cli_adc_scaled_val(i, record_data->adc_data[i].min),
                 px_cli_adc_scaled_val(i, record_data->adc_data[i].max), 
                 px_cli_adc_scaled_ave(i, record_data->adc_data[i].sum));
    }
    putchar('\n');
}

static const char* px_cli_cmd_log_start_fn(uint8_t argc, char* argv[])
{
    uint8_t                 data;
    px_log_fs_record_data_t record_data;
    uint8_t                 retry;
    uint8_t                 i;

    // Print column headings
    PX_PRINTF_P("Timestamp");
    for(i=0; i<4; i++)
    {
        PX_PRINTF_P("\tADC%u_MIN\tADC%u_MAX\tADC%u_AVE", i, i, i);
    }
    PX_PRINTF_P("\n");

    // Start ADC sampling
    px_adc_start(1000, cfg.nr_of_samples);    
    while(true)
    {
        // ADC Data ready?
        if(px_adc_data_ready())
        {
            PX_LED_ON();
            // Reset flag
            px_adc_rst_data_ready_flag();
            // Get ADC data
            memcpy(&record_data.adc_data, px_adc_get_data(0), PX_SIZEOF_ARRAY(record_data.adc_data));
            // Get RTC time
            record_data.timestamp = px_rtc_util_sec_since_y2k_rd();
            // Log data
            retry = 8;
            while(px_log_fs_wr(&px_log_fs_handle, &record_data, sizeof(record_data)) != PX_LOG_FS_ERR_NONE)
            {
                if(--retry == 0)
                {
                    // Stop logging
                    PX_PRINTF_P("Error. Log file full or write error\n");
                    break;
                }
            }            
            // Display record_data
            px_cli_disp_record(&record_data);            
            PX_LED_OFF();
        }

        // Byte has been received?
        if(px_uart_rd_u8(&px_uart_handle, &data))
        {
            // Has a SPACE been pressed?
            if(data == ' ')
            {
                // Stop logging
                break;
            }
        }
        // Button pressed?
        if(PX_PB_IS_PRESSED())
        {
            // Stop
            break;
        }
    }
    PX_LED_ON();

    px_adc_stop();
    return NULL;
}

static const char* px_cli_cmd_log_dump_fn(uint8_t argc, char* argv[])
{
    uint8_t                 i;
    px_log_fs_err_t         px_log_fs_err;
    px_log_fs_record_data_t record_data;

    // Print column headings
    PX_PRINTF_P("Timestamp");
    for(i=0; i<4; i++)
    {
        PX_PRINTF_P("\tADC%u_MIN\tADC%u_MAX\tADC%u_AVE", i, i, i);
    }
    PX_PRINTF_P("\n");

    px_log_fs_err = px_log_fs_rd_first(&px_log_fs_handle, &record_data, sizeof(record_data));
    while(px_log_fs_err == PX_LOG_FS_ERR_NONE)
    {
        // Display record_data
        px_cli_disp_record(&record_data);
        // Next record_data
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

static const char* px_cli_cmd_log_info_fn(uint8_t argc, char* argv[])
{
    uint8_t i;

    PX_PRINTF_P("Sampling rate = 1000 Hz\n");
    PX_PRINTF_P("Number of samples = %u\n", cfg.nr_of_samples);
    for(i = 0; i<PX_ADC_NR_OF_CHANNELS; i++)
    {
        PX_PRINTF_P("ADC%d : Scale = %f, Offset = %f\n", 
                 i, cfg.fit[i].scale, cfg.fit[i].offset);
    }

    return NULL;
}

static const char* px_cli_cmd_log_fit_fn(uint8_t argc, char* argv[])
{
    uint16_t   ch;
    float scale;
    float offset;

    // <ch>
    if(!px_cli_util_argv_to_u8(0, 0, 3))
    {
        return PX_PGM_STR("Error. <ch> must be 0 to 3");
    }
    ch = px_cli_argv_val.u8;

    // <scale>
    if(!px_cli_util_argv_to_float(1))
    {
        return PX_PGM_STR("Error. Unable to convert <scale> to float");
    }
    scale = px_cli_argv_val.f;

    // <offset>
    if(!px_cli_util_argv_to_float(2))
    {
        return PX_PGM_STR("Error. Unable to convert <offset> to float");
    }
    offset = px_cli_argv_val.f;

    cfg.fit[ch].scale  = scale;
    cfg.fit[ch].offset = offset;

    return NULL;
}

static const char* px_cli_cmd_log_samples_fn(uint8_t argc, char* argv[])
{
    // <nr of samples>
    if(!px_cli_util_argv_to_u16(0, 100, 60000))
    {
        return PX_PGM_STR("Error. <nr of samples> must be 100 to 60000");
    }
    cfg.nr_of_samples = px_cli_argv_val.u16;

    return NULL;
}

// Create CLI command structures
PX_CLI_CMD_CREATE(px_cli_cmd_log_start,     "s",        0, 0,   "",                         "Start logging")
PX_CLI_CMD_CREATE(px_cli_cmd_log_dump,      "dump",     0, 0,   "",                         "Dump log")
PX_CLI_CMD_CREATE(px_cli_cmd_log_del,       "del",      0, 0,   "",                         "Delete log")
PX_CLI_CMD_CREATE(px_cli_cmd_log_info,      "info",     0, 0,   "",                         "Report ADC settings")
PX_CLI_CMD_CREATE(px_cli_cmd_log_fit,       "fit",      3, 3,   "<ch> <scale> <offset>",    "Configure linear fit of ADC channel (y = scale x ADC + offset)")
PX_CLI_CMD_CREATE(px_cli_cmd_log_samples,   "samples",  1, 1,   "<nr of samples>",          "Set number of samples accumulated for each ADC channel")

PX_CLI_GROUP_CREATE(px_cli_group_log, "log")
    PX_CLI_CMD_ADD(px_cli_cmd_log_start,     px_cli_cmd_log_start_fn)
    PX_CLI_CMD_ADD(px_cli_cmd_log_dump,      px_cli_cmd_log_dump_fn)
    PX_CLI_CMD_ADD(px_cli_cmd_log_del,       px_cli_cmd_log_del_fn)
    PX_CLI_CMD_ADD(px_cli_cmd_log_info,      px_cli_cmd_log_info_fn)
    PX_CLI_CMD_ADD(px_cli_cmd_log_fit,       px_cli_cmd_log_fit_fn)
    PX_CLI_CMD_ADD(px_cli_cmd_log_samples,   px_cli_cmd_log_samples_fn)
PX_CLI_GROUP_END()

PX_CLI_CMD_CREATE(px_cli_cmd_help,          "help",     0, 1,   "[cmd(s) starts with...]",  "Display list of commands with help. Optionally the list can be reduced.")

// Add CLI commands to CLI list
PX_CLI_CMD_LIST_CREATE(px_cli_cmd_list)
    PX_CLI_GROUP_ADD   (px_cli_group_log)
    PX_CLI_GROUP_ADD   (px_cli_group_rtc)
    PX_CLI_GROUP_ADD   (px_cli_group_log_fs)
    PX_CLI_GROUP_ADD   (px_cli_group_sf)
    PX_CLI_CMD_ADD     (px_cli_cmd_help,  px_cli_cmd_help_fn)
PX_CLI_CMD_LIST_END()
