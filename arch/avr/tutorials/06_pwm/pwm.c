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
 *  @defgroup AVR_TUT06_PWM 06_pwm
 *  
 *  How to configure and use a TMR peripheral to generate a PWM output.
 *  
 *  File(s): 
 *  - arch/avr/tutorials/06_pwm/pwm.c
 *  
 *  The piezo buzzer is driven with a 50% duty cycle at about 1000 Hz.
 *  
 *  @tip_s
 *  An undesirable side-effect effect of the flexibility and power of the
 *  TMR peripherals is the sheer size of the datasheet documentation. Most
 *  times it helps to skip the lengthy description and focus on the actual
 *  peripheral register description, e.g. do not start at p.94, but jump
 *  directly to "15.9 Register Description" on p.105 of the ATmega328p
 *  datasheet.
 *  @tip_e
 */

#include <stdint.h>
#include <avr/io.h>

// Define CPU frequency in Hz
#define F_CPU 7372800ul

int main(void)
{
    // Initialise PD6 to output low to buzzer
    PORTD &= ~(1<<6);
    DDRD  |=  (1<<6);

    /* 
     *  Start Timer 0 with clock prescaler CLK/64 and CTC mode.
     *  Output on PD6 (OC0A).
     * 
     *  For F_CPU = 7372800 Hz:
     *   - resolution is 8.7 us
     *   - frequency range is 57600 Hz to 225 Hz
     */
    TCCR0A =  (0<<COM0A1) | (1<<COM0A0) | (1<<WGM01) | (0<<WGM00);
    TCCR0B =  (0<<WGM02)  | (1<<CS02)   | (0<<CS01)  | (0<<CS00);

    // Reset counter
    TCNT0 = 0;

    // Set frequency to about 1000 Hz
    OCR0A  = ((F_CPU / (2 * 64)) / 1000) - 1;

    // Repeat indefinitely
    for(;;)
    {
        ;
    }
}
