/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2006-2014 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

============================================================================= */

/**
 *  @ingroup AVR_TUTORIALS 
 *  @defgroup AVR_TUT01_GPIO 01_gpio
 *  
 *  How to configure and use general purpose (digital) I/O pins.
 *  
 *  File(s): 
 *  - arch/avr/tutorials/01_gpio/gpio.c
 *  
 *  The LED shines while the push button is being pressed.
 *  
 *  Compact C techniques are used to manipulate individual bits of a Peripheral
 *  Control Register. It is good programming practice to encapsulate these
 *  often used and error prone pieces of code in macros, as has been
 *  demonstrated in @ref AVR_EX_FLASHING_LED.
 *  
 *  The following piece of code: 
 *  
 *      @code{.c}
 *      PORTB = PORTB | 0x01; // Set bit 0 of PORTB register to enable LED
 *      PORTB = PORTB & 0xfe; // Clear bit 0 of PORTB to disable LED
 *      @endcode
 *  
 *  can be compacted to:
 *  
 *      @code {.c}
 *      PORTB |=  (1<<0);     // Set bit 0 of PORTB register to enable LED
 *      PORTB &= ~(1<<0);     // Clear bit 0 of PORTB to disable LED
 *      @endcode
 *  
 *  and better yet, be replaced with macros:
 *  
 *      @code{.c}
 *      // General purpose bit manipulation macros
 *      #define BIT_SET_HI(var, bit)    var |=  (1<<bit)
 *      #define BIT_SET_LO(var, bit)    var &= ~(1<<bit)
 *  
 *      // Define LED GPIO pin
 *      #define GPIO_LED_REG_PORT       PORTB
 *      #define GPIO_LED_BIT            0
 *  
 *      // Define macros to turn LED on and off
 *      #define LED_ON()                BIT_SET_HI(GPIO_LED_REG_PORT, GPIO_LED_BIT)
 *      #define LED_OFF()               BIT_SET_LO(GPIO_LED_REG_PORT, GPIO_LED_BIT)
 *  
 *      void example_fn(void)
 *      {
 *          LED_ON();  // Enable LED
 *          LED_OFF(); // Disable LED
 *      }
 *      @endcode 
 *  
 *  or best yet, use @ref AVR_GPIO. Here is an example:
 *  
 *  @include "avr/test/px_gpio_test.c"
 *  
 *  @tip_s
 *  Assuming Atmel Studio 7 is installed in it's default location, The
 *  peripheral registers and bit names are defined in:
 *  @tip_e
 *  
 *      C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\avr\include\avr\iom328p.h
 */

#include <avr/io.h>

// LED is an output on PORTB, pin 0
// PB is an input (with pull-up enabled) on PORTD, pin 7

int main(void)
{
    // Configure LED as output pin (set bit 0 of DDRB register)
    DDRB |= (1<<0);

    // Configure PB as input pin (clear bit 7 of DDRD register)
    DDRD &= ~(1<<7);

    // Enable pull-up on PB (set bit 7 of PORTD register)
    PORTD |= (1<<7);

    // Repeat forever
    for(;;)
    {
        // PB is pressed? (bit 7 of PIND register cleared?)
        if((PIND & (1<<7)) == 0)
        {
            // Enable LED (set bit 0 of PORTB register)
            PORTB |= (1<<0);
        }
        else
        {
            // Disable LED (clear bit 0 of PORTB register)
            PORTB &= ~(1<<0);
        }
    }
}
