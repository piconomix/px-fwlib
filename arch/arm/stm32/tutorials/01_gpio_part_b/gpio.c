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
 *  @defgroup STM32_TUT01_GPIO_PART_B 01_gpio_part_b
 *  
 *  How to configure and use General Purpose (digital) I/O pins - part B.
 *  
 *  File(s): 
 *  - arch/arm/stm32/tutorials/01_gpio_part_b/gpio.c
 *  
 *  Reference(s):
 *  - [1] [STM32L072RB Reference Manual](https://www.st.com/resource/en/reference_manual/dm00108281.pdf)
 *  
 *  The LED flashes at 1 Hz (500 ms on; 500 ms off). On the Hero Board, the LED
 *  is wired to Port H pin 0. PH0 must be configured as a digital output pin.
 *  Set PH0 high to enable the LED and set PH0 low to disable the LED.
 *  
 *  The relative register map of each GPIO peripheral stays the same, it is
 *  just the base address that changes:
 *  - GPIOA : 0x5000_0000
 *  - GPIOB : 0x5000_0400
 *  - GPIOC : 0x5000_0800
 *  - GPIOD : 0x5000_0C00
 *  - GPIOE : 0x5000_1000
 *  - GPIOH : 0x5000_1C00
 *  
 *  An advanced C pointer technique is used to declare the relative register map
 *  once and reuse it for each GPIO peripheral by changing the base address.
 *  
 *  A structure type `gpio_peripheral_t` is created that defines the GPIO
 *  peripheral register offset map and a C pointer `gpioh` is created to point
 *  to the GPIO peripheral base address. Each GPIO register is accessed relative
 *  to this base address using a structure pointer operator, e.g. `gpioh->bsrr`
 *  will access the register at address 0x5000_1C00.
 *  
 *  @warn_s
 *  Note the use of the `volatile` keyword for each member of the structure.
 *  @warn_e
 *  
 *  The structure definition is thus a template that defines how a certain
 *  memory area should be treated.
 *  
 *  In this tutorial the bit left shift and negation operator is also
 *  demonstrated. Let's break this down into individual steps:
 *  
 *      @code{.c}
 *      // Compound statement to clear bit 1 and bit 0 
 *      val &= ~(3 << 0);
 *  
 *      // Step 1: Expand &= operator
 *      val  = val & (~(3 << 0));
 *      // Step 2: Perform bit shift operator (zero shift means no change)
 *      val  = val & (~(0x00000003));
 *      // Step 3: Perform negation operator (flip bits)
 *      val  = val & 0xfffffffc;
 *      @endcode
 *  
 *  It is important to remember that the ARM Cortex M0+ requires 32-bit reads
 *  and writes to be aligned on 32-bit addresses. For 16-bit reads and writes
 *  the address must be aligned on a 16-bit boundary. The default behaviour of
 *  the compiler is to create a structure with members correctly aligned by
 *  inserting padding, but it is sometimes better to explicitely declare the
 *  padding, e.g.
 *  
 *      @code{.c}
 *      typedef struct
 *      {
 *          uint8_t  field1;        // Offset 0
 *          uint8_t  reserved_1[3]; // Padding of 3 bytes
 *          uint32_t field2;        // Offset 4
 *          uint16_t field3;        // Offset 8
 *          uint16_t reserved_2;    // Padding of 2 bytes
 *          uint32_t field4;        // Offset 12
 *      } my_struct_t;
 *      @endcode
 */

// Include standard type integer definitions, e.g. uint8_t, uint16_t, uint32_t, ...
#include <stdint.h>

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
    *rcc_iopenr |= (1 << 7);

    // Read GPIOH_MODER register value
    val          = gpioh->moder;
    // Clear MODE0[1:0]
    val         &= ~(3 << 0);
    // Set MODE0[1:0] = 01 (General purpose output mode)
    val         |=  (1 << 0);
    // Write updated GPIOH_MODER register value
    gpioh->moder = val;

    // Repeat forever
    for(;;)
    {
        // Set PH0 output to enable LED
        gpioh->bsrr = (1 << 0);
        delay();

        // Clear PH0 output to disable LED
        gpioh->brr  = (1 << 0);
        delay();
    }
}
