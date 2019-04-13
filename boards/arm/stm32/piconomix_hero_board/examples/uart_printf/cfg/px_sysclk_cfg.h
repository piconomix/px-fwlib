#ifndef __PX_SYSCLK_CFG_H__
#define __PX_SYSCLK_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2013 Pieter Conradie <https://piconomix.com>
 
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
    
    Title:          px_sysclk_cfg.h : System Clock using a TMRx peripheral configuration
    Author(s):      Pieter Conradie
    Creation Date:  2013-01-16

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
//#include "px_rtc.h"

/* _____DEFINITIONS _________________________________________________________ */
/// The number of system clock ticks per second
#define PX_SYSCLK_CFG_TICKS_PER_SEC 100ul

/// Specify which TMR peripheral must be used
#define PX_SYSCLK_CFG_USE_TMRX 2

/// Specify TMR Prescaler
#define PX_SYSCLK_CFG_TMR_PRESCALER 1024

/// Specify periodic timeout (in sysclk ticks)
#define PX_SYSCLK_CFG_TIMEOUT_PERIOD_TICKS     PX_SYSCLK_CFG_TICKS_PER_SEC 

/// Specify function to call on periodic timeout
//#define PX_SYSCLK_ON_PERIODIC_TIMEOUT()    px_rtc_on_tick()

/// @}
#endif // #ifndef __PX_SYSCLK_CFG_H__
