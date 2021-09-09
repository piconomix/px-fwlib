/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2012 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          CLI Commands
    Author(s):      Pieter Conradie
    Creation Date:  2008-11-01

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <string.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_cli.h"
#include "px_pgm_P.h"
#include "px_systmr.h"
#include "px_board.h"
#include "px_gpio.h"
#include "main.h"
#include "px_cli_cmds_adc.h"
#include "px_cli_cmds_dac.h"
#include "px_cli_cmds_gpio.h"
#include "px_cli_cmds_i2c.h"
#include "px_cli_cmds_lcd.h"
#include "px_cli_cmds_mem.h"
#include "px_cli_cmds_ow.h"
#include "px_cli_cmds_spi.h"
#include "px_cli_cmds_rtc.h"
#include "px_cli_cmds_uart.h"
#include "px_cli_cmds_sd.h"
#include "px_cli_cmds_sf.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("cli");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */
static const char* px_cli_cmd_reset_fn(uint8_t argc, char* argv[]) PX_ATTR_NORETURN;

/* _____LOCAL FUNCTIONS______________________________________________________ */
static const char* px_cli_cmd_reset_fn(uint8_t argc, char* argv[])
{
    NVIC_SystemReset();
    for(;;)
    {
        ;
    }
}

static const char* px_cli_cmd_led_fn(uint8_t argc, char* argv[])
{
    // <on|off>
    switch(px_cli_util_argv_to_option(0, "on\0off\0"))
    {
    case 0: 
        PX_USR_LED_ON();
        break;
    case 1: 
        PX_USR_LED_OFF();
        break;
    default:
        return "Error: Invalid parameter";
    }

    return NULL;
}
static const char* px_cli_cmd_beep_fn(uint8_t argc, char* argv[])
{
    uint16_t delay_ms;
    uint16_t freq_hz;

    // <ms>
    if(px_cli_util_argv_to_u16(0, 0, 10000))
    {
        delay_ms = px_cli_argv_val.u16;        
    }
    else
    {
        return "Error: Delay must be from 0 to 10000 ms";
    }

    // [freq Hz]
    if(argc > 1)
    {
        if(px_cli_util_argv_to_u16(1, 100, 10000))
        {
            freq_hz = px_cli_argv_val.u16;        
        }
        else
        {
            return "Error: Frequency must be from 100 to 10000 Hz";
        }
    }
    else
    {
        freq_hz = 4000;
    }

    px_board_buzzer_on(freq_hz);
    px_board_delay_ms(delay_ms);
    px_board_buzzer_off();

    return NULL;
}

static const char* px_cli_cmd_delay_fn(uint8_t argc, char* argv[])
{
    uint16_t delay;

    // <delay>
    if(!px_cli_util_argv_to_u16(0, 0, 65535))
    {
        return "Error. <delay> must be 0 to 65535";
    }
    delay = px_cli_argv_val.u16;

    // [ms|s]
    if(argc > 1)
    {
        switch(px_cli_util_argv_to_option(1, "ms\0s\0"))
        {
        case 0:
            while(delay != 0)
            {
                px_board_delay_us(1000);
                delay--;
            }
            break;
        case 1:
            while(delay != 0)
            {
                px_board_delay_ms(1000);
                delay--;
            }
            break;
        default:
            return "Error: <ms|s> must be ms or s";
        }
    }
    else
    {
        while(delay != 0)
        {
            px_board_delay_us(1000);
            delay--;
        }
    }

    return NULL;
}

// Create CLI command structures
PX_CLI_CMD_CREATE(px_cli_cmd_reset, "rst",      0, 0,   "",                         "Reset microcontroller")
PX_CLI_CMD_CREATE(px_cli_cmd_beep,  "beep",     1, 2,   "<ms> [freq Hz]",           "Beep for specified number of milliseconds. Frequency can also be specified")
PX_CLI_CMD_CREATE(px_cli_cmd_led,   "led",      1, 1,   "<on|off>",                 "Switch LED on or off")
PX_CLI_CMD_CREATE(px_cli_cmd_delay, "delay",    1, 2,   "<delay> [ms|s]",           "Delay in milliseconds or seconds")
PX_CLI_CMD_CREATE(px_cli_cmd_help,  "help",     0, 1,   "[cmd(s) starts with...]",  "Display list of commands with help. Optionally the list can be reduced.")

// Add CLI commands to CLI list
PX_CLI_CMD_LIST_CREATE(px_cli_cmd_list)
    PX_CLI_CMD_ADD     (px_cli_cmd_led,       px_cli_cmd_led_fn)
    PX_CLI_CMD_ADD     (px_cli_cmd_beep,      px_cli_cmd_beep_fn)
    PX_CLI_GROUP_ADD   (px_cli_group_lcd)
    PX_CLI_GROUP_ADD   (px_cli_group_adc)
    PX_CLI_GROUP_ADD   (px_cli_group_dac)
    PX_CLI_GROUP_ADD   (px_cli_group_gpio)
    PX_CLI_GROUP_ADD   (px_cli_group_uart)
    PX_CLI_GROUP_ADD   (px_cli_group_i2c)
    PX_CLI_GROUP_ADD   (px_cli_group_spi)
    PX_CLI_GROUP_ADD   (px_cli_group_ow)
    PX_CLI_GROUP_ADD   (px_cli_group_sf)
    PX_CLI_GROUP_ADD   (px_cli_group_sd)
    PX_CLI_GROUP_ADD   (px_cli_group_rtc)
    PX_CLI_GROUP_ADD   (px_cli_group_mem)
    PX_CLI_CMD_ADD     (px_cli_cmd_delay,     px_cli_cmd_delay_fn)
    PX_CLI_CMD_ADD     (px_cli_cmd_reset,     px_cli_cmd_reset_fn)
    PX_CLI_CMD_ADD     (px_cli_cmd_help,      px_cli_cmd_help_fn)
PX_CLI_CMD_LIST_END()
