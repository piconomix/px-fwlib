/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2012-2014 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          flashing_led : Microchip AVR Flashing LED example
    Author(s):      Pieter Conradie
    Creation Date:  2007-04-27
 
============================================================================= */

/** 
 *  @ingroup AVR_EXAMPLES
 *  @defgroup AVR_EX_FLASHING_LED flashing_led : Microchip AVR Flashing LED example
 *   
 *  This example toggles the LED once a second. 
 *   
 *  File(s): 
 *  - arch/avr/examples/flashing_led/main.c 
 *   
 *  The following modules are exercised: 
 *  - @ref AVR_SYSCLK 
 *  - @ref PX_SYSTMR 
 *  - @ref BOARDS_AVR_PICONOMIX_SCORPION
 *  
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_systmr.h"
#include "px_board.h"
#include "px_sysclk.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____PUBLIC FUNCTIONS_____________________________________________________ */
int main(void)
{
    px_systmr_t systmr;

    // Initialise modules
    px_board_init();
    px_sysclk_init();
    // Enable LED
    PX_LED_ON();
    // Enable interrupts
    px_interrupts_enable();
    // Initialise timer to expire once a second
    px_systmr_start(&systmr, PX_SYSTMR_TICKS_PER_SEC);

    // Repeat forever
    for(;;)
    {
        // Wait until timer has expired
        while(!px_systmr_has_expired(&systmr)) {;}
        // Reset timer
        px_systmr_reset(&systmr);
        // Toggle LED pin
        PX_LED_TOGGLE();
    }
}
