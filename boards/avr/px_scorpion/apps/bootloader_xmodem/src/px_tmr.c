/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2012 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
 
    Title:          Polled timeout functionality using a TMR peripheral
    Author(s):      Pieter Conradie
    Creation Date:  2012-09-08

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <avr/io.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_tmr.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_tmr_init(void)
{
    // Initialise 16-bit TMR1 @ CK/1024 in CTC (Clear Timer Compare) mode
    TCCR1A = (0 << WGM11) | (0 << WGM10);
    TCCR1B = (0 << WGM13) | (1 << WGM12) | (1 << CS12) | (0 << CS11) | (1 << CS10);
}

void px_tmr_start(uint16_t ticks)
{
    OCR1A = ticks;
    TCNT1 = 0;

    // Clear output compare flag
    PX_BIT_SET_HI(TIFR1, OCF1A);
}

bool px_tmr_has_expired(void)
{
    // See if output compare flag is set
    if(PX_BIT_IS_HI(TIFR1, OCF1A))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void px_tmr_wait(uint16_t ticks)
{
    OCR1A = ticks;
    TCNT1 = 0;

    // Clear output compare flag
    PX_BIT_SET_HI(TIFR1, OCF1A);
    // Wait until otput compare flag is set
    PX_WAIT_UNTIL_BIT_IS_HI(TIFR1, OCF1A);   
}
