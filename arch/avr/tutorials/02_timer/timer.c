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
 *  @defgroup AVR_TUT02_TIMER 02_timer
 *  
 *  How to configure and use a TMR peripheral as a blocking delay.
 *  
 *  File(s):
 *  - arch/avr/tutorials/02_timer/timer.c
 *  
 *  The LED flashes: on for 100 ms, off for 900 ms, on for 100 ms, ...
 *  
 *  See also:
 *  - @ref AVR_SYSCLK 
 *  - @ref AVR_EX_FLASHING_LED 
 */

#include <stdint.h>
#include <avr/io.h>

// LED GPIO pin macros
#define LED_INIT()  DDRB  |=  (1<<0)
#define LED_ON()    PORTB |=  (1<<0)
#define LED_OFF()   PORTB &= ~(1<<0)

// Define CPU frequency in Hz
#define F_CPU 7372800ul

void tmr_init(void)
{
    /* 
     *  Start 16-bit TMR1 with clock prescaler CLK/1024. For F_CPU = 7372800 Hz,
     *  the resolution is 139 us and the maximum time is 9.1 s
     *
     *  Select Clear Timer on Compare match (CTC) mode of operation. This means
     *  that when TCNT1 reaches the OCR1A value, OCF1A flag will be set and
     *  TCNT1 will be reset to 0.
     */ 
    TCCR1A = (0<<WGM11) | (0<<WGM10);
    TCCR1B = (0<<WGM13) | (1<<WGM12) | (1<<CS12) | (0<<CS11) | (1<<CS10);
}

void tmr_delay(uint16_t delay_ms)
{
    // Calculate and set delay
    OCR1A = ((F_CPU / 1024) * delay_ms) / 1000;

    // Reset counter
    TCNT1 = 0;

    // Clear OCF1A flag by writing a logical 1; other flags are unchanged
    // This is more efficient than using "TIFR1 |= (1<<OCF1A);"
    TIFR1 = (1<<OCF1A);

    // Wait until OCF1A flag is set
    while((TIFR1 & (1<<OCF1A)) == 0)
    {
        ;
    }
}

int main(void)
{
    // Initialise LED GPIO pin
    LED_INIT();

    // Initialise timer
    tmr_init();

    // Repeat forever
    for(;;)
    {
        // Enable LED
        LED_ON();

        // Wait 100 ms
        tmr_delay(100);

        // Disable LED
        LED_OFF();

        // Wait 900 ms
        tmr_delay(900);
    }
}
