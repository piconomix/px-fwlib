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
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          px_log_cfg.h : Debug module configuration
    Author(s):      Pieter Conradie
    Creation Date:  2014-01-17

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"

/* _____DEFINITIONS__________________________________________________________ */
// PX_LOG symbol not defined in Makefile?
#ifndef PX_LOG
/// Disable (0) or Enable (1) debug log output
#define PX_LOG 0
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

/// Disable (0) or Enable (1) run time log filter
#define PX_LOG_CFG_FILTER 0

/// Disable (0) or Enable (1) VT100 terminal color output
#define PX_LOG_CFG_COLOR 1

/// Debug output string buffer size
#define PX_LOG_CFG_BUF_SIZE 64

/// Provide function to output log timestamp
#if 0
// Example 1: Create timestamp using sysclk tick
#include "px_sysclk.h"
#define PX_LOG_CFG_TIMESTAMP(str)  sprintf(str, "%08lu", (uint32_t)px_sysclk_get_tick_count())
#endif
#if 1
// Example 2: Call a function in 'main.h'
#include "main.h"
#define PX_LOG_CFG_TIMESTAMP(str)  main_log_timestamp(str)
#endif

/// Provide function to output log character
#if 0
// Example 1: Direct debug output to stderr
#include <stdio.h>
#define PX_LOG_CFG_PUTCHAR(data) putc(data, stderr)
#endif
#if 0
// Example 2: Use UART and handle created in main
#include "px_uart.h"
#include "main.h"
#define PX_LOG_CFG_PUTCHAR(data) px_uart_putchar(&main_uart_handle, data)
#endif
#if 1
// Example 3: Call function in main
#include "main.h"
#define PX_LOG_CFG_PUTCHAR(data) main_log_putchar(data)
#endif

/// Provide function to output log string (optional to increase speed)
#if 1
// Example 1: Call function in main.c
#include "main.h"
#define PX_LOG_CFG_PRINT(str) main_log_print(str)
#endif

/// @}
#endif
