#ifndef __PX_DBG_CFG_H__
#define __PX_DBG_CFG_H__
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
    
    Title:          px_dbg_cfg.h : Debug module configuration
    Author(s):      Pieter Conradie
    Creation Date:  2014-01-17

============================================================================= */

/** 
 *  @addtogroup PX_DBG
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

/* _____DEFINITIONS__________________________________________________________ */
// PX_DBG symbol not defined in Makefile?
#ifndef PX_DBG
/// Set flag to disable (PX_DBG=0) or enable (PX_DBG=1) debug.
#define PX_DBG 0
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
#define PX_DBG_CFG_MSG_LEVEL PX_DBG_CFG_MSG_LEVEL_ALL
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
#if 0
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
#if 0
// Example 3: Call function in main
#include "main.h"
#define PX_DBG_CFG_PUT_CHAR(data) main_dbg_put_char(data)
#endif

/// @}
#endif // #ifndef __PX_DBG_CFG_H__
