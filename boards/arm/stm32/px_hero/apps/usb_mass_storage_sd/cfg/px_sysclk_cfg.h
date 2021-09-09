#ifndef __PX_SYSCLK_CFG_H__
#define __PX_SYSCLK_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_sysclk.h : System Clock using the SysTick peripheral
    Author(s):      Pieter Conradie
    Creation Date:  2018-03-06

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
//#include "px_rtc.h"

/* _____DEFINITIONS__________________________________________________________ */
/// The number of system clock ticks per second
#define PX_SYSCLK_CFG_TICKS_PER_SEC 1000ul

/// Enable (1) or disable (0) support for STM32Cube HAL 1 ms SysTick timer functionality
#define PX_SYSCLK_CFG_STMCUBE_HAL_TMR 1

/// Specify periodic timeout (in sysclk ticks)
//#define PX_SYSCLK_CFG_TIMEOUT_PERIOD_TICKS     PX_SYSCLK_CFG_TICKS_PER_SEC

/// Specify function to call on periodic timeout
//#define PX_SYSCLK_CFG_ON_PERIODIC_TIMEOUT()    px_rtc_on_tick()

/// @}
#endif
