#ifndef __PX_RTC_UTIL_CFG_H__
#define __PX_RTC_UTIL_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2014 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          px_rtc.h : Software Real Time Clock module
    Author(s):      Pieter Conradie
    Creation Date:  2014-06-22

============================================================================= */

/** 
 *  @addtogroup PX_RTC_UTIL
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

/* _____DEFINITIONS__________________________________________________________ */
/// Option to also keep track of seconds elapsed since Y2K (2000-01-01 00:00:00)
#define PX_RTC_UTIL_CFG_OPTION_SEC_SINCE_Y2K    1

/// Option to enable periodic flags (minute, hour and day)
#define PX_RTC_UTIL_CFG_OPTION_PERIODIC_FLAGS   1

/// @}
#endif // #ifndef __PX_RTC_UTIL_CFG_H__
