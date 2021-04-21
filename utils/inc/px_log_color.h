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
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
 
    Title:          px_log_color.h : Color definitions for debug logging module
    Author(s):      Pieter Conradie
    Creation Date:  2021-04-20

============================================================================= */

/**
 *  @addtogroup PX_LOG
 */
/// @{

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

#define PX_LOG_CR           PX_VT100_RST_ALL_ATTRS      ///< Color Reset

#define PX_LOG_CF_BLACK     PX_VT100_SET_FG_BLACK       ///< Color Foreground BLACK
#define PX_LOG_CF_RED       PX_VT100_SET_FG_RED         ///< Color Foreground RED
#define PX_LOG_CF_GREEN     PX_VT100_SET_FG_GREEN       ///< Color Foreground GREEN
#define PX_LOG_CF_YELLOW    PX_VT100_SET_FG_YELLOW      ///< Color Foreground YELLOW
#define PX_LOG_CF_BLUE      PX_VT100_SET_FG_BLUE        ///< Color Foreground BLUE
#define PX_LOG_CF_MAGENTA   PX_VT100_SET_FG_MAGENTA     ///< Color Foreground MAGENTA
#define PX_LOG_CF_CYAN      PX_VT100_SET_FG_CYAN        ///< Color Foreground CYAN
#define PX_LOG_CF_WHITE     PX_VT100_SET_FG_WHITE       ///< Color Foreground WHITE

#define PX_LOG_CB_BLACK     PX_VT100_SET_BG_BLACK       ///< Color Background BLACK
#define PX_LOG_CB_RED       PX_VT100_SET_BG_RED         ///< Color Background RED
#define PX_LOG_CB_GREEN     PX_VT100_SET_BG_GREEN       ///< Color Background GREEN
#define PX_LOG_CB_YELLOW    PX_VT100_SET_BG_YELLOW      ///< Color Background YELLOW
#define PX_LOG_CB_BLUE      PX_VT100_SET_BG_BLUE        ///< Color Background BLUE
#define PX_LOG_CB_MAGENTA   PX_VT100_SET_BG_MAGENTA     ///< Color Background MAGENTA
#define PX_LOG_CB_CYAN      PX_VT100_SET_BG_CYAN        ///< Color Background CYAN
#define PX_LOG_CB_WHITE     PX_VT100_SET_BG_WHITE       ///< Color Background WHITE

#else

#define PX_LOG_CR
#define PX_LOG_CF_BLACK
#define PX_LOG_CF_RED
#define PX_LOG_CF_GREEN
#define PX_LOG_CF_YELLOW
#define PX_LOG_CF_BLUE
#define PX_LOG_CF_MAGENTA
#define PX_LOG_CF_CYAN
#define PX_LOG_CF_WHITE
#define PX_LOG_CB_BLACK
#define PX_LOG_CB_RED
#define PX_LOG_CB_GREEN
#define PX_LOG_CB_YELLOW
#define PX_LOG_CB_BLUE
#define PX_LOG_CB_MAGENTA
#define PX_LOG_CB_CYAN
#define PX_LOG_CB_WHITE

#endif

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */

/* _____MACROS_______________________________________________________________ */

/// @}

#endif
