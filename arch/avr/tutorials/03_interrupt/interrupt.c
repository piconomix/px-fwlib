/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2006-2014 Pieter Conradie <https://piconomix.com>
 
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.

============================================================================= */

/**
 *  @ingroup AVR_TUTORIALS 
 *  @defgroup AVR_TUT03_INTERRUPT 03_interrupt
 *  
 *  How to implement a simple interrupt handler. 
 *  
 *  File(s):
 *  - arch/avr/tutorials/03_interrupt/interrupt.c 
 *  
 *  A timer is configured to generate an interrupt every 250 ms. The LED output 
 *  pin is toggled every 250 ms in the timer interrupt.
 *  
 *  A macro `sei()` is called to enable interrupts globally. It sets bit 7 (I)
 *  of the SREG register. `cli()` can be called to disable all interrupts.
 *  
 *  @tip_s
 *  Refer to Table 12-6 "Reset and Interrupt Vectors in ATmega328 and
 *  ATmega328P" on page 65 of the ATmega328p datasheet to view the full list of
 *  interrupt vectors. Assuming Atmel Studio 7 is installed in it's default
 *  location, the interrupt vector names are defined in:
 *  @tip_e
 *  
 *      C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\avr\include\avr\iom328p.h
 */


#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// LED GPIO pin macros
#define LED_INIT()   DDRB  |= (1<<0)
#define LED_TOGGLE() PORTB ^= (1<<0)

// Define CPU frequency in Hz
#define F_CPU           7372800ul

// Define delay
#define PERIOD_MS       250

// Interrupt handler on compare match (TCNT1 is cleared automatically)
ISR(TIMER1_COMPA_vect)
{
    // Toggle LED
    LED_TOGGLE();
}

void tmr_init(void)
{
    /* 
     *  Start 16-bit TMR1 with clock prescaler CLK/1024. For F_CPU = 7372800 Hz,
     *  resolution is 139 us, maximum time is 9.1 s
     *
     *  Select Clear Timer on Compare match (CTC) mode of operation. This means
     *  that when TCNT1 reaches the OCR1A value, OCF1A flag will be set and TCNT1
     *  will be reset to 0.
     */ 
    TCCR1A = (0<<WGM11) | (0<<WGM10);
    TCCR1B = (0<<WGM13) | (1<<WGM12) | (1<<CS12) | (0<<CS11) | (1<<CS10);

    // Reset counter
    TCNT1 = 0;

    // Calculate and set delay
    OCR1A  = (uint16_t)(((F_CPU / 1024) * PERIOD_MS) / 1000) - 1;

    // Enable interrupt on compare match
    TIMSK1 |= (1<<OCIE1A);
}

int main(void)
{
    // Initialise LED GPIO pin
    LED_INIT();

    // Initialise timer
    tmr_init();

    // Enable interrupts
    sei();

    // Repeat forever
    for(;;)
    {
        ;
    }
}
