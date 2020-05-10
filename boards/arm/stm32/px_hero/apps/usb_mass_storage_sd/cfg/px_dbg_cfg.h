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
// PX_DBG symbol not defined in Makefile?
#ifndef PX_DBG
/// Set flag to disable (PX_DBG=0) or enable (PX_DBG=1) debug.
#define PX_DBG 1
#endif

// PX_DBG_CFG_MSG_LEVEL symbol not defined in Makefile?
#ifndef PX_DBG_CFG_MSG_LEVEL
/**
 *  Set global debug output level.
 *  
 *  It is a bitmask that sets which debug info will be emitted. E.g.
 *  - PX_DBG_CFG_MSG_LEVEL = PX_DBG_CFG_MSG_LEVEL_ERR  : Report errors only
 *  - PX_DBG_CFG_MSG_LEVEL = (PX_DBG_CFG_MSG_LEVEL_ERR | PX_DBG_CFG_MSG_LEVEL_WARN) : Report errors + warnings
 *  - PX_DBG_CFG_MSG_LEVEL = (PX_DBG_CFG_MSG_LEVEL_ERR | PX_DBG_CFG_MSG_LEVEL_WARN | PX_DBG_CFG_MSG_LEVEL_INFO) : Report errors + warnings + info
 */
#define PX_DBG_CFG_MSG_LEVEL (PX_DBG_CFG_MSG_LEVEL_ERR | PX_DBG_CFG_MSG_LEVEL_WARN)
#endif

/// Disable (0) or Enable (1) VT100 terminal color output
#define PX_DBG_CFG_COLOR 1

/// Debug output string buffer size
#define PX_DBG_CFG_BUF_SIZE 64

/// Provide function to output debug timestamp
#if 0
// Example 1: Create timestamp using sysclk tick
#include "px_sysclk.h"
#define PX_DBG_CFG_TIMESTAMP(str)  sprintf(str, "%08lu", (uint32_t)px_sysclk_get_tick_count())
#endif
#if 1
// Example 2: Call a function in 'main.h'
#include "main.h"
#define PX_DBG_CFG_TIMESTAMP(str)  main_dbg_timestamp(str)
#endif

/// Provide function to output debug character
#if 0
// Example 1: Direct debug output to stderr
#include <stdio.h>
#define PX_DBG_CFG_PUT_CHAR(data) putc(data, stderr)
#endif
#if 0
// Example 2: Use UART and handle created in main
#include "px_uart.h"
#include "main.h"
#define PX_DBG_CFG_PUT_CHAR(data) px_uart_put_char(&main_uart_handle, data)
#endif
#if 1
// Example 3: Call function in main
#include "main.h"
#define PX_DBG_CFG_PUT_CHAR(data) main_dbg_put_char(data)
#endif

#endif // #ifndef __PX_DBG_CFG_H__
