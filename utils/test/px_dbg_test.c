#include "px_defines.h"
#include "px_uart.h"
#include "px_uart_stdio.h"

#include "px_dbg.h"
PX_DBG_DECL_NAME("px_dbg_test")

// Declare UART handle structure
px_uart_handle_t px_uart_handle;

uint8_t calc(uint8_t val)
{
    uint8_t answer;

    // Report function call with parameter value
    PX_DBG_INFO("calc(val = %u)", val);

    // Is val equal to zero?
    if(val == 0)
    {
        PX_DBG_ERR("val may not be equal to zero");
        return 0;
    }

    // Calculate answer
    answer = 100 / val;

    // Report answer
    PX_DBG_INFO("answer = %u", answer);

    return answer;
}

void px_dbg_test(void)
{
    uint8_t val;
    uint8_t answer;
 
    // Initialise modules
    px_board_init();
    px_uart_init();

    // Open UART0 @ 115200 BAUD, 8 data bits, no parity, 1 stop bit
    px_uart_open2(&px_uart_handle,
                  PX_UART_PER_0, 
                  115200, 
                  PX_UART_DATA_BITS_8, 
                  PX_UART_PARITY_NONE, 
                  PX_UART_STOP_BITS_1);

    // Direct stdio to UART0
    px_uart_stdio_init(&px_uart_handle);

    // Enable interrupts
    px_interrupts_enable();
   
    PX_DBG_TRACE("Calculation started\n");
    for(val = 0; val < 3; val++)
    {
        // Calculate answer
        answer = calc(val);
    }
    PX_DBG_TRACE("Calculation finished\n");
}
