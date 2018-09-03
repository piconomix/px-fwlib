#include "px_board.h"
#include "px_compiler.h"
#include "px_uart.h"

// Create UART handle object
static px_uart_handle_t px_uart_handle;

int main(void)
{
    uint8_t data;
 
    // Initialise board
    px_board_init();

    // Initialise module
    px_uart_init();

    // Enable interrupts
    px_interrupts_enable();

    // Open UART1 @ 115200 BAUD, 8 data bits, no parity, 1 stop bit
    px_uart_open2(&px_uart_handle,
                  PX_UART_PER_1, 
                  115200, 
                  PX_UART_DATA_BITS_8, 
                  PX_UART_PARITY_NONE, 
                  PX_UART_STOP_BITS_1);

    // Repeat forever
    for(;;)
    {
        // Wait until a byte is received
        if(px_uart_rd_u8(&px_uart_handle, &data))
        {
            // Send back received byte (loopback/echo mode)
            px_uart_wr_u8(&px_uart_handle, data);
        }
    }
}
