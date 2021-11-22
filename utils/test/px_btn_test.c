#include "px_btn.h"
#include "px_gpio.h"
#include "px_sysclk.h"
#include "px_systmr.h"
#include "px_compiler.h"

// LED is on PORT B, pin 2, configured as an output, initially off
#define PX_GPIO_LED    PX_GPIO(B, 0, PX_GPIO_DIR_OUT, PX_GPIO_INIT_LO)
// Push Button is on PORT B, pin 0, configured as an input, pull-up enabled
#define PX_GPIO_PB     PX_GPIO(D, 7, PX_GPIO_DIR_IN, PX_GPIO_INIT_PULL_UP)

// Create a button object
px_btn_t px_btn;

int main(void)
{   
    // Initialise module
    px_sysclk_init();

    // Enable interrupts
    px_interrupts_enable();

    // Initialise GPIO pins
    PX_GPIO_INIT(PX_GPIO_LED);
    PX_GPIO_INIT(PX_GPIO_PB);

    // Initialise button state
    px_btn_init(&px_btn, PX_GPIO_IN_IS_LO(PX_GPIO_PB));

    // Loop forever
    while(true)
    {
        // Wait one systmr tick
        px_systmr_wait(1);

        // Update button state
        px_btn_update(&px_btn, PX_GPIO_IN_IS_LO(PX_GPIO_PB));

        // Has button been pressed?
        if(px_btn_event_press(&px_btn))
        {
            // Enable LED
            PX_GPIO_OUT_SET_HI(PX_GPIO_LED);
        }
        
        // Has button been released?
        if(px_btn_event_release(&px_btn))
        {
            // Disable LED
            PX_GPIO_OUT_SET_LO(PX_GPIO_LED);
        }
    }
}
