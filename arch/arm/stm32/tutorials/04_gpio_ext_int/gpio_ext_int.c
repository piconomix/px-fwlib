/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

============================================================================= */

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
    // Wait ~ 500 ms; default system clock is 2.1 MHz after startup from reset
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
        // (WFI = Wait For Interrupt)
        __WFI();
    }
}
