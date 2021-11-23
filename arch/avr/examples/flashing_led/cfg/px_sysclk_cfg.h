#ifndef __PX_SYSCLK_CFG_H__
#define __PX_SYSCLK_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2013 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_sysclk_cfg.h : System Clock using a TMRx peripheral configuration
    Author(s):      Pieter Conradie
    Creation Date:  2013-01-16

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"

/* _____DEFINITIONS__________________________________________________________ */
/// The number of system clock ticks per second
#define PX_SYSCLK_CFG_TICKS_PER_SEC 100ul

/// Specify which TMR peripheral must be used
#define PX_SYSCLK_CFG_USE_TMRX 2

/// Specify TMR Prescaler
#define PX_SYSCLK_CFG_TMR_PRESCALER 1024

/// Specify periodic timeout (in sysclk ticks)
#define PX_SYSCLK_CFG_TIMEOUT_PERIOD_TICKS     PX_SYSCLK_CFG_TICKS_PER_SEC 

#endif
