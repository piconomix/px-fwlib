#include "px_gpio.h"

// LED is on PORT B, pin 0, configured as an output, initally off
#define PX_GPIO_LED    PX_GPIO(B, 0, PX_GPIO_DIR_OUT, PX_GPIO_INIT_LO)
// Push Button is on PORT D, pin 7, configured as an input, pull-up enabled
#define PX_GPIO_PB     PX_GPIO(D, 7, PX_GPIO_DIR_IN, PX_GPIO_INIT_PULL_UP)

int main(void)
{
    // Initialise pins
    PX_GPIO_INIT(PX_GPIO_LED);
    PX_GPIO_INIT(PX_GPIO_PB);

    // Repeat forever
    for(;;)
    {
        // Is button being pressed?
        if(PX_GPIO_IN_IS_LO(PX_GPIO_PB))
        {
            // Enable LED
            PX_GPIO_OUT_SET_HI(PX_GPIO_LED);
        }
        else
        {
            // Disable LED
            PX_GPIO_OUT_SET_LO(PX_GPIO_LED);
        }
    }
}
