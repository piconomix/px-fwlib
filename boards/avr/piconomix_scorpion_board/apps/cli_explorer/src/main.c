/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2014 Pieter Conradie <https://piconomix.com>
 
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

    Title:          Piconomix ATmega328P Scorpion Board CLI application
    Author(s):      Pieter Conradie
    Creation Date:  2014-06-01
 
============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <avr/sleep.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "main.h"
#include "px_defines.h"
#include "px_pgm_P.h"
#include "px_cli.h"
#include "px_at45d.h"
#include "px_systmr.h"
#include "px_rtc_util.h"
#include "px_debounce.h"

#include "px_board.h"
#include "px_uart.h"
#include "px_uart_stdio.h"
#include "px_spi.h"
#include "px_i2c.h"
#include "px_sysclk.h"
#include "px_buzzer.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
px_uart_handle_t px_uart_handle;
px_spi_handle_t  px_at45d_spi_handle;
px_i2c_handle_t  px_i2c_handle;

uint8_t          main_buffer[MAIN_BUFFER_SIZE];

/* _____LOCAL VARIABLES______________________________________________________ */
/// CLI splash text on start up. Back slashes ('\') have to be escaped ('\\')
PX_ATTR_PGM static const char main_cli_init_str[] =
    " ____   __  __ TM\n"
    "|  _ \\  \\ \\/ /\n"
    "| |_) |  \\  /\n"
    "|  __/   /  \\ PICONOMIX - Embedded Elegance\n"
    "|_|     /_/\\_\\    https://piconomix.com\n\n";

static px_debounce_t px_debounce_pb;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static bool main_init(void)
{
    // Initialize modules
    px_board_init();
    px_sysclk_init();
    px_rtc_util_init();
    px_buzzer_init();
    px_uart_init();
    px_uart_open2(&px_uart_handle,
                  PX_UART_PER_0,
                  115200, 
                  PX_UART_DATA_BITS_8, 
                  PX_UART_PARITY_NONE, 
                  PX_UART_STOP_BITS_1);
    px_uart_stdio_init(&px_uart_handle);

    px_spi_init();
    px_i2c_init();
    px_i2c_open(&px_i2c_handle, PX_I2C_PER_0, 0x00);
    
    // Enable interrupts
    px_interrupts_enable();

    // Initialise AT45D driver
    px_spi_open2(&px_at45d_spi_handle,
                 PX_SPI_PER_0,
                 PX_BOARD_SPI_CS_DF,
                 px_spi_util_baud_hz_to_clk_div(PX_AT45D_MAX_SPI_CLOCK_HZ),
                 PX_AT45D_SPI_MODE, 
                 PX_AT45D_SPI_DATA_ORDER,
                 0x00);
    px_at45d_init(&px_at45d_spi_handle);

    // Resume DataFlash in case it was left in a power down state and the AVR reset
    px_at45d_resume_from_power_down();

    // Check DataFlash communication
    if((px_at45d_get_status()&0xfe) != ((1<<PX_AT45D_STATUS_READY) | (0x7 << 2)))
    {
        return false;
    }

    // Success
    return true;
}

/* _____PUBLIC FUNCTIONS_____________________________________________________ */
int main(void)
{
    uint8_t data;

    main_init();
    px_debounce_init(&px_debounce_pb, PX_GPIO_PIN_IS_HI(PX_GPIO_PB));
      
    // Enable LED
    PX_LED_ON();

    // Beep
    px_buzzer_on(4000);
    px_systmr_wait(PX_SYSTMR_MS_TO_TICKS(100));
    px_buzzer_off();

    // Initialise CLI (after STDIO is ready)
    px_cli_init(main_cli_init_str);

    for(;;)
    {
        // Update debounce state machine with push button state
        px_debounce_update(&px_debounce_pb, PX_GPIO_PIN_IS_HI(PX_GPIO_PB));
        // Button pressed?
        if(px_debounce_falling_edge_detected(&px_debounce_pb))
        {
            // Indicate that key has been pressed
            printf("\nPB\n>");
        }

        // Byte has been received?
        if(px_uart_rd_u8(&px_uart_handle, &data))
        {
            // Pass received byte to Command Line Interpreter module
            px_cli_on_rx_char((char)data);
        }

        // Sleep in Idle mode until an interrupt wakes up the CPU
        set_sleep_mode(SLEEP_MODE_IDLE); 
        sleep_mode();
    }
}
