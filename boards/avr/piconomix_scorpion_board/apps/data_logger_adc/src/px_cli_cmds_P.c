/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2014 Pieter Conradie <https://piconomix.com>
 
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.
    
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

#include "px_dbg.h"
PX_DBG_DECL_NAME("cli")

/* _____LOCAL DEFINITIONS____________________________________________________ */

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

static void px_cli_adc_disp_data(const px_adc_data_t * adc_data)
{
    uint8_t i;

    for(i=0; i<PX_ADC_NR_OF_CHANNELS; i++)
    {
        PX_PRINTF_P("\t%f\t%f\t%f", 
                 px_cli_adc_scaled_val(i, adc_data[i].min),
                 px_cli_adc_scaled_val(i, adc_data[i].max), 
                 px_cli_adc_scaled_ave(i, adc_data[i].sum));
    }
    putchar('\n');
}

static const char* px_cli_cmd_log_start_fn(uint8_t argc, char* argv[])
{
    uint8_t                data;
    uint16_t               nr;
    const px_adc_data_t  * adc_data;
    px_rtc_date_time_t     date_time;
    uint8_t                retry;
    uint8_t                i;

    // Record must be big enough to hold ADC Data
    PX_DBG_ASSERT(PX_LOG_FS_CFG_REC_DATA_SIZE >= sizeof(px_adc_data_t)*PX_ADC_NR_OF_CHANNELS);

    // Get RTC time
    px_rtc_util_date_time_rd(&date_time);
    // Create a new log file
    if(px_log_fs_create((px_log_fs_time_stamp_t *)&date_time) != PX_LOG_FS_ERR_NONE)
    {
        return PX_PGM_STR("Error. Unable to create file");
    }

    // Print column headings
    PX_PRINTF_P("Number");
    for(i=0; i<4; i++)
    {
        PX_PRINTF_P("\tADC%u_MIN\tADC%u_MAX\tADC%u_AVE", i, i, i);
    }
    PX_PRINTF_P("\n");

    // Start ADC sampling
    nr = 0;
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
            adc_data = px_adc_get_data(0);
            // Log data
            retry = 8;
            while(px_log_fs_record_wr(adc_data, sizeof(px_adc_data_t) * PX_ADC_NR_OF_CHANNELS) != PX_LOG_FS_ERR_NONE)
            {
                if(--retry == 0)
                {
                    // Stop logging
                    PX_PRINTF_P("Error. Log file full or write error\n");
                    break;
                }
            }
            
            // Display TAB separated ADC data for each channel
            PX_PRINTF_P("%05u", nr++);
            px_cli_adc_disp_data(adc_data);
            
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

static const char* px_cli_cmd_log_list_fn(uint8_t argc, char* argv[])
{
    px_log_fs_err_t  px_log_fs_err;
    px_log_fs_file_t file;
    uint16_t         file_nr = 1;

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
    uint8_t          i;
    uint16_t         nr;
    px_log_fs_err_t  px_log_fs_err;
    px_log_fs_file_t file;
    px_adc_data_t    adc_data[PX_ADC_NR_OF_CHANNELS];

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

    PX_PRINTF_P("File %u [%02hu-%02hu-%02hu %02hu:%02hu:%02hu]\n",
             nr++,
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

    // Print column headings
    PX_PRINTF_P("Number");
    for(i=0; i<4; i++)
    {
        PX_PRINTF_P("\tADC%u_MIN\tADC%u_MAX\tADC%u_AVE", i, i, i);
    }
    PX_PRINTF_P("\n");

    nr = 0;
    px_log_fs_err = px_log_fs_record_rd_first(adc_data, sizeof(adc_data));
    while(px_log_fs_err == PX_LOG_FS_ERR_NONE)
    {
        // Display TAB separated ADC data for each channel
        PX_PRINTF_P("%05u", nr++);
        px_cli_adc_disp_data(adc_data);
        // Next record
        px_log_fs_err = px_log_fs_record_rd_next(adc_data, sizeof(adc_data));
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
PX_CLI_CMD_CREATE(px_cli_cmd_log_list,      "ls",       0, 0,   "",                         "List logs")
PX_CLI_CMD_CREATE(px_cli_cmd_log_dump,      "dump",     1, 1,   "<number>",                 "Dump log")
PX_CLI_CMD_CREATE(px_cli_cmd_log_del,       "del",      0, 0,   "",                         "Delete all logs")
PX_CLI_CMD_CREATE(px_cli_cmd_log_info,      "info",     0, 0,   "",                         "Report ADC settings")
PX_CLI_CMD_CREATE(px_cli_cmd_log_fit,       "fit",      3, 3,   "<ch> <scale> <offset>",    "Configure linear fit of ADC channel (y = scale x ADC + offset)")
PX_CLI_CMD_CREATE(px_cli_cmd_log_samples,   "samples",  1, 1,   "<nr of samples>",          "Set number of samples accumulated for each ADC channel")

PX_CLI_GROUP_CREATE(px_cli_group_log, "log")
    PX_CLI_CMD_ADD(px_cli_cmd_log_start,     px_cli_cmd_log_start_fn)
    PX_CLI_CMD_ADD(px_cli_cmd_log_list,      px_cli_cmd_log_list_fn)
    PX_CLI_CMD_ADD(px_cli_cmd_log_dump,      px_cli_cmd_log_dump_fn)
    PX_CLI_CMD_ADD(px_cli_cmd_log_del,       px_cli_cmd_log_del_fn)
    PX_CLI_CMD_ADD(px_cli_cmd_log_info,      px_cli_cmd_log_info_fn)
    PX_CLI_CMD_ADD(px_cli_cmd_log_fit,       px_cli_cmd_log_fit_fn)
    PX_CLI_CMD_ADD(px_cli_cmd_log_samples,   px_cli_cmd_log_samples_fn)
PX_CLI_GROUP_END()

PX_CLI_CMD_CREATE(px_cli_cmd_help,          "help",     0, 1,   "[cmd(s) starts with...]",  "Display list of commands with help. Optionally the list can be reduced.")

// Add CLI commands to CLI list
PX_CLI_CMD_LIST_CREATE()
    PX_CLI_GROUP_ADD   (px_cli_group_log)
    PX_CLI_GROUP_ADD   (px_cli_group_rtc)
    PX_CLI_GROUP_ADD   (px_cli_group_log_fs)
    PX_CLI_GROUP_ADD   (px_cli_group_sf)
    PX_CLI_CMD_ADD     (px_cli_cmd_help,  px_cli_cmd_help_fn)
PX_CLI_CMD_LIST_END()
