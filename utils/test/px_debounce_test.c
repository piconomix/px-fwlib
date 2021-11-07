#include "px_debounce.h"
#include "px_gpio.h"
#include "px_sysclk.h"
#include "px_systmr.h"
#include "px_compiler.h"

// LED is on PORT B, pin 2, configured as an output, initially off
#define PX_GPIO_LED    PX_GPIO(B, 0, PX_GPIO_DIR_OUT, PX_GPIO_INIT_LO)
// Push Button is on PORT B, pin 0, configured as an input, pull-up enabled
#define PX_GPIO_PB     PX_GPIO(D, 7, PX_GPIO_DIR_IN, PX_GPIO_INIT_PULL_UP)

// Declare debounce state tracking structure
px_debounce_t px_debounce_pb;

int main(void)
{   
    // Initialise module
    px_sysclk_init();

    // Enable interrupts
    px_interrupts_enable();

    // Initialise GPIO pins
    PX_GPIO_INIT(PX_GPIO_LED);
    PX_GPIO_INIT(PX_GPIO_PB);

    // Initialise debounce state
    px_debounce_init(&px_debounce_pb, PX_GPIO_IN_IS_HI(PX_GPIO_PB));

    // Loop forever
    for(;;)
    {
        // Wait one systmr tick
        px_systmr_wait(1);

        // Update debounce state
        px_debounce_update(&px_debounce_pb, PX_GPIO_IN_IS_HI(PX_GPIO_PB));

        // Has button been pressed?
        if(px_debounce_falling_edge_detected(&px_debounce_pb))
        {
            // Enable LED
            PX_GPIO_OUT_SET_HI(PX_GPIO_LED);
        }
        
        // Has button been released?
        if(px_debounce_rising_edge_detected(&px_debounce_pb))
        {
            // Disable LED
            PX_GPIO_OUT_SET_LO(PX_GPIO_LED);
        }
    }
}
