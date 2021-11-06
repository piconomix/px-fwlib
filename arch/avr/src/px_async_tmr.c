/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2014 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          px_async_tmr.h : Asynchronous TMR connected to 32 kHz crystal
    Author(s):      Pieter Conradie
    Creation Date:  2014-06-23

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <avr/io.h>
#include <avr/interrupt.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_async_tmr.h"
#include "px_board.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
// Check if TMR0 must be used
#ifdef AS0
#define ASYNC_TMR                   0
#define ASYNC_TMR_INT_DISABLE()     PX_BIT_SET_LO(TIMSK0, OCIE0)
#define ASYNC_TMR_INT_ENABLE()      PX_BIT_SET_HI(TIMSK0, OCIE0)
#define ASYNC_TMR_INT_IS_ENABLED()  PX_BIT_IS_HI(TIMSK0,  OCIE0)
#define ASYNC_TMR_INT               TIMER0_COMP_vect
#endif

// Check if TMR2 must be used
#ifdef AS2
#define ASYNC_TMR                   2
#define ASYNC_TMR_INT_DISABLE()     PX_BIT_SET_LO(TIMSK2, OCIE2A)
#define ASYNC_TMR_INT_ENABLE()      PX_BIT_SET_HI(TIMSK2, OCIE2A)
#define ASYNC_TMR_INT_IS_ENABLED()  PX_BIT_IS_HI(TIMSK2,  OCIE2A)
#define ASYNC_TMR_INT               TIMER2_COMPA_vect
#endif

#ifndef ASYNC_TMR
#error "Unsupported AVR"
#endif

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
/// Interrupt handler on TMR compare match (TCNT is cleared automatically)
ISR(ASYNC_TMR_INT)
{
    ASYNC_TMR_CFG_ON_TICK();
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_async_tmr_init(void)
{
#if (ASYNC_TMR == 0)

    // Select asynchronous timer operation to use external 32.768 kHz crystal
    PX_BIT_SET_HI(ASSR, AS0);
    // Start timer with clock prescaler CLK/1024 and CTC Mode ("Clear Timer on Compare")
    // Resolution is 32.25 ms
    TCCR0 = (0 << WGM00) | (1 << WGM01) | (1 << CS02) | (1 << CS01) | (1 << CS00);
    // Reset time
    TCNT0 = 0;
    // Calculate and set period
    OCR0 = (uint16_t)(((F_RTC / 1024) * PX_RTC_PERIOD_MS) / 1000) - 1;
    // Wait for "update busy" flags to clear
    while(ASSR & ((1 << TCN0UB) | (1 << OCR0UB) | (1 << TCR0UB))) {;}

#elif (ASYNC_TMR == 2)

    // Select asynchronous timer operation to use external 32768 Hz crystal
    PX_BIT_SET_HI(ASSR, AS2);
    // Start timer with clock prescaler CLK/1024 and CTC Mode ("Clear Timer on Compare")
    // Resolution is 32.25 ms
    TCCR2A = (0 << WGM20) | (1 << WGM21);
    TCCR2B = (0 << WGM22) | (1 << CS22) | (1 << CS21) | (1 << CS20);
    // Reset time
    TCNT2 = 0;
    // Calculate and set period (F_CPU is defined in 'px_board.h')
    OCR2A = (uint16_t)(((F_RTC / 1024) * ASYNC_TMR_CFG_PERIOD_MS) / 1000) - 1;
    // Wait for "update busy" flags to clear
    while(ASSR & ((1 << TCN2UB) | (1 << OCR2AUB) | (1 << TCR2AUB))) {;}

#else
#error "Unsupported AVR"
#endif

    // Enable interrupt
    ASYNC_TMR_INT_ENABLE();
}

void px_async_tmr_int_enable(void)
{
    ASYNC_TMR_INT_ENABLE();
}

bool px_async_tmr_int_disable(void)
{
    bool int_is_enabled = ASYNC_TMR_INT_IS_ENABLED();

    ASYNC_TMR_INT_DISABLE();

    return int_is_enabled;
}
