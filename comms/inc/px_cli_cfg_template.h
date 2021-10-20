#ifndef __PX_CLI_CFG_H__
#define __PX_CLI_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2012 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          px_cli_cfg.h : CLI Peripheral Driver configuration
    Author(s):      Pieter Conradie
    Creation Date:  2013-01-15

============================================================================= */

/**
 *  @addtogroup PX_CLI
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

/* _____DEFINITIONS__________________________________________________________ */
/// Maximum number of arguments (including command)
#define PX_CLI_CFG_ARGV_MAX            16

/// Define the maximum length of a command line
#define PX_CLI_CFG_LINE_LENGTH_MAX     64

/// Define the maximum depth of command strings
#define PX_CLI_CFG_TREE_DEPTH_MAX      2

/**
 *  Define the command line history size (use 0 to remove history).
 *
 *  It must be able to accept at least one cmd line completely!
 *  PX_CLI_HISTORY_SIZE must be equal or less than 65536 (256 best value).
 *  If not zero, PX_CLI_HISTORY_SIZE must also be equal or greater than
 *  PX_CLI_LINE_LENGTH_MAX.
 */
#define PX_CLI_CFG_HISTORY_SIZE        64

/// Display help strings (1) or remove help strings (0) to reduce code size
#define PX_CLI_CFG_DISP_HELP_STR       1

/// Specify maximum command name string length (not zero) or calculate run time (zero)
#define PX_CLI_CFG_NAME_STR_MAX_SIZE   16

/// Specify maximum param string length (not zero) or calculate run time (zero)
#define PX_CLI_CFG_PARAM_STR_MAX_SIZE  42

/// Disable (0) or Enable (1) VT100 terminal color output
#define PX_CLI_CFG_COLOR               1

/// Disable (0) or Enable (1) echo of characters typed
#define PX_CLI_CFG_ECHO_CHARS          1

/// Specify ENTER character (Carriage Return "\r" or Line Feed "\n") that signifies the end of a command
#define PX_CLI_CFG_CHAR_ENTER          PX_VT100_CHAR_CR
//#define PX_CLI_CFG_CHAR_ENTER          PX_VT100_CHAR_LF

/// @}
#endif
