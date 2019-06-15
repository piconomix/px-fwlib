/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md

============================================================================= */

/**
 *  @ingroup STM32_TUTORIALS 
 *  @defgroup STM32_TUT02_SYSCLK_INT 02_sysclk_int
 *  
 *  How to use the SysClk peripheral to generate a periodic interrupt.
 *  
 *  File(s): 
 *  - arch/arm/stm32/tutorials/02_sysclk_int/sysclk_int.c
 *  
 *  The LED flashes at 1 Hz (500 ms on; 500 ms off).
 *  
 */

#include "stm32l0xx.h"
#include "stm32l0xx_ll_bus.h"
#include "stm32l0xx_ll_gpio.h"

volatile uint32_t systick_delay;

// SysTick interrupt handler
void SysTick_Handler(void)
{
    // Delay started?
    if(systick_delay != 0)
    {
        // Decrement delay
        systick_delay--;
    }
}

void delay(void)
{
    // Reset delay to 500 ms
    systick_delay = 500;

    // Wait until delay has expired
    while(systick_delay != 0)
    {
        ;
    }
}

int main(void)
{
    // Enable GPIOH clock
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOH);

    // Configure PH0 as a digital output
    LL_GPIO_SetPinMode(GPIOH, LL_GPIO_PIN_0, LL_GPIO_MODE_OUTPUT);

    // Configure SysTick peripheral to generate an interrupt every 1 ms
    SysTick_Config(2100);
    // Enable SysTick interrupt
    NVIC_EnableIRQ(SysTick_IRQn);

    // Repeat forever
    for(;;)
    {
        // Enable LED
        LL_GPIO_SetOutputPin(GPIOH, LL_GPIO_PIN_0);
        delay();

        // Disable LED
        LL_GPIO_ResetOutputPin(GPIOH, LL_GPIO_PIN_0);
        delay();
    }
}
