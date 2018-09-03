/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2015 Pieter Conradie <https://piconomix.com>
 
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
    
    Title:          Arduino Uno Board GPIO demo
    Author(s):      Pieter Conradie
    Creation Date:  2015-02-03
 
============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_gpio.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
// Assign Digital 2 (Port D pin 2) as LED output pin
#define PX_GPIO_LED    PX_GPIO(D, 2, PX_GPIO_DIR_OUT, PX_GPIO_INIT_LO)
// Assign Digital 3 (Port D pin 3) as Button input pin
#define PX_GPIO_PB     PX_GPIO(D, 3, PX_GPIO_DIR_IN, PX_GPIO_INIT_PULLUP)

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
    px_gpio_pin_init(&px_gpio_led);
    px_gpio_pin_init(&px_gpio_pb);

    // Repeat forever
    for(;;)
    {
        // Is button being pressed?
        if(px_gpio_pin_is_lo(&px_gpio_pb))
        {
            // Enable LED
            px_gpio_pin_set_hi(&px_gpio_led);
        }
        else
        {
            // Disable LED
            px_gpio_pin_set_lo(&px_gpio_led);
        }
    }
}
