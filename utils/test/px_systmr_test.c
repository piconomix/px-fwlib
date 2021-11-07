#include "px_sysclk.h"
#include "px_systmr.h"
#include "px_compiler.h"

void px_systmr_test(void)
{
    // Create a timer object
    px_systmr_t tmr;

    // Initialise module
    px_sysclk_init();

    // Enable interrupts
    px_interrupts_enable();
  
    // Start timer with a 250 ms timeout
    px_systmr_start(&tmr, TMR_MS_TO_TICKS(250));
    
    // Loop forever
    while(true)
    {
        // Wait until timer has expired
        while(!px_systmr_has_expired(&tmr))
        {
            ;
        }

        // Restart timer
        px_systmr_restart(&tmr);
        
        // Do someting...
    }
}
