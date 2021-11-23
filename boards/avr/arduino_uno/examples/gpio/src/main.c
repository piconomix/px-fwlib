/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2015 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          Arduino Uno Board GPIO demo
    Author(s):      Pieter Conradie
    Creation Date:  2015-02-03
 
============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"
#include "px_gpio.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
// Assign Digital 2 (Port D pin 2) as LED output pin
#define PX_GPIO_LED    PX_GPIO(D, 2, PX_GPIO_DIR_OUT, PX_GPIO_INIT_LO)
// Assign Digital 3 (Port D pin 3) as Button input pin
#define PX_GPIO_PB     PX_GPIO(D, 3, PX_GPIO_DIR_IN, PX_GPIO_INIT_PULL_UP)

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static const px_gpio_handle_t px_gpio_led = {PX_GPIO_LED};
static const px_gpio_handle_t px_gpio_pb  = {PX_GPIO_PB};

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____PUBLIC FUNCTIONS_____________________________________________________ */
int main(void)
{    
    // Initialise pins
    px_gpio_init(&px_gpio_led);
    px_gpio_init(&px_gpio_pb);

    // Loop forever
    while(true)
    {
        // Is button being pressed?
        if(px_gpio_in_is_lo(&px_gpio_pb))
        {
            // Enable LED
            px_gpio_out_set_hi(&px_gpio_led);
        }
        else
        {
            // Disable LED
            px_gpio_out_set_lo(&px_gpio_led);
        }
    }
}
