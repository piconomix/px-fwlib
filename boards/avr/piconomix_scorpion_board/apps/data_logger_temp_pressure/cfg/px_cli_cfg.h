#ifndef __PX_CLI_CFG_H__
#define __PX_CLI_CFG_H__
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
    
    Title:          px_cli_cfg.h : CLI Peripheral Driver configuration
    Author(s):      Pieter Conradie
    Creation Date:  2014-02-11

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

/* _____DEFINITIONS _________________________________________________________ */
/// Maximum number of arguments (including command)
#define PX_CLI_CFG_ARGV_MAX            36

/// Define the maximum length of a command line
#define PX_CLI_CFG_LINE_LENGTH_MAX     128

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
#define PX_CLI_CFG_HISTORY_SIZE        128

/// Display help strings (1) or remove help strings (0) to reduce code size
#define PX_CLI_CFG_DISP_HELP_STR       1

/// Specify maximum command name string length (not zero) or calculate run time (zero)
#define PX_CLI_CFG_NAME_STR_MAX_SIZE   0

/// Specify maximum param string length (not zero) or calculate run time (zero)
#define PX_CLI_CFG_PARAM_STR_MAX_SIZE  0

#endif // #ifndef __PX_CLI_CFG_H__
