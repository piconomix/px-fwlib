#ifndef __PX_DBG_CFG_H__
#define __PX_DBG_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2014 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          px_dbg_cfg.h : Debug module configuration
    Author(s):      Pieter Conradie
    Creation Date:  2014-01-17

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

/* _____DEFINITIONS__________________________________________________________ */
#ifndef PX_DBG
/// Flag to disable (PX_DBG=0) or enable (PX_DBG=1) debug.
#define PX_DBG 0
#endif

#ifndef PX_DBG_CFG_MSG_LEVEL
/**
 * Global debug output level.
 *  
 * It is a bitmask that sets which debug info will be emmitted. E.g.
 * - PX_DBG_CFG_MSG_LEVEL = PX_DBG_CFG_MSG_LEVEL_NONE : No debug output
 * - PX_DBG_CFG_MSG_LEVEL = PX_DBG_CFG_MSG_LEVEL_ERR  : Report errors only
 * - PX_DBG_CFG_MSG_LEVEL = (PX_DBG_CFG_MSG_LEVEL_ERR|PX_DBG_CFG_MSG_LEVEL_WARN) : Report errors + warnings
 * - PX_DBG_CFG_MSG_LEVEL = (PX_DBG_CFG_MSG_LEVEL_ERR|PX_DBG_CFG_MSG_LEVEL_WARN|PX_DBG_CFG_MSG_LEVEL_INFO) : Report errors + warnings + info
 */
#define PX_DBG_CFG_MSG_LEVEL PX_DBG_CFG_MSG_LEVEL_ERR
#else
#warning "PX_DBG_CFG_MSG_LEVEL already defined"
#endif

/// Disable (0) or Enable (1) VT100 terminal color output
#define PX_DBG_CFG_COLOR 1

/// Debug output string buffer size
#define PX_DBG_CFG_BUF_SIZE 32

#endif // #ifndef __PX_DBG_CFG_H__
