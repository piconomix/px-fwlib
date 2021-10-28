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

#include "stm32l0xx_ll_bus.h"
#include "stm32l0xx_ll_gpio.h"

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
    // Set RCC_IOPENR register bit 7 to enable clock of Port H
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOH);
    // Configure PH0 as a digital output
    LL_GPIO_SetPinMode(GPIOH, LL_GPIO_PIN_0, LL_GPIO_MODE_OUTPUT);

    // Repeat forever
    for(;;)
    {
        // Set PH0 output to enable LED
        LL_GPIO_SetOutputPin(GPIOH, LL_GPIO_PIN_0);
        delay();
        // Clear PH0 output to disable LED
        LL_GPIO_ResetOutputPin(GPIOH, LL_GPIO_PIN_0);
        delay();
    }
}
