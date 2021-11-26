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

// RCC_IOPENR (Reset and Clock Control - GPIO clock enable register)
volatile uint32_t * rcc_iopenr = (volatile uint32_t *)0x4002102c;

// Define GPIO peripheral register offset map
typedef struct
{
  volatile uint32_t moder;      // GPIO port mode register               (offset: 0x00)
  volatile uint32_t otyper;     // GPIO port output type register        (offset: 0x04)
  volatile uint32_t ospeedr;    // GPIO port output speed register       (offset: 0x08)
  volatile uint32_t pupdr;      // GPIO port pull-up/pull-down register  (offset: 0x0C)
  volatile uint32_t idr;        // GPIO port input data register         (offset: 0x10)
  volatile uint32_t odr;        // GPIO port output data register        (offset: 0x14)
  volatile uint32_t bsrr;       // GPIO port bit set/reset register      (offset: 0x18)
  volatile uint32_t lckr;       // GPIO port configuration lock register (offset: 0x1C)
  volatile uint32_t afrl;       // GPIO alternate function low register  (offset: 0x20)
  volatile uint32_t afrh;       // GPIO alternate function high register (offset: 0x20)
  volatile uint32_t brr;        // GPIO port bit reset register          (offset: 0x28)
} gpio_peripheral_t;

// Declare structure pointer to GPIOH peripheral base address
gpio_peripheral_t * gpioh = (gpio_peripheral_t *)0x50001c00;

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
    *rcc_iopenr |= (1 << 7);

    // Read GPIOH_MODER register value
    val  = gpioh->moder;
    // Clear MODE0[1:0]
    val &= ~(3 << 0);
    // Set MODE0[1:0] = 01 (General purpose output mode)
    val |=  (1 << 0);
    // Write updated GPIOH_MODER register value
    gpioh->moder = val;

    // Loop forever
    while(true)
    {
        // Set PH0 output to enable LED
        gpioh->bsrr = (1 << 0);
        delay();
        // Clear PH0 output to disable LED
        gpioh->bsrr = (1 << 16);
        delay();
    }
}
