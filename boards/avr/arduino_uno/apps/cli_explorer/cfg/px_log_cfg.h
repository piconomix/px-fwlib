#ifndef __PX_LOG_CFG_H__
#define __PX_LOG_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2014 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          px_log_cfg.h : Debug module configuration
    Author(s):      Pieter Conradie
    Creation Date:  2014-01-17

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

/* _____DEFINITIONS__________________________________________________________ */
#ifndef PX_LOG
/// Flag to disable (PX_LOG=0) or enable (PX_LOG=1) debug.
#define PX_LOG 1
#endif

// PX_LOG_CFG_LEVEL symbol not defined in Makefile?
#ifndef PX_LOG_CFG_LEVEL
/**
 *  Set *COMPILE TIME* log output level.
 *
 *  The levels are sorted in increasing levels of verbosity. For example if
 *  PX_LOG_CFG_LEVEL is set to PX_LOG_LEVEL_INFO then all ERROR, WARNING and
 *  INFO messages will be included in the code but DEBUG and VERBOSE messages
 *  will not.
 */
#define PX_LOG_CFG_LEVEL PX_LOG_LEVEL_INFO
#endif

/// Disable (0) or Enable (1) VT100 terminal color output
#define PX_LOG_CFG_COLOR 1

/// Debug output string buffer size
#define PX_LOG_CFG_BUF_SIZE 32

#endif // #ifndef __PX_LOG_CFG_H__
