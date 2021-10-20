#ifndef __PX_ASYNC_TMR_CFG_H__
#define __PX_ASYNC_TMR_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2014 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          px_async_tmr_cfg.h : Asynchronous TMR configuration
    Author(s):      Pieter Conradie
    Creation Date:  2014-06-23

============================================================================= */

/** 
 *  @addtogroup AVR_ASYNC_TMR
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_rtc.h"

/* _____DEFINITIONS__________________________________________________________ */

/// Define timeout period
#define ASYNC_TMR_CFG_PERIOD_MS     1000

/// Define handler function to call when timeout interrupt occurs
#define ASYNC_TMR_CFG_ON_TICK()     px_rtc_on_tick()

/// @}
#endif
