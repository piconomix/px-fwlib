#ifndef __PX_LOG_COLOR_H__
#define __PX_LOG_COLOR_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
 
    Title:          px_log_color.h : Color definitions for debug logging module
    Author(s):      Pieter Conradie
    Creation Date:  2021-04-20

============================================================================= */

/**
 *  @addtogroup PX_LOG
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#ifndef PX_LOG_CFG_COLOR
#error "PX_LOG_CFG_COLOR must be defined before including this file"
#endif

#if PX_LOG_CFG_COLOR
// Include VT100 escape code definitions
#include "px_vt100.h"
#endif

/* _____DEFINITIONS__________________________________________________________ */
#if PX_LOG_CFG_COLOR

#define PX_LOG_CR           PX_VT100_ATTR_RST       ///< Color Reset

#define PX_LOG_FG_BLACK     PX_VT100_FG_BLACK       ///< Color Foreground BLACK
#define PX_LOG_FG_RED       PX_VT100_FG_RED         ///< Color Foreground RED
#define PX_LOG_FG_GREEN     PX_VT100_FG_GREEN       ///< Color Foreground GREEN
#define PX_LOG_FG_YELLOW    PX_VT100_FG_YELLOW      ///< Color Foreground YELLOW
#define PX_LOG_FG_BLUE      PX_VT100_FG_BLUE        ///< Color Foreground BLUE
#define PX_LOG_FG_MAGENTA   PX_VT100_FG_MAGENTA     ///< Color Foreground MAGENTA
#define PX_LOG_FG_CYAN      PX_VT100_FG_CYAN        ///< Color Foreground CYAN
#define PX_LOG_FG_WHITE     PX_VT100_FG_WHITE       ///< Color Foreground WHITE

#define PX_LOG_BG_BLACK     PX_VT100_BG_BLACK       ///< Color Background BLACK
#define PX_LOG_BG_RED       PX_VT100_BG_RED         ///< Color Background RED
#define PX_LOG_BG_GREEN     PX_VT100_BG_GREEN       ///< Color Background GREEN
#define PX_LOG_BG_YELLOW    PX_VT100_BG_YELLOW      ///< Color Background YELLOW
#define PX_LOG_BG_BLUE      PX_VT100_BG_BLUE        ///< Color Background BLUE
#define PX_LOG_BG_MAGENTA   PX_VT100_BG_MAGENTA     ///< Color Background MAGENTA
#define PX_LOG_BG_CYAN      PX_VT100_BG_CYAN        ///< Color Background CYAN
#define PX_LOG_BG_WHITE     PX_VT100_BG_WHITE       ///< Color Background WHITE

#else

#define PX_LOG_CR
#define PX_LOG_FG_BLACK
#define PX_LOG_FG_RED
#define PX_LOG_FG_GREEN
#define PX_LOG_FG_YELLOW
#define PX_LOG_FG_BLUE
#define PX_LOG_FG_MAGENTA
#define PX_LOG_FG_CYAN
#define PX_LOG_FG_WHITE
#define PX_LOG_BG_BLACK
#define PX_LOG_BG_RED
#define PX_LOG_BG_GREEN
#define PX_LOG_BG_YELLOW
#define PX_LOG_BG_BLUE
#define PX_LOG_BG_MAGENTA
#define PX_LOG_BG_CYAN
#define PX_LOG_BG_WHITE

#endif

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */

/* _____MACROS_______________________________________________________________ */

/// @}
#endif
