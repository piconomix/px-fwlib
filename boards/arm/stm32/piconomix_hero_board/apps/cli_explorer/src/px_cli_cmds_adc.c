/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
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
 
    Title:          px_cli_cmds_adc.h : CLI commands for ADC
    Author(s):      Pieter Conradie
    Creation Date:  2018-11-04

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdlib.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_cli.h"
#include "px_adc.h"
#include "px_board.h"
#include "main.h"
#include "px_systmr.h"

#include "px_dbg.h"
PX_DBG_DECL_NAME("cli_cmds_adc")

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static px_systmr_t adc_tmr;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static const char* px_cli_cmd_fn_adc_s(uint8_t argc, char* argv[])
{
    px_adc_ch_t ch;
    uint16_t    val;
    uint16_t    val_scaled;
    uint16_t    interval_ms;
    uint32_t    counter = 0;
    char *      end;

    // <0..3> or <c0..c18> combination?
    if((strlen(argv[0]) < 1) || (strlen(argv[0]) > 3))
    {
        return "Error. <ch> must either be <0..3> or <ch0..ch18>";
    }

    // <ch>
    if(strlen(argv[0]) == 1)
    {
        if(!px_cli_util_argv_to_u8(0, 0, 3))
        {
            return "Error. <ch> must be 0 to 3";
        }

        // Map board's ADC pin to ADC channel
        switch(px_cli_argv_val.u8)
        {
        case 0:  ch = PX_BOARD_ADC0;     break;
        case 1:  ch = PX_BOARD_ADC1;     break;
        case 2:  ch = PX_BOARD_ADC2;     break;
        case 3:  ch = PX_BOARD_ADC3;     break;
        default: ch = PX_BOARD_ADC0;     break;
        }
    }
    else
    {
        // Convert number
        unsigned long i = strtoul(&argv[0][1], &end, 0);
        if(  (*end != '\0') || (i > 18)  )
        {
            return "Error. <ch> must either be <ch0..ch18>";
        }
        // Set channel
        ch = (px_adc_ch_t)i;        
    }

    // [interval ms]
    if(argc > 1)
    {
        if(px_cli_util_argv_to_u16(1, 0, 60000))
        {
            interval_ms = px_cli_argv_val.u16;        
        }
        else
        {
            return "Error: Interval must be from 0 to 60000 ms";
        }
    }
    else
    {
        interval_ms = 0;
    }

    // Start timer
    px_systmr_start(&adc_tmr, PX_SYSTMR_MS_TO_TICKS(interval_ms));

    for(;;)
    {
        // Get raw ADC value
        val = px_adc_sample(&px_adc_handle, ch);
        // Scale value
        val_scaled = (uint16_t)PX_UDIV_ROUND((uint32_t)val * 3300, (uint32_t)4095);

        // Continuous reporting?
        if(interval_ms > 0)
        {
            printf("%lu ", counter++);
        }
        // Report value
        printf("%04u %u mV\n", val, val_scaled);

        // Once only?
        if(interval_ms == 0)
        {
            break;
        }

        // Wait until timer has expired
        while(!px_systmr_has_expired(&adc_tmr))
        {
            // Button pressed?
            if(PX_USR_PB_IS_PRESSED())
            {
                // Wait until button is released
                while(PX_USR_PB_IS_PRESSED())
                {
                    ;
                }
                // Stop
                return NULL;
            }
        }
        // Reset timer
        px_systmr_reset(&adc_tmr);
    };

    return NULL;
}

static const char* px_cli_cmd_fn_adc_bat(uint8_t argc, char* argv[])
{
    uint16_t val;
    uint32_t counter = 0;
    uint16_t interval_ms;

    // [interval ms]
    if(argc > 0)
    {
        if(px_cli_util_argv_to_u16(0, 0, 60000))
        {
            interval_ms = px_cli_argv_val.u16;        
        }
        else
        {
            return "Error: Interval must be from 0 to 60000 ms";
        }
    }
    else
    {
        interval_ms = 0;
    }

    // Start timer
    px_systmr_start(&adc_tmr, PX_SYSTMR_MS_TO_TICKS(interval_ms));

    for(;;)
    {
        // Enable measurement circuit
        PX_VBAT_MEAS_ENABLE();
        // Wait 1 ms for capacitor to charge to final value;
        // (actually 250 us (5 x RC) is good enough for 99.33% of final value)
        px_board_delay_ms(1);
        // Get raw ADC value
        val = px_adc_sample(&px_adc_handle, PX_BOARD_ADC_VBAT);
        // Disable measurement circuit
        PX_VBAT_MEAS_DISABLE();
        // Scale value
        val = (uint16_t)PX_UDIV_ROUND((uint32_t)val * (3300 * 2), (uint32_t)4095);

        // Continuous reporting?
        if(interval_ms > 0)
        {
            printf("%lu ", counter++);
        }
        // Report value
        printf("%u mV\n", val);

        // Once only?
        if(interval_ms == 0)
        {
            break;
        }

        // Wait until timer has expired
        while(!px_systmr_has_expired(&adc_tmr))
        {
            // Button pressed?
            if(PX_USR_PB_IS_PRESSED())
            {
                // Wait until button is released
                while(PX_USR_PB_IS_PRESSED())
                {
                    ;
                }
                // Stop
                return NULL;
            }
        }
        // Reset timer
        px_systmr_reset(&adc_tmr);
    };

    return NULL;
}

// Create CLI command structures
PX_CLI_CMD_CREATE(px_cli_cmd_adc_s,       "s",            1, 2,   "<ch> [interval ms]", "Sample ADC channel. <ch> is <0..3> or <c0..c18>")
PX_CLI_CMD_CREATE(px_cli_cmd_adc_bat,     "b",            0, 1,   "[interval ms]",      "Measure battery voltage. Optionally, continuous until button is pressed")

PX_CLI_GROUP_CREATE(px_cli_group_adc, "adc")
    PX_CLI_CMD_ADD(px_cli_cmd_adc_s,       px_cli_cmd_fn_adc_s)
    PX_CLI_CMD_ADD(px_cli_cmd_adc_bat,     px_cli_cmd_fn_adc_bat)
PX_CLI_GROUP_END()
