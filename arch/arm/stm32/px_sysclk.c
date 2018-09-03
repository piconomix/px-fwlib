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
#include "px_sysclk.h"
#include "px_board.h"
#include "px_lib_stm32cube.h"

#if PX_SYSCLK_CFG_STMCUBE_HAL_TMR
#include "stm32l0xx_hal.h"
#endif

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static volatile px_sysclk_ticks_t px_sysclk_tick_counter;

#ifdef PX_SYSCLK_CFG_TIMEOUT_PERIOD_TICKS
static volatile px_sysclk_ticks_t px_sysclk_timeout_counter;
#endif

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
/// SysTick interrupt handler.
void SysTick_Handler(void)
{
    // Increment counter
    px_sysclk_tick_counter++;

#if PX_SYSCLK_CFG_STMCUBE_HAL_TMR
#if (PX_SYSCLK_CFG_TICKS_PER_SEC != 1000)
#error "STMCube HAL expects SysTick interval to be 1 ms"
#endif
    // Increment HAL Tick value for timeout functionality in STMCUBE HAL library
    HAL_IncTick();
#endif

#ifdef PX_SYSCLK_CFG_TIMEOUT_PERIOD_TICKS
    // Decrement timeout counter
    if(--px_sysclk_timeout_counter == 0)
    {
        // Reset timeout counter
        px_sysclk_timeout_counter =  PX_SYSCLK_CFG_TIMEOUT_PERIOD_TICKS;
#endif

#ifdef PX_SYSCLK_CFG_ON_PERIODIC_TIMEOUT
        // Call periodic timeout function
        PX_SYSCLK_CFG_ON_PERIODIC_TIMEOUT();
#endif

#ifdef PX_SYSCLK_CFG_TIMEOUT_PERIOD_TICKS
    }
#endif
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_sysclk_init(void)
{

   uint32_t ticks_per_period;

   // Calculate number of ticks per SysTick period
   ticks_per_period = PX_UDIV_ROUND(PX_BOARD_PER_CLK_HZ, PX_SYSCLK_CFG_TICKS_PER_SEC);

   // Configure SysTick peripheral
   SysTick_Config(ticks_per_period);

   // Enable SysTick interrupt
   NVIC_EnableIRQ(SysTick_IRQn);

#ifdef PX_SYSCLK_CFG_TIMEOUT_PERIOD_TICKS
    // Reset timeout counter
    px_sysclk_timeout_counter =  PX_SYSCLK_CFG_TIMEOUT_PERIOD_TICKS;
#endif
}

px_sysclk_ticks_t px_sysclk_get_tick_count(void)
{
    return px_sysclk_tick_counter;
}
