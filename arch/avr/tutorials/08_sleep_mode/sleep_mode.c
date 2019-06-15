/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2006-2014 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md

============================================================================= */

/**
 *  @ingroup AVR_TUTORIALS 
 *  @defgroup AVR_TUT08_SLEEP_MODE 08_sleep_mode
 *  
 *  How to use one of the AVR's low power sleep modes.
 *  
 *  File(s):
 *  - arch/avr/tutorials/08_sleep_mode/sleep_mode.c 
 *  
 *  The AVR wakes up every 250 ms, toggles the LED output and returns to sleep.
 *  
 *  There are various sleep modes, each with increasing levels of power saving. 
 *  Refer to 10.1 "Sleep Modes", p.38 of the ATmega328p datasheet to view the
 *  list.
 *  
 *  @tip_s
 *  The recommended way to implement sleep mode is to use
 *  [<avr/sleep.h>](https://www.nongnu.org/avr-libc/user-manual/group__avr__sleep.html).
 *  Example:
 *  @tip_e    
 *  
 *      @code{.c}
 *      #include <avr/sleep.h> 
 *  
 *      set_sleep_mode(SLEEP_MODE_IDLE); 
 *      sleep_mode(); 
 *      @endcode 
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
    // Do nothing; interrupt is only used to wake up microcontroller
}

void tmr_init(void)
{
    /* 
     *  Start 16-bit TMR1 with clock prescaler CLK/1024. For F_CPU = 7372800 Hz,
     *  resolution is 139 us, maximum time is 9.1 s
     *
     *  Select Clear Timer on Compare match (CTC) mode of operation. This means
     *  that when TCNT1 reaches the OCR1A value, OCF1A flag will be set and
     *  TCNT1 will be reset to 0.
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

    // Select IDLE sleep Mode by clearing bits SM2...SM0
    SMCR &= ~((1<<SM2) | (1<<SM1) | (1<<SM0));

    // Repeat forever
    for(;;)
    {
        // Enable sleep mode
        SMCR |= (1<<SE);
        
        // Call "sleep" assembler instruction
        __asm__ __volatile__ ("sleep\n\t"::);

        // An interrupt will wake up the microcontroller and it's handler 
        // executed, before execution is returned here to the main program flow.

        // Toggle LED
        LED_TOGGLE();
        
        // Disable sleep mode
        SMCR &= ~(1<<SE);
    }
}
