#ifndef __PX_SYSCLK_H__
#define __PX_SYSCLK_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2006 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_sysclk.h : System Clock using a TMRx peripheral
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31

=========================================================================== */

/** 
 *  @ingroup AVR
 *  @defgroup AVR_SYSCLK px_sysclk.h : System Clock using a TMRx peripheral
 *  
 *  Driver to initalise TMRx so that a periodic system clock tick interrupt is
 *  generated.
 *  
 *  File(s):
 *  - arch/avr/inc/px_sysclk.h
 *  - arch/avr/inc/px_sysclk_cfg_template.h
 *  - arch/avr/src/px_sysclk.c
 *  
 *  px_sysclk_init() configures TMRx to expire every 1/PX_SYSCLK_TICKS_PER_SEC second
 *  and generate an interrupt during which a clock tick counter is incremented.
 *  px_sysclk_get_tick_count() must be called to fetch a copy of the clock tick
 *  counter in an interrupt safe way (atomically).
 *  
 *  A periodic timeout function can also be called by the system clock timer
 *  interrupt, for example a software RTC can be implemented by calling an
 *  RTC update function once a second. The period is specified in 'px_sysclk_cfg.h'
 *  with PX_SYSCLK_TIMEOUT_PERIOD_TICKS and the function to call with.
 *  
 *  @tip_s
 *  See @ref PX_SYSTMR which builds on @ref AVR_SYSCLK to provide multiple software
 *  timers. Example:
 *  @tip_e
 *  
 *  @include utils/test/px_systmr_test.c
 *
 *  @{
 */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"

// Include project specific configuration. See "px_sysclk_cfg_template.h"
#include "px_sysclk_cfg.h"

// Check that all project specific options have been specified in "px_sysclk_cfg.h"
#if (   !defined(PX_SYSCLK_CFG_TICKS_PER_SEC) \
     || !defined(PX_SYSCLK_CFG_USE_TMRX     )  )
      )
#error "One or more options not defined in 'px_sysclk_cfg.h'"
#endif

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Size definition of the tick counter
typedef uint16_t px_sysclk_ticks_t;

/* _____DEFINITIONS__________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/** 
 *  Start system clock (one clock tick every 1/PX_SYSCLK_TICKS_PER_SEC seconds).
 */
void px_sysclk_init(void);

/**
 * Disable system clock
 *  
 * @return true     System clock was enabled
 * @return false    System clock was disabled
 */
bool px_sysclk_disable(void);

/// Enable system clock
void px_sysclk_enable(void);

/**
 * Return number of ticks since system clock started.
 *  
 * @return px_sysclk_ticks_t   Number of clock ticks 
 */
px_sysclk_ticks_t px_sysclk_get_tick_count(void);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
