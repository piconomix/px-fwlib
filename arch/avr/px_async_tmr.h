#ifndef __PX_ASYNC_TMR_H__
#define __PX_ASYNC_TMR_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2014 Pieter Conradie <https://piconomix.com>
 
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.

    Title:          px_async_tmr.h : Asynchronous TMR connected to 32 kHz crystal
    Author(s):      Pieter Conradie
    Creation Date:  2014-06-23

============================================================================= */
/** 
 *  @ingroup AVR
 *  @defgroup AVR_ASYNC_TMR px_async_tmr.h : Asynchronous TMR connected to 32 kHz crystal
 *  
 *  This component provides basic time, calender and alarm functionality.
 *  
 *  File(s):
 *  - arch/avr/px_async_tmr.h
 *  - arch/avr/px_async_tmr_cfg_template.h
 *  - arch/avr/px_async_tmr.c
 *  
 *  Asynchronous 8-bit TMRx operation is selected to use an external
 *  32768 Hz crystal. TMRx is configured to generate an interrupt and call a
 *  handler function.
 *  
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

// Include project specific config. See "px_async_tmr_cfg_template.h"
#include "px_async_tmr_cfg.h"

// Check that all project specific options have been specified in "px_async_tmr_cfg.h"
#if (   !defined(ASYNC_TMR_CFG_PERIOD_MS) \
     || !defined(ASYNC_TMR_CFG_ON_TICK  )  )
#error "One or more options not defined in 'px_async_tmr_cfg.h'"
#endif

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS _________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initalises TMRx to use an external 32768 Hz crystal.
 *  
 *  TMRx is configured to generate an interrupt once a second, during which the
 *  date and time is updated and checked if an alarm has occurred.
 */
void px_async_tmr_init(void);

/** 
 *   Enable TMRx interrupt.
 */
void px_async_tmr_int_enable(void);

/** 
 *   Disable TMRx interrupt.
 */
bool px_async_tmr_int_disable(void);

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif // #ifndef __PX_ASYNC_TMR_H__
