/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2015 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          CLI Commands
    Author(s):      Pieter Conradie
    Creation Date:  2015-01-31

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <avr/io.h>
#include <avr/wdt.h>
#include <string.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_cli.h"
#include "px_pgm_P.h"
#include "px_systmr.h"
#include "px_board.h"
#include "px_gpio.h"
#include "main.h"
#include "px_cli_cmds_adc.h"
#include "px_cli_cmds_led.h"
#include "px_cli_cmds_gpio.h"
#include "px_cli_cmds_i2c.h"
#include "px_cli_cmds_spi.h"
#include "px_cli_cmds_rtc.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("cli");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
// Create CLI command structures
PX_CLI_CMD_CREATE(px_cli_cmd_help,  "help",     0, 1,   "[cmd(s) starts with...]",  "Display list of commands with help. Optionally the list can be reduced.")

// Add CLI commands to CLI list
PX_CLI_CMD_LIST_CREATE(px_cli_cmd_list)
    PX_CLI_GROUP_ADD   (px_cli_group_led)
    PX_CLI_GROUP_ADD   (px_cli_group_gpio)
    PX_CLI_GROUP_ADD   (px_cli_group_adc)
    PX_CLI_GROUP_ADD   (px_cli_group_i2c)
    PX_CLI_GROUP_ADD   (px_cli_group_spi)
    PX_CLI_GROUP_ADD   (px_cli_group_rtc)
    PX_CLI_CMD_ADD     (px_cli_cmd_help,      px_cli_cmd_help_fn)
PX_CLI_CMD_LIST_END()
