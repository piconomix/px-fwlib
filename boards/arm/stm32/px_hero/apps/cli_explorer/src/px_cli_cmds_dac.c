/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
 
    Title:          px_cli_cmds_dac.h : CLI commands for DAC
    Author(s):      Pieter Conradie
    Creation Date:  2018-11-15

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdlib.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_cli.h"
#include "px_dac.h"
#include "px_board.h"
#include "main.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("cli_cmds_dac");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static const char* px_cli_cmd_fn_dac_o(uint8_t argc, char* argv[])
{
    uint16_t val;
    
    // <val>
    if(!px_cli_util_argv_to_u16(0, 0, 4095))
    {
        return "Error. <val> must be 0 to 4095";
    }
    val = px_cli_argv_val.u16;

    // Output DAC value
    px_dac_wr(&px_dac_handle, PX_DAC_CHANNEL_1, val);

    return NULL;
}

// Create CLI command structures
PX_CLI_CMD_CREATE(px_cli_cmd_dac_o,       "o",            1, 1,   "<val>", "Set DAC output value")

PX_CLI_GROUP_CREATE(px_cli_group_dac, "dac")
    PX_CLI_CMD_ADD(px_cli_cmd_dac_o,       px_cli_cmd_fn_dac_o)
PX_CLI_GROUP_END()
