/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2015 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          px_cli_cmds_led.h : CLI commands for LED
    Author(s):      Pieter Conradie
    Creation Date:  2015-09-23

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_cli.h"
#include "px_cli_cmds_gpio.h"
#include "px_board.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("cli");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static const char* px_cli_cmd_fn_led_on(uint8_t argc, char* argv[])
{
    PX_LED_ON();

    return NULL;
}

static const char* px_cli_cmd_fn_led_off(uint8_t argc, char* argv[])
{
    PX_LED_OFF();

    return NULL;
}

// Create CLI command structures
PX_CLI_CMD_CREATE(px_cli_cmd_led_on,  "on", 0, 0,   "",   "Enable LED  (if SPI is not used)")
PX_CLI_CMD_CREATE(px_cli_cmd_led_off, "off",0, 0,   "",   "Disable LED (if SPI is not used)")

PX_CLI_GROUP_CREATE(px_cli_group_led, "led")
    PX_CLI_CMD_ADD(px_cli_cmd_led_on,     px_cli_cmd_fn_led_on)
    PX_CLI_CMD_ADD(px_cli_cmd_led_off,    px_cli_cmd_fn_led_off)    
PX_CLI_GROUP_END()
