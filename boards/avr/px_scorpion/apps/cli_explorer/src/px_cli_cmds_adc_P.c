/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2014 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          px_cli_cmds_spi.h : CLI commands for SPI
    Author(s):      Pieter Conradie
    Creation Date:  2014-05-31

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <avr/io.h>
#include <string.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_cli.h"
#include "px_pgm_P.h"
#include "px_adc.h"
#include "px_board.h"
#include "main.h"
#include "px_uart.h"
#include "px_dbg.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_DBG_DECL_NAME("cli_cmds_adc");

/// Linear fit variables
typedef struct
{
    float scale;
    float offset;
} px_adc_linear_fit_t;

/// ADC configuration for all channels
typedef struct
{
    uint16_t            nr_of_samples;
    px_adc_linear_fit_t channel[PX_ADC_NR_OF_CHANNELS];
} px_adc_cfg_t;

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
px_adc_cfg_t px_adc_cfg =
{
    1000,
    {
        {(3300.0 * 1.0) / 1023.0, 0.0},
        {(3300.0 * 1.0) / 1023.0, 0.0},
        {(3300.0 * 11.0)/ 1023.0, 0.0},
        {(3300.0 * 11.0)/ 1023.0, 0.0},
    }
};

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static const char* px_cli_cmd_fn_adc_info(uint8_t argc, char* argv[])
{
    uint8_t i;

    PX_PRINTF_P("Sampling rate = 1000 Hz\n");
    PX_PRINTF_P("Number of samples = %d\n", px_adc_cfg.nr_of_samples);
    for(i = 0; i<PX_ADC_NR_OF_CHANNELS; i++)
    {
        PX_PRINTF_P("ADC%d : Scale = %f, Offset = %f\n", 
                 i, px_adc_cfg.channel[i].scale, px_adc_cfg.channel[i].offset);
    }

    return NULL;
}

static const char* px_cli_cmd_fn_adc_cfg(uint8_t argc, char* argv[])
{
    uint16_t ch;
    float    scale;
    float    offset;

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

    px_adc_cfg.channel[ch].scale  = scale;
    px_adc_cfg.channel[ch].offset = offset;

    return NULL;
}

static const char* px_cli_cmd_fn_adc_cfg_samples(uint8_t argc, char* argv[])
{
    // <nr of samples>
    if(!px_cli_util_argv_to_u16(0, 100, 60000))
    {
        return PX_PGM_STR("Error. <nr of samples> must be 100 to 60000");
    }
    px_adc_cfg.nr_of_samples = px_cli_argv_val.u16;

    return NULL;
}

static const char* px_cli_cmd_fn_adc_s(uint8_t argc, char* argv[])
{
    uint16_t ch;
    uint16_t val;
    float scaled_val;

    // <ch>
    if(!px_cli_util_argv_to_u8(0, 0, 3))
    {
        return PX_PGM_STR("Error. <ch> must be 0 to 3");
    }
    ch = px_cli_argv_val.u8;

    val = px_adc_sample(ch);

    scaled_val  =   px_adc_cfg.channel[ch].scale * (float)val
                  + px_adc_cfg.channel[ch].offset;

    PX_PRINTF_P("%04u\t%f\n", val, scaled_val);

    return NULL;
}

static const char* px_cli_cmd_fn_adc_sc(uint8_t argc, char* argv[])
{
    uint8_t   i;
    uint8_t   data;
    uint16_t  counter;
    const     px_adc_data_t * adc_data;
    float     scaled_val;

    px_adc_start(1000, px_adc_cfg.nr_of_samples);

    counter = 0;
    while(true)
    {
        // ADC Data ready?
        if(px_adc_data_ready())
        {
            // Reset flag
            px_adc_rst_data_ready_flag();
            // Display TAB separated ADC data for each channel
            PX_PRINTF_P("%05d", counter);
            for(i=0; i<PX_ADC_NR_OF_CHANNELS; i++)
            {
                adc_data = px_adc_get_data(i);
                scaled_val  =    px_adc_cfg.channel[i].scale *
                                ((float)adc_data->sum / px_adc_cfg.nr_of_samples)
                              + px_adc_cfg.channel[i].offset;
                PX_PRINTF_P("\t%f", scaled_val);
            }
            putchar('\n');
            // Increment counter
            counter++;
        }

        // Byte has been received?
        if(px_uart_rd_u8(&px_uart_handle, &data))
        {
            // Has a SPACE been pressed?
            if(data == ' ')
            {
                // Stop
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

    px_adc_stop();
    return NULL;
}

// Create CLI command structures
PX_CLI_CMD_CREATE(px_cli_cmd_adc_info,    "info",         0, 0,   "",                                     "Report ADC settings")
PX_CLI_CMD_CREATE(px_cli_cmd_adc_cfg,     "cfg",          3, 3,   "<ch> <scale> <offset>",                "Configure linear fit of ADC channel (y = scale x ADC + offset)")
PX_CLI_CMD_CREATE(px_cli_cmd_cfg_samples, "cfg_samples",  1, 1,   "<nr of samples>",                      "Set number of samples accumulated for each ADC channel")
PX_CLI_CMD_CREATE(px_cli_cmd_adc_s,       "s",            1, 1,   "<ch>",                                 "Sample single ADC channel")
PX_CLI_CMD_CREATE(px_cli_cmd_adc_sc,      "sc",           0, 0,   "",                                     "Sample all ADC channels until SPACE or user button is pressed")

PX_CLI_GROUP_CREATE(px_cli_group_adc, "adc")
    PX_CLI_CMD_ADD(px_cli_cmd_adc_info,    px_cli_cmd_fn_adc_info)
    PX_CLI_CMD_ADD(px_cli_cmd_adc_cfg,     px_cli_cmd_fn_adc_cfg)
    PX_CLI_CMD_ADD(px_cli_cmd_cfg_samples, px_cli_cmd_fn_adc_cfg_samples)
    PX_CLI_CMD_ADD(px_cli_cmd_adc_s,       px_cli_cmd_fn_adc_s)
    PX_CLI_CMD_ADD(px_cli_cmd_adc_sc,      px_cli_cmd_fn_adc_sc)
PX_CLI_GROUP_END()
