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

// Include standard bool defintions, e.g. bool, true, false, ...
#include <stdbool.h>

#include "stm32l0xx.h"
#include "stm32l0xx_ll_bus.h"
#include "stm32l0xx_ll_gpio.h"

// Delay counter decremented once every 1 ms by the SysTick interrupt
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
    while(systick_delay != 0) {;}
}

int main(void)
{
    // Enable GPIOH clock
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOH);
    // Configure PH0 as a digital output
    LL_GPIO_SetPinMode(GPIOH, LL_GPIO_PIN_0, LL_GPIO_MODE_OUTPUT);

    // Configure SysTick peripheral to generate an interrupt every 1 ms:
    // Default system clock is 2.1 MHz after startup from reset
    // Reload value = 1 ms / ( 1 / 2.1 MHz) - 1 = 2099
    SysTick_Config(2099);
    // Enable SysTick interrupt
    NVIC_EnableIRQ(SysTick_IRQn);

    // Loop forever
    while(true)
    {
        // Enable LED
        LL_GPIO_SetOutputPin(GPIOH, LL_GPIO_PIN_0);
        delay();
        // Disable LED
        LL_GPIO_ResetOutputPin(GPIOH, LL_GPIO_PIN_0);
        delay();
    }
}
