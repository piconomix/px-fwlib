#include "px_gpio.h"

// LED is on PORT B, pin 0, configured as an output, initally off
#define PX_GPIO_LED    PX_GPIO(B, 0, PX_GPIO_DIR_OUT, PX_GPIO_INIT_LO)
// Push Button is on PORT D, pin 6, configured as an input, pull-up enabled
#define PX_GPIO_PB     PX_GPIO(D, 6, PX_GPIO_DIR_IN, PX_GPIO_INIT_PULL_UP)

/*
 *  Declare gpio handle objects as 'static const' to tell the compiler that it
 *  will not be used anywhere else ('static') and it will never change ('const').
 * 
 *  This gives the compiler the most freedom to optimize the code.
 */
static const px_gpio_handle_t px_gpio_led = {PX_GPIO_LED};
static const px_gpio_handle_t px_gpio_pb  = {PX_GPIO_PB};

int main(void)
{    
    // Initialise pins
    px_gpio_init(&px_gpio_led);
    px_gpio_init(&px_gpio_pb);

    // Loop forever
    for(;;)
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
