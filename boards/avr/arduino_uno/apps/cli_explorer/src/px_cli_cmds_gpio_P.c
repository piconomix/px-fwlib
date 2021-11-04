/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2014 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_cli_cmds_gpio.h : CLI Commands for GPIO
    Author(s):      Pieter Conradie
    Creation Date:  2014-05-31

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <avr/io.h>
#include <string.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_cli.h"
#include "px_pgm_P.h"
#include "px_board.h"
#include "px_gpio.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("cli_cmds_gpio");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static px_gpio_handle_t px_gpio_handle;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
const char * px_cli_cmd_gpio_str_to_handle(px_gpio_handle_t * handle,
                                           char *             pin_str)
{
    // <pin> or <port,pin> combination?
    if((strlen(pin_str) < 1) || (strlen(pin_str) > 2))
    {
        return "Error. <pin> must either be <2..9>, <b0..b7>, <c0..c7> or <d0..d7>";
    }

    // Expansion GPIO pins <2..9>?
    if(strlen(pin_str) == 1)
    {
        switch(pin_str[0])
        {
        case '2':
            px_gpio_open(handle, PX_GPIO_D2);
            break;
        case '3':
            px_gpio_open(handle, PX_GPIO_D3);
            break;
        case '4':
            px_gpio_open(handle, PX_GPIO_D4);
            break;
        case '5':
            px_gpio_open(handle, PX_GPIO_D5);
            break;
        case '6':
            px_gpio_open(handle, PX_GPIO_D6);
            break;
        case '7':
            px_gpio_open(handle, PX_GPIO_D7);
            break;
        case '8':
            px_gpio_open(handle, PX_GPIO_D8);
            break;
        case '9':
            px_gpio_open(handle, PX_GPIO_D9);
            break;
        default:
            return PX_PGM_STR("Error. <pin> must be 2,3,4,5,6,7,8 or 9");
        }
    }
    else if(strlen(pin_str) == 2)
    {
        switch(pin_str[0])
        {
        case 'b':
            handle->port = &PORTB;
            handle->ddr  = &DDRB;
            handle->pin  = &PINB;
            break;

        case 'c':
            handle->port = &PORTC;
            handle->ddr  = &DDRC;
            handle->pin  = &PINC;
            break;

        case 'd':
            handle->port = &PORTD;
            handle->ddr  = &DDRD;
            handle->pin  = &PIND;
            break;

        default:
            return PX_PGM_STR("Error. <pin> prefix must be b, c or d");
        }

        if( (pin_str[1] >= '0') && (pin_str[1] <= '7')  )
        {
            handle->bit = pin_str[1] - '0';
        }
        else
        {
            return PX_PGM_STR("Error. <pin> suffix must be 0..7");
        }
    }

    return NULL;
}

static void px_report_gpio_reg(const char * name, uint8_t val, char zero, char one)
{
    uint8_t i;

    printf_P(name);
    PX_PRINTF_P(" = 0x%02x   ", val);

    for(i=(1<<7); i!=0; i>>=1)
    {
        if(val & i)
        {
            putchar(one);
        }
        else
        {
            putchar(zero);
        }
        putchar(' ');
    }
    putchar('\n');
}

static void px_report_gpio(const char * name, bool dir, bool out, bool in)
{
    printf_P(name);
    PX_PRINTF_P(" : ");
    if(dir)
    {
        PX_PRINTF_P("DIR = O, ");
        if(out)
        {
            PX_PRINTF_P("OUT = 1 ");
        }
        else
        {
            PX_PRINTF_P("OUT = 0 ");
        }
        if(in)
        {
            PX_PRINTF_P("(IN = 1)");
        }
        else
        {
            PX_PRINTF_P("(IN = 0)");
        }
    }
    else
    {
        PX_PRINTF_P("DIR = I, ");
        if(in)
        {
            PX_PRINTF_P("IN = 1 ");
        }
        else
        {
            PX_PRINTF_P("IN = 0 ");
        }

        if(out)
        {
            PX_PRINTF_P("(pull-up Enabled)");
        }
        else
        {
            PX_PRINTF_P("(pull-up Disabled)");
        }
    }
    putchar('\n');
}

static const char* px_cli_cmd_fn_gpio_info(uint8_t argc, char* argv[])
{
    PX_PRINTF_P("\n        HEX    7 6 5 4 3 2 1 O\n");
    px_report_gpio_reg(PX_PGM_STR("DDRB "), DDRB,  'I', 'O');
    px_report_gpio_reg(PX_PGM_STR("PORTB"), PORTB, '0', '1');
    px_report_gpio_reg(PX_PGM_STR("PINB "), PINB,  '0', '1');
    putchar('\n');

    px_report_gpio_reg(PX_PGM_STR("DDRC "), DDRC,  'I', 'O');
    px_report_gpio_reg(PX_PGM_STR("PORTC"), PORTC, '0', '1');
    px_report_gpio_reg(PX_PGM_STR("PINC "), PINC,  '0', '1');
    putchar('\n');

    px_report_gpio_reg(PX_PGM_STR("DDRD "), DDRD,  'I', 'O');
    px_report_gpio_reg(PX_PGM_STR("PORTD"), PORTD, '0', '1');
    px_report_gpio_reg(PX_PGM_STR("PIND "), PIND,  '0', '1');
    putchar('\n');

    px_report_gpio(PX_PGM_STR("D2"), PX_GPIO_DIR_IS_OUT(PX_GPIO_D2), 
                                     PX_GPIO_OUT_IS_HI(PX_GPIO_D2),
                                     PX_GPIO_IN_IS_HI(PX_GPIO_D2));
    px_report_gpio(PX_PGM_STR("D3"), PX_GPIO_DIR_IS_OUT(PX_GPIO_D3), 
                                     PX_GPIO_OUT_IS_HI(PX_GPIO_D3),
                                     PX_GPIO_IN_IS_HI(PX_GPIO_D3));
    px_report_gpio(PX_PGM_STR("D4"), PX_GPIO_DIR_IS_OUT(PX_GPIO_D4), 
                                     PX_GPIO_OUT_IS_HI(PX_GPIO_D4),
                                     PX_GPIO_IN_IS_HI(PX_GPIO_D4));
    px_report_gpio(PX_PGM_STR("D5"), PX_GPIO_DIR_IS_OUT(PX_GPIO_D5), 
                                     PX_GPIO_OUT_IS_HI(PX_GPIO_D5),
                                     PX_GPIO_IN_IS_HI(PX_GPIO_D5));
    px_report_gpio(PX_PGM_STR("D6"), PX_GPIO_DIR_IS_OUT(PX_GPIO_D6), 
                                     PX_GPIO_OUT_IS_HI(PX_GPIO_D6),
                                     PX_GPIO_IN_IS_HI(PX_GPIO_D6));
    px_report_gpio(PX_PGM_STR("D7"), PX_GPIO_DIR_IS_OUT(PX_GPIO_D7), 
                                     PX_GPIO_OUT_IS_HI(PX_GPIO_D7),
                                     PX_GPIO_IN_IS_HI(PX_GPIO_D7));
    px_report_gpio(PX_PGM_STR("D8"), PX_GPIO_DIR_IS_OUT(PX_GPIO_D8), 
                                     PX_GPIO_OUT_IS_HI(PX_GPIO_D8),
                                     PX_GPIO_IN_IS_HI(PX_GPIO_D8));
    px_report_gpio(PX_PGM_STR("D9"), PX_GPIO_DIR_IS_OUT(PX_GPIO_D9), 
                                     PX_GPIO_OUT_IS_HI(PX_GPIO_D9),
                                     PX_GPIO_IN_IS_HI(PX_GPIO_D9));
    return NULL;
}

static const char* px_cli_cmd_fn_gpio_cfg_out(uint8_t argc, char* argv[])
{
    const char * result;

    // <pin>
    result = px_cli_cmd_gpio_str_to_handle(&px_gpio_handle, argv[0]);
    if(result != NULL)
    {
        return result;
    }
 
    // <0|1>
    switch(argv[1][0])
    {
    case '0': px_gpio_handle.init = PX_GPIO_INIT_LO; break;
    case '1': px_gpio_handle.init = PX_GPIO_INIT_HI; break;
    default:  return "Error. <0|1> must be 0 or 1";
    }

    // Configure specified GPIO pin as an output
    px_gpio_handle.dir = PX_GPIO_DIR_OUT;
    px_gpio_init(&px_gpio_handle);

    return NULL;
}

static const char * px_cli_cmd_fn_gpio_cfg_in(uint8_t argc, char* argv[])
{
    const char * result;

    // <pin>
    result = px_cli_cmd_gpio_str_to_handle(&px_gpio_handle, argv[0]);
    if(result != NULL)
    {
        return result;
    }

    // [pu]
    if(argc > 1)
    {
        if(strcmp_P(argv[1], PX_PGM_STR("pu")) != 0)
        {
            return "Error. [pu] must be pu";
        }
        px_gpio_handle.init = PX_GPIO_INIT_PULL_UP;
    }
    else
    {
        px_gpio_handle.init = PX_GPIO_INIT_HIZ;
    }

    // Configure specified GPIO pin as an input
    px_gpio_handle.dir = PX_GPIO_DIR_IN;
    px_gpio_init(&px_gpio_handle);

    return NULL;
}

static const char * px_cli_cmd_fn_gpio_out(uint8_t argc, char* argv[])
{
    const char * result;

    // <pin>
    result = px_cli_cmd_gpio_str_to_handle(&px_gpio_handle, argv[0]);
    if(result != NULL)
    {
        return result;
    }
 
    // <0|1>
    switch(argv[1][0])
    {
    case '0': px_gpio_out_set_lo(&px_gpio_handle); break;
    case '1': px_gpio_out_set_hi(&px_gpio_handle); break;
    default:  return "Error. <0|1> must be 0 or 1";
    }

    return NULL;
}

static const char * px_cli_cmd_fn_gpio_in(uint8_t argc, char* argv[])
{
    const char * result;

    // <pin>
    result = px_cli_cmd_gpio_str_to_handle(&px_gpio_handle, argv[0]);
    if(result != NULL)
    {
        return result;
    }

    if(px_gpio_in_is_lo(&px_gpio_handle))
    {
        PX_PRINTF_P("0\n");
    }
    else
    {
        PX_PRINTF_P("1\n");
    }

    return NULL;
}

// Create CLI command structures
PX_CLI_CMD_CREATE(px_cli_cmd_gpio_info,       "info", 0, 0,   "",                       "Report GPIO info")
PX_CLI_CMD_CREATE(px_cli_cmd_gpio_cfg_out,    "co",   2, 2,   "<pin> <0|1>",            "Configure GPIO pin as output")
PX_CLI_CMD_CREATE(px_cli_cmd_gpio_cfg_in,     "ci",   1, 2,   "<pin> [pu]",             "Configure GPIO pin as input. Optionally PullUp can be enabled")
PX_CLI_CMD_CREATE(px_cli_cmd_gpio_out,        "o",    2, 2,   "<pin> <0|1>",            "Write GPIO pin output 0 or 1")
PX_CLI_CMD_CREATE(px_cli_cmd_gpio_in,         "i",    1, 1,   "<pin>",                  "Read GPIO pin input")

PX_CLI_GROUP_CREATE(px_cli_group_gpio, "gpio")
    PX_CLI_CMD_ADD(px_cli_cmd_gpio_info,       px_cli_cmd_fn_gpio_info)
    PX_CLI_CMD_ADD(px_cli_cmd_gpio_cfg_out,    px_cli_cmd_fn_gpio_cfg_out)
    PX_CLI_CMD_ADD(px_cli_cmd_gpio_cfg_in,     px_cli_cmd_fn_gpio_cfg_in)
    PX_CLI_CMD_ADD(px_cli_cmd_gpio_out,        px_cli_cmd_fn_gpio_out)
    PX_CLI_CMD_ADD(px_cli_cmd_gpio_in,         px_cli_cmd_fn_gpio_in)
PX_CLI_GROUP_END()
