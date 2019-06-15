/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2006 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          px_sysclk.h : System Clock using a TMRx peripheral 
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <avr/io.h>
#include <avr/interrupt.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_sysclk.h"
#include "px_board.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
#if (PX_SYSCLK_CFG_USE_TMRX == 0)

#define TMR_VECT                TIMER0_COMPA_vect
#define TMR_INT_ENABLE()        PX_BIT_SET_HI(TIMSK0, OCIE0A)
#define TMR_INT_DISABLE()       PX_BIT_SET_LO(TIMSK0, OCIE0A)
#define TMR_INT_IS_ENABLED()    PX_BIT_IS_HI(TIMSK0, OCIE0A)

#if   (PX_SYSCLK_CFG_TMR_PRESCALER == 1)
#define PX_SYSCLK_CS   1
#elif (PX_SYSCLK_CFG_TMR_PRESCALER == 8)
#define PX_SYSCLK_CS   2
#elif (PX_SYSCLK_CFG_TMR_PRESCALER == 64)
#define PX_SYSCLK_CS   3
#elif (PX_SYSCLK_CFG_TMR_PRESCALER == 256)
#define PX_SYSCLK_CS   4
#elif (PX_SYSCLK_CFG_TMR_PRESCALER == 1024)
#define PX_SYSCLK_CS   5
#else
#error "PX_SYSCLK_TMR_PRESCALER value is invalid"
#endif

#elif (PX_SYSCLK_CFG_USE_TMRX == 1)

#define TMR_VECT                TIMER1_COMPA_vect
#define TMR_INT_ENABLE()        PX_BIT_SET_HI(TIMSK1, OCIE1A)
#define TMR_INT_DISABLE()       PX_BIT_SET_LO(TIMSK1, OCIE1A)
#define TMR_INT_IS_ENABLED()    PX_BIT_IS_HI(TIMSK1, OCIE1A)
#if   (PX_SYSCLK_CFG_TMR_PRESCALER == 1)
#define PX_SYSCLK_CS   1
#elif (PX_SYSCLK_CFG_TMR_PRESCALER == 8)
#define PX_SYSCLK_CS   2
#elif (PX_SYSCLK_CFG_TMR_PRESCALER == 64)
#define PX_SYSCLK_CS   3
#elif (PX_SYSCLK_CFG_TMR_PRESCALER == 256)
#define PX_SYSCLK_CS   4
#elif (PX_SYSCLK_CFG_TMR_PRESCALER == 1024)
#define PX_SYSCLK_CS   5
#else
#error "PX_SYSCLK_TMR_PRESCALER value is invalid"
#endif

#elif (PX_SYSCLK_CFG_USE_TMRX == 2)

#define TMR_VECT                TIMER2_COMPA_vect
#define TMR_INT_ENABLE()        PX_BIT_SET_HI(TIMSK2, OCIE2A)
#define TMR_INT_DISABLE()       PX_BIT_SET_LO(TIMSK2, OCIE2A)
#define TMR_INT_IS_ENABLED()    PX_BIT_IS_HI(TIMSK2, OCIE2A)
#if   (PX_SYSCLK_CFG_TMR_PRESCALER == 1)
#define PX_SYSCLK_CS   1
#elif (PX_SYSCLK_CFG_TMR_PRESCALER == 8)
#define PX_SYSCLK_CS   2
#elif (PX_SYSCLK_CFG_TMR_PRESCALER == 32)
#define PX_SYSCLK_CS   3
#elif (PX_SYSCLK_CFG_TMR_PRESCALER == 64)
#define PX_SYSCLK_CS   4
#elif (PX_SYSCLK_CFG_TMR_PRESCALER == 128)
#define PX_SYSCLK_CS   5
#elif (PX_SYSCLK_CFG_TMR_PRESCALER == 256)
#define PX_SYSCLK_CS   6
#elif (PX_SYSCLK_CFG_TMR_PRESCALER == 1024)
#define PX_SYSCLK_CS   7
#else
#error "PX_SYSCLK_TMR_PRESCALER value is invalid"
#endif

#else
#error "Specified TMR not supported!"
#endif


/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static volatile px_sysclk_ticks_t px_sysclk_tick_counter;

#ifdef PX_SYSCLK_CFG_TIMEOUT_PERIOD_TICKS
static volatile px_sysclk_ticks_t px_sysclk_timeout_counter;
#endif

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
/// Timer interrupt handler.
ISR(TMR_VECT)
{
    // Increment counter
    px_sysclk_tick_counter++;

#ifdef PX_SYSCLK_CFG_TIMEOUT_PERIOD_TICKS
    // Decrement timeout counter
    if(--px_sysclk_timeout_counter == 0)
    {
        // Reset timeout counter
        px_sysclk_timeout_counter =  PX_SYSCLK_CFG_TIMEOUT_PERIOD_TICKS;
#endif

#ifdef PX_SYSCLK_CFG_ON_PERIODIC_TIMEOUT
        // Call periodic timeout function
        PX_SYSCLK_CFG_ON_PERIODIC_TIMEOUT();
#endif

#ifdef PX_SYSCLK_CFG_TIMEOUT_PERIOD_TICKS
    }
#endif
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_sysclk_init(void)
{
#if (PX_SYSCLK_CFG_USE_TMRX == 0)

    // Initialise timer interrupt @ CLK/PX_SYSCLK_TMR_PRESCALER in CTC mode ("Clear Timer on Compare")
    TCCR0A =  (1<<WGM01) | (0<<WGM00);
    TCCR0B =  (0<<WGM02) | (PX_SYSCLK_CS<<CS00);
    // Reset TMR counter
    TCNT0  = 0;
    // Calculate and set maximum TMR counter value. When TCNT reaches this 
    // value, an interrupt is generated and TCNT is reset to 0.
    OCR0A  = PX_UDIV_ROUND(F_CPU, PX_SYSCLK_CFG_TMR_PRESCALER*PX_SYSCLK_CFG_TICKS_PER_SEC) - 1;

#elif (PX_SYSCLK_CFG_USE_TMRX == 1)

    // Initialise timer interrupt @ CLK/PX_SYSCLK_TMR_PRESCALER in CTC mode ("Clear Timer on Compare")
    TCCR1A =  (0<<WGM11) | (0<<WGM10);
    TCCR1B =  (0<<WGM13) | (1<<WGM12) | (PX_SYSCLK_CS<<CS10);
    // Reset TMR counter
    TCNT1  = 0;
    // Calculate and set maximum TMR counter value. When TCNT reaches this 
    // value, an interrupt is generated and TCNT is reset to 0.
    OCR1A  = PX_UDIV_ROUND(F_CPU, PX_SYSCLK_CFG_TMR_PRESCALER*PX_SYSCLK_CFG_TICKS_PER_SEC) - 1;

#elif (PX_SYSCLK_CFG_USE_TMRX == 2)

    // Initialise timer interrupt @ CLK/PX_SYSCLK_TMR_PRESCALER in CTC mode ("Clear Timer on Compare")
    TCCR2A =  (1<<WGM21) | (0<<WGM20);
    TCCR2B =  (0<<WGM22) | (PX_SYSCLK_CS<<CS20);
    // Reset TMR counter
    TCNT2  = 0;
    // Calculate and set maximum TMR counter value. When TCNT reaches this 
    // value, an interrupt is generated and TCNT is reset to 0.
    OCR2A  = PX_UDIV_ROUND(F_CPU, PX_SYSCLK_CFG_TMR_PRESCALER*PX_SYSCLK_CFG_TICKS_PER_SEC) - 1;

#else
    #error "Specified TMR not supported!"
#endif

#ifdef PX_SYSCLK_CFG_TIMEOUT_PERIOD_TICKS
    // Reset timeout counter
    px_sysclk_timeout_counter =  PX_SYSCLK_CFG_TIMEOUT_PERIOD_TICKS;
#endif

    // Enable timer interrupt
    TMR_INT_ENABLE();
}

bool px_sysclk_disable(void)
{
    bool enabled = TMR_INT_IS_ENABLED();

    TMR_INT_DISABLE();

    return enabled;
}

void px_sysclk_enable(void)
{
    TMR_INT_ENABLE();
}

px_sysclk_ticks_t px_sysclk_get_tick_count(void)
{
    px_sysclk_ticks_t counter;

    // Disable timer interrupt to perform atomic access
    TMR_INT_DISABLE();

    // Fetch current time
    counter = px_sysclk_tick_counter;

    // Enable timer interrupt
    TMR_INT_ENABLE();

    return counter;
}
