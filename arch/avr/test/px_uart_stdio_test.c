#include <stdio.h>

#include "px_board.h"
#include "px_uart.h"
#include "px_uart_stdio.h"

// Create UART handle object
px_uart_handle_t px_uart_handle;

int main(void)
{
    uint8_t counter;

    // Initialise board
    px_board_init();
    // Initialise module
    px_uart_init();

    // Open UART0 @ 115200 BAUD, 8 data bits, no parity, 1 stop bit
    px_uart_open2(&px_uart_handle,
                  PX_UART_NR_0,
                  115200, 
                  PX_UART_DATA_BITS_8, 
                  PX_UART_PARITY_NONE, 
                  PX_UART_STOP_BITS_1);
    // Initialise stdio stream to use the specified UART
    px_uart_stdio_init(&px_uart_handle);

    // Enable interrupts
    px_interrupts_enable();

    printf("Starting counter...\n");
    for(counter = 0; counter < 8; counter++)
    {
        printf("Counter = %u\n", counter);
    }
}
