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

// Include standard type integer definitions, e.g. uint8_t, uint16_t, uint32_t, ...
#include <stdint.h>
// Include standard bool defintions, e.g. bool, true, false, ...
#include <stdbool.h>

// Declare unsigned 32-bit pointer to RCC_IOPENR (Reset and Clock Control - GPIO clock enable register)
volatile uint32_t * rcc_iopenr = (volatile uint32_t *)0x4002102c;

// GPIOH_MODER (GPIO port H mode register) address
volatile uint32_t * gpioh_moder = (volatile uint32_t *)0x50001c00;

// GPIOH_BSRR (GPIO port H bit set/reset register)
volatile uint32_t * gpioh_bsrr = (volatile uint32_t *)0x50001c18;

void delay(void)
{
    // Wait ~ 500 ms; default system clock is 2.1 MHz after startup from reset
    for(uint32_t i = 0x40000; i != 0; i--)
    {
        // Prevent compiler from optimizing and removing empty delay loop
        __asm__ __volatile__("\n\t");
    }
}

int main(void)
{
    uint32_t val;

    // Set RCC_IOPENR register bit 7 to enable clock of Port H
    *rcc_iopenr |= 0x00000080;

    // Read GPIOH_MODER register value
    val          = *gpioh_moder;
    // Set MODE0[1:0] = 01 (General purpose output mode)
    val         &= 0xfffffffd; // Clear bit 1
    val         |= 0x00000001; // Set bit 0
    // Write updated GPIOH_MODER register value
    *gpioh_moder = val;

    // Loop forever
    while(true)
    {
        // Set PH0 output to enable LED
        *gpioh_bsrr = 0x00000001;
        delay();
        // Clear PH0 output to disable LED
        *gpioh_bsrr = 0x00010000;
        delay();
    }
}
