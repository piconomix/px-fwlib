/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2023 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          ST STM32C0116-DK basic GPIO example
    Author(s):      Pieter Conradie
    Creation Date:  2023-05-23
 
============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include "px_defs.h"

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_board.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____PUBLIC FUNCTIONS_____________________________________________________ */
int main(void)
{
    // Initialize board
    px_board_init();
    // LED on
    PX_USR_LED_ON();

    // Loop forever
    while(true)
    {
        // Wait 250 milliseconds
        px_board_delay_ms(250);
        // Toggle LED
        PX_USR_LED_TOGGLE();
    }	
}
