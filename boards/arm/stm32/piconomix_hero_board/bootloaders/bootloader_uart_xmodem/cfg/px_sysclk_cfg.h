#ifndef __PX_SYSCLK_CFG_H__
#define __PX_SYSCLK_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
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
    
    Title:          px_sysclk.h : System Clock using the SysTick peripheral
    Author(s):      Pieter Conradie
    Creation Date:  2018-03-06

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
//#include "px_rtc.h"

/* _____DEFINITIONS _________________________________________________________ */
/// The number of system clock ticks per second
#define PX_SYSCLK_CFG_TICKS_PER_SEC 1000ul

/// Enable (1) or disable (0) support for STM32Cube HAL 1 ms SysTick timer functionality
#define PX_SYSCLK_CFG_STMCUBE_HAL_TMR 0

/// Specify periodic timeout (in sysclk ticks)
//#define PX_SYSCLK_CFG_TIMEOUT_PERIOD_TICKS     PX_SYSCLK_CFG_TICKS_PER_SEC

/// Specify function to call on periodic timeout
//#define PX_SYSCLK_ON_PERIODIC_TIMEOUT()    px_rtc_on_tick()

/// @}
#endif // #ifndef __PX_SYSCLK_CFG_H__
