/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          ST Nucleo64 L053 minimal printf to UART demo
    Author(s):      Pieter Conradie
    Creation Date:  2018-02-06
 
============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

#include "px_board.h"
#include "px_uart.h"
#include "px_uart_stdio.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
px_uart_handle_t px_uart_handle;

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static bool main_init(void)
{
    // Initialize modules
    px_board_init();
    px_uart_init();
    px_uart_open2(&px_uart_handle,
                  PX_UART_NR_2,
                  115200, 
                  PX_UART_DATA_BITS_8, 
                  PX_UART_PARITY_NONE, 
                  PX_UART_STOP_BITS_1);
    px_uart_stdio_init(&px_uart_handle);

    // Success
    return true;
}

/* _____PUBLIC FUNCTIONS_____________________________________________________ */
int main(void)
{
    uint8_t data;

    main_init();
      
    // Enable LED
    PX_USR_LED_ON();

    // Send message
    printf("Hello World!\n");

    // Loop forever
    while(true)
    {
        // Byte has been received?
        if(px_uart_rd_u8(&px_uart_handle, &data))
        {
            // Echo byte
            px_uart_wr_u8(&px_uart_handle, data);
        }
        // Put core into SLEEP mode until an interrupt occurs
        __WFI();
    }
}
