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
 *  @defgroup STM32_TUT01_GPIO_PART_A 01_gpio_part_a
 *  
 *  How to configure and use General Purpose (digital) I/O pins - part A.
 *  
 *  File(s): 
 *  - arch/arm/stm32/tutorials/01_gpio_part_a/gpio.c
 *  
 *  Reference(s):
 *  - [1] [STM32L072RB Reference Manual](https://www.st.com/resource/en/reference_manual/dm00108281.pdf)
 *  
 *  The LED flashes at 1 Hz (500 ms on; 500 ms off). On the Hero Board, the LED
 *  is wired to Port H pin 0. PH0 must be configured as a digital output pin.
 *  Set PH0 high to enable the LED and set PH0 low to disable the LED.
 *  
 *  All peripherals are memory mapped. The processor core communicates with a
 *  peripheral by writing to or reading from specific memory addresses. To
 *  accomplish this in C, you need to be comfortable with pointer instructions.
 *  A C pointer stores a memory address. The pointer type specifies how the
 *  memory location should be treated.
 *  
 *  @tip_s
 *  This example is simplified as much as possible to demonstrate easy access
 *  to a peripheral's register using C pointers. More advanced pointer
 *  techniques will follow in subsequent examples.
 *  @tip_e
 *  
 *  [1] "9. General-purpose I/Os (GPIO)" on page 234 documents how to use the
 *  GPIO peripheral. Feel free to skip ahead and scan [1] "9.4 GPIO registers"
 *  on page 243 first. After all, this part documents the actual interface to
 *  the peripheral.
 *  
 *  The memory map of peripherals is listed in [1] "2.2.2 Memory map and
 *  register boundary addresses" on page 58. The base address of GPIOH is
 *  0x5000_1c00. The offset of GPIOx_MODER register is 0x00, so the address of
 *  GPIOH_MODER is 0x5000_1c00. The address of GPIOH_BSRR register is
 *  0x5000_1c18 (offset = 0x18).
 *  
 *  The C pointer to the peripheral register is declared as a
 *  `volatile uint32_t *` type. The `uint32_t` part specifies that the memory
 *  address stores an unsigned 32-bit integer. The `volatile` keyword is very
 *  important and specifies to the compiler that the value at that pointer
 *  location can change at any time and the compiler should not use a cached
 *  value. The detrimental effects of forgetting to add the `volatile` keyword
 *  is often seen when compiler optimization is turned on.
 *  
 *  @warn_s
 *  On start up all peripheral clocks are disabled.
 *  @warn_e
 *  
 *  As a general rule you always need to enable a peripheral's clock before you
 *  can use it. Imagine an ARM Cortex microcontroller being a mansion with a
 *  large number of rooms. You would not leave **all** the lights on, but only
 *  switch the lights on in the rooms that you use, otherwise the electricity
 *  bill would be huge. Likewise the clocks to peripherals are disabled by
 *  default to save power. It is also important to know that a higher clock
 *  frequency uses more power and therefor it is desirable to run a peripheral
 *  at the lowest acceptable frequency. The lower peripheral frequency may
 *  incur a communication penalty as the faster processor core has to wait for
 *  the slower peripheral register to return a valid value.
 *  
 *  The first step is thus to enable the clock to GPIOH by setting bit 7 in
 *  the RCC_IOPENR register (address 0x4002102c). See [1] "7.3.12 GPIO clock
 *  enable register (RCC_IOPENR)" on page 202.
 *  
 *  PH0's mode must be changed from analog to digital output by setting
 *  GPIOH_MODER[1:0] to 01. See [1] "9.4.1 GPIO port mode register (GPIOx_MODER)
 *  (x =A..E and H)" on page 243.
 *  
 *  The LED is enabled by setting PH0 high. This is acomplished by writing a 1
 *  to GPIOH_BSRR[0].
 *  
 *  The LED is disabled by setting PH0 low. This is acomplished by writing a 1
 *  to GPIOH_BRR[0].
 *  
 *  @tip_s
 *  STM32CubeIDE enables you to view the address and content of each
 *  peripheral register and also to modify it manually when using the debugging
 *  perspective:
 *  @tip_e
 *  
 *  ![STM32CubeIDE Special Function Register window](sci_debug_sfr.png)
 */

// Include standard type integer definitions, e.g. uint8_t, uint16_t, uint32_t, ...
#include <stdint.h>

// Declare unsigned 32-bit pointer to GPIOH_MODER (GPIO port H mode register) address
volatile uint32_t * gpioh_moder = (volatile uint32_t *)0x50001c00;

// GPIOH_BSRR (GPIO port H bit set/reset register)
volatile uint32_t * gpioh_bsrr = (volatile uint32_t *)0x50001c18;

// GPIOH_BRR (GPIO port H bit reset register)
volatile uint32_t * gpioh_brr  = (volatile uint32_t *)0x50001c28;

// RCC_IOPENR (Reset and Clock Control - GPIO clock enable register)
volatile uint32_t * rcc_iopenr = (volatile uint32_t *)0x4002102c;

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

    // Repeat forever
    for(;;)
    {
        // Set PH0 output to enable LED
        *gpioh_bsrr = 0x00000001;
        delay();

        // Clear PH0 output to disable LED
        *gpioh_brr  = 0x00000001;
        delay();
    }
}
