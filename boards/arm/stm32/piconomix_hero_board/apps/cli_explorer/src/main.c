/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
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

    Title:          Piconomix STM32L072 Hero Board CLI application
    Author(s):      Pieter Conradie
    Creation Date:  2018-03-01
 
============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "main.h"
#include "px_defines.h"
#include "px_compiler.h"
#include "px_cli.h"
#include "px_at45d.h"
#include "px_systmr.h"
#include "px_rtc.h"

#include "px_board.h"
#include "px_uart.h"
//#include "px_uart_stdio.h"
#include "px_usb_cdc_stdio.h"
#include "px_spi.h"
#include "px_i2c.h"
#include "px_sysclk.h"
#include "px_sd.h"
#include "px_lcd_st7565p_tse2g0330e.h"
#include "px_gfx.h"
#include "px_gfx_res.h"

#include "usb_device.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
px_uart_handle_t px_uart1_handle;
px_spi_handle_t  px_spi_df_handle;
px_spi_handle_t  px_spi_sd_handle;
px_spi_handle_t  px_spi_lcd_handle;
px_i2c_handle_t  px_i2c_handle;

uint8_t          main_buffer[MAIN_BUFFER_SIZE];

bool             main_usb_connected_event_flag;

/* _____LOCAL VARIABLES______________________________________________________ */             
/// CLI splash text on start up. Back slashes ('\') have to be escaped ('\\')
static const char main_cli_init_str[] =
    "  ______    ______   _____     ____ TM\n"
    " / |  | \\  |  ____| |  __ \\   / __ \\\n"
    "/| |__| |\\ | |__    | |__) | | |  | |\n"
    "\\|  __  |/ |  __|   |  _  /  | |  | |\n"
    " \\ |  | /  | |____  | | \\ \\  | |__| |\n"
    "  \\|  |/   |______| |_|  \\_\\  \\____/\n"
    "   \\  / PICONOMIX - Embedded Elegance\n"
    "    \\/          https://piconomix.com\n\n";

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static bool main_init(void)
{
    // Initialize modules
    px_board_init();
    px_sysclk_init();
    px_rtc_init();
    px_uart_init();
    px_spi_init();
    px_i2c_init();

    // Open UART1
    px_uart_open2(&px_uart1_handle,
                  PX_UART_PER_1,
                  115200, 
                  PX_UART_DATA_BITS_8, 
                  PX_UART_PARITY_NONE, 
                  PX_UART_STOP_BITS_1);
#if 0
    // Connect stdin and stdout to UART1
    px_uart_stdio_init(&px_uart1_handle);
#endif

    // Connect stdin and stdout to USB CDC circular buffers
    px_usb_cdc_stdio_init();

    // Start USB driver
    MX_USB_DEVICE_Init();

    // Open I2C1
    px_i2c_open(&px_i2c_handle, PX_I2C_PER_1, 0x3c);

    // Initialise SD Card driver
    px_spi_open2(&px_spi_sd_handle,
                 PX_SPI_PER_1,
                 PX_BOARD_SPI1_CS_SD,
                 px_spi_util_baud_hz_to_clk_div(PX_SD_MAX_SPI_CLOCK_HZ),
                 PX_SD_SPI_MODE, 
                 PX_SD_SPI_DATA_ORDER,
                 PX_SD_SPI_MO_DUMMY_BYTE);
    px_sd_init(&px_spi_sd_handle);

    // Initialise LCD driver
    px_spi_open2(&px_spi_lcd_handle,
                 PX_SPI_PER_2,
                 PX_BOARD_SPI2_CS_LCD,
                 px_spi_util_baud_hz_to_clk_div(PX_LCD_MAX_SPI_CLOCK_HZ),
                 PX_LCD_SPI_MODE, 
                 PX_LCD_SPI_DATA_ORDER,
                 0x00);
    px_lcd_init(&px_spi_lcd_handle);

    // Initialise AT45D DataFlash driver
    px_spi_open2(&px_spi_df_handle,
                 PX_SPI_PER_2,
                 PX_BOARD_SPI2_CS_DF,
                 px_spi_util_baud_hz_to_clk_div(PX_AT45D_MAX_SPI_CLOCK_HZ), 
                 PX_AT45D_SPI_MODE, 
                 PX_AT45D_SPI_DATA_ORDER,
                 0x00);
    px_at45d_init(&px_spi_df_handle);

    // Resume DataFlash in case it was left in a power down state and the 
    // processor reset
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

    // Initialise board and peripheral drivers
    main_init();

    // Enable LED
    PX_USR_LED_ON();

    // Beep
    px_board_buzzer_on(4000);
    px_systmr_wait(PX_SYSTMR_MS_TO_TICKS(100));
    px_board_buzzer_off();

    // Draw LCD splash screen
    px_gfx_init();
    px_gfx_draw_img((PX_GFX_DISP_SIZE_X - px_gfx_img_hero_logo_64x42.width) / 2, 
                    (PX_GFX_DISP_SIZE_Y - px_gfx_img_hero_logo_64x42.height) / 2,
                    px_gfx_img_hero_logo_64x42.width,
                    px_gfx_img_hero_logo_64x42.height,
                    PX_GFX_COLOR_ON,
                    px_gfx_img_hero_logo_64x42.data);
    px_gfx_update();

    // Initialise CLI
    px_cli_init(main_cli_init_str);

    for(;;)
    {
#if 0
        // Byte has been received?
        if(px_uart_rd_u8(&px_uart1_handle, &data))
        {
            // Pass received byte to Command Line Interpreter module
            px_cli_on_rx_char((char)data);
        }
#else
        if(main_usb_connected_event_flag)
        {
            main_usb_connected_event_flag = false;
            // (Re)initialise CLI
            px_cli_init(main_cli_init_str);
        }

        // Byte has been received over USB?
        if(px_usb_cdc_stdio_rd_u8(&data))
        {
            // Pass received byte to Command Line Interpreter module
            px_cli_on_rx_char((char)data);
        }
#endif
        // Put core into SLEEP mode until an interrupt occurs
        __WFI();
    }
}

void main_usb_event_connected(void)
{
    main_usb_connected_event_flag = true;
}

void main_dbg_put_char(char data)
{
    // New line character?
    if(data == '\n')
    {
        // Prepend a carriage return
        main_dbg_put_char('\r');
    }

    px_uart_put_char(&px_uart1_handle, data);
}

void main_dbg_timestamp(char * str)
{
    px_rtc_date_time_t date_time;

    px_rtc_date_time_rd(&date_time);

    sprintf(str, "%02hu:%02hu:%02hu",
                 (unsigned short)date_time.hour,
                 (unsigned short)date_time.min,
                 (unsigned short)date_time.sec);
}
