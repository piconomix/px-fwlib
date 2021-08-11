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
    uint32_t val;

    // Set RCC_IOPENR register bit 7 to enable clock of Port H
    RCC->IOPENR |= RCC_IOPENR_IOPHEN;

    // Read GPIOH_MODER register value
    val          = GPIOH->MODER;
    // Set MODE0[1:0] = 01 (General purpose output mode)
    val         &= ~GPIO_MODER_MODE0_Msk;        // Clear bit 1 and bit 0
    val         |=  (1 << GPIO_MODER_MODE0_Pos); // Set bit 0
    // Write updated GPIOH_MODER register value
    GPIOH->MODER = val;

    // Repeat forever
    for(;;)
    {
        // Set PH0 output to enable LED
        GPIOH->BSRR = GPIO_BSRR_BS_0;
        delay();

        // Clear PH0 output to disable LED
        GPIOH->BSRR = GPIO_BSRR_BR_0;
        delay();
    }
}
