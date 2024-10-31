#ifndef __PX_LOG_FT_CFG_H__
#define __PX_LOG_FT_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2021 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          px_log_ft.h : Flow Trace logging module configuration
    Author(s):      Pieter Conradie
    Creation Date:  2021-05-31

============================================================================= */

/** 
 *  @addtogroup PX_LOG_FT
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"

/* _____DEFINITIONS__________________________________________________________ */
// PX_LOG_FT symbol not defined in Makefile?
#ifndef PX_LOG_FT
/// Disable (0) or Enable (1) debug flow trace
#define PX_LOG_FT 0
#endif

/// Debug output string buffer size
#define PX_LOG_FT_CFG_BUF_SIZE 16

/// Customized name values (must be sequential starting at 0, e.g. 0, 1, 2, 3, ...)
typedef enum
{
    PX_LOG_FT_NAME_NONE     = 0,
    PX_LOG_FT_NAME_MAIN     = 1,
} px_log_ft_name_t;

/// Optional: provide name strings for each value (must be sequential starting at 0, e.g. 0, 1, 2, 3, ...)
#if 0
#define PX_LOG_FT_CFG_NAMES() \
static const char * px_log_ft_name_str[] = \
{ \
    "", \
    "MAIN", \
};
#endif

/// @}
#endif
