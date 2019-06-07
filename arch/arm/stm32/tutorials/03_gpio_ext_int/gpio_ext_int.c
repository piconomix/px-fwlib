/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
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

============================================================================= */

/**
 *  @ingroup STM32_TUTORIALS 
 *  @defgroup STM32_TUT03_GPIO_EXT_INT 03_gpio_ext_int
 *  
 *  How to wake up on an external interrupt.
 *  
 *  File(s): 
 *  - arch/arm/stm32/tutorials/03_gpio_ext_int/gpio_ext_int.c
 *  
 *  The button GPIO pin (PC9) is configured as an input with pull-up enabled.
 *  The EXTI peripheral is configured to generate an interrupt when a falling
 *  edge (button press event) is detected on line 9. The `EXTI4_15_IRQHandler()`
 *  function is called that handles external interrupts 4 to 15 and it clears
 *  the interrupt flag for external interrupt 9. The interrupt wakes up the
 *  core and the LED will be flashed before it goes back to sleep.
 */

#include "stm32l0xx.h"
#include "stm32l0xx_ll_bus.h"
#include "stm32l0xx_ll_gpio.h"
#include "stm32l0xx_ll_system.h"
#include "stm32l0xx_ll_exti.h"

void EXTI4_15_IRQHandler(void)
{
    // Pending flag set for line 9?
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_9))
    {
        // Clear pending flag
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_9);
    }    
}

void delay(void)
{
    // Wait ~ 500 ms; default system clock is 2.1 MHz after startup from power reset
    for(uint32_t i = 0x40000; i!=0; i--)
    {
        // Prevent compiler from optimizing and removing empty delay loop
        __asm__ __volatile__("\n\t");
    }
}

int main(void)
{
    // Enable GPIOC, GPIOH & SYSCFG clocks
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOH);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);

    // Configure LED pin (PH0) as a digital output low
    LL_GPIO_SetPinMode(GPIOH, LL_GPIO_PIN_0, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_ResetOutputPin(GPIOH, LL_GPIO_PIN_0);    

    // Configure button pin (PC9) as a digital input with pull-up
    LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_9, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(GPIOC, LL_GPIO_PIN_9, LL_GPIO_PULL_UP);

    // Select PC9 for extended interrupt on EXTI9
    LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTC, LL_SYSCFG_EXTI_LINE9);
    // Enable falling edge external interrupt on line 9
    LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_9);
    LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_9);
    // Enable EXTI4_15 interrupt
    NVIC_EnableIRQ(EXTI4_15_IRQn);

    // Repeat forever
    for(;;)
    {
        // Flash LED
        LL_GPIO_SetOutputPin(GPIOH, LL_GPIO_PIN_0);
        delay();
        LL_GPIO_ResetOutputPin(GPIOH, LL_GPIO_PIN_0);

        // Put core into sleep mode until an interrupt occurs
        __WFI();
    }
}
