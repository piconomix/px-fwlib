#ifndef __PX_SYSTMR_H__
#define __PX_SYSTMR_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2008 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          px_systmr.h : Polled software timers
    Author(s):      Pieter Conradie
    Creation Date:  2008-02-11

=========================================================================== */

/** 
 *  @ingroup UTILS
 *  @defgroup PX_SYSTMR px_systmr.h : Polled software timers
 *  
 *  Non-blocking software timers that have to be polled to determine if they
 *  have expired.
 *  
 *  File(s):
 *  - utils/inc/px_systmr.h
 *  - utils/src/px_systmr.c
 *  
 *  These timers are suitable for applications that are not timing critical.
 *  A global counter is incremented with each clock tick and this value is
 *  polled to determine if a timer has expired. Provision is made for counter
 *  roll-over.
 *  
 *  This module depends on a system clock module, e.g. @ref AVR_SYSCLK to
 *  return a counter that is incremented with every system clock tick. The
 *  number of ticks per second (Hz) is defined with #PX_SYSTMR_TICKS_PER_SEC.
 *  
 *  Example:
 *  
 *  @include utils/test/px_systmr_test.c
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_sysclk.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
/// The number of timer ticks per second
#define PX_SYSTMR_TICKS_PER_SEC   PX_SYSCLK_CFG_TICKS_PER_SEC

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Size definition of the tick counter
typedef px_sysclk_ticks_t px_systmr_ticks_t;

/// Timer state
typedef enum
{
    PX_SYSTMR_STOPPED = 0,
    PX_SYSTMR_STARTED,
    PX_SYSTMR_EXPIRED,
} px_systmr_state_t;

/// Structure to track state of a timer
typedef struct
{
    px_systmr_state_t state;           ///< State of timer: STOPPED, STARTED or EXPIRED
    px_systmr_ticks_t start_tick;      ///< Tick when timer started
    px_systmr_ticks_t delay_in_ticks;  ///< Timer delay, used for subsequent timer restarts/resets
} px_systmr_t;

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/** 
 *  Start a timer.
 *  
 *  @param[in,out]  systmr          Pointer to a timer object
 *  @param[in]      delay_in_ticks  Delay in timer ticks
 */
void px_systmr_start(px_systmr_t * systmr, const px_systmr_ticks_t delay_in_ticks);

/**
 *  See if a timer has been started.
 *  
 *  @param[in]  systmr  Pointer to a timer object
 *  
 *  @retval true        timer started
 *  @retval false       timer stopped or expired
 */
bool px_systmr_has_started(const px_systmr_t * systmr);

/** 
 *  See if a timer has expired.
 *  
 *  @param[in,out]  systmr  Pointer to a timer object
 *  
 *  @retval true            timer expired
 *  @retval false           timer not expired or timer stopped
 */
bool px_systmr_has_expired(px_systmr_t * systmr);

/** 
 *  Stop a running timer.
 *  
 *  @param[in,out] systmr    Pointer to a timer object
 */
void px_systmr_stop(px_systmr_t * systmr);

/** 
 *  Restart a timer with the delay set with px_systmr_start().
 *  
 *  The timer will expire from the current timer tick + systmr->delay_in_ticks.
 *  
 *  @param[in,out]  systmr   Pointer to a timer object
 */    
void px_systmr_restart(px_systmr_t * systmr);

/** 
 *  Reset a timer with the delay set with px_systmr_start().
 *  
 *  The timer will expire on systmr->start_tick + systmr->delay_in_ticks.
 *  
 *  Use this function instead of px_systmr_restart() for periodic timers,
 *  because the frequency will not drift over time. An error may accumulate if
 *  there is a delay between px_systmr_has_expired() and px_systmr_restart().
 *  Thus the prefered usage for a periodic timer is:
 *  
 *      @code{.c}
 *      for(;;)
 *      {
 *          // Wait until timer has expired
 *          while(px_systmr_has_expired(&systmr) == false)
 *          {
 *              ;
 *          }
 *          // Reset periodic timer
 *          px_systmr_reset(&systmr);
 *          // Do something...
 *      }
 *      @endcode
 *  
 *  @param[in,out]  systmr   Pointer to a timer object
 */    
void px_systmr_reset(px_systmr_t * systmr);

/** 
 *  Blocking wait for specified number of ticks.
 *  
 *  @param[in]  delay_in_ticks    Delay in timer ticks
 */ 
void px_systmr_wait(const px_systmr_ticks_t delay_in_ticks);

/** 
 *  Return the number of ticks that have elapsed sinced the timer has been started.
 *  
 *  @param[in,out]  systmr   Pointer to a timer object
 *  
 *  @return px_systmr_ticks_t Number of ticks elapsed
 */ 
px_systmr_ticks_t px_systmr_ticks_elapsed(px_systmr_t * systmr);

/* _____MACROS_______________________________________________________________ */
/** 
 *  Macro used to convert a timeout in milliseconds to timer ticks
 *  @param[in] delay_in_ms Delay in milliseconds
 *  @return    Delay in timer ticks
 */
#define PX_SYSTMR_MS_TO_TICKS(delay_in_ms)    \
     PX_UDIV_ROUND((delay_in_ms) * PX_SYSTMR_TICKS_PER_SEC, 1000ul)

/// @}
#ifdef __cplusplus
}
#endif

#endif // #ifndef __PX_SYSTMR_H__
