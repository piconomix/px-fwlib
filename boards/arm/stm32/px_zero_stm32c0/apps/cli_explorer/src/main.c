/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2025 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          PX-ZER0 STM32C0 CLI application
    Author(s):      Pieter Conradie
    Creation Date:  2025-05-25
 
============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdio.h>
#include <sys/stat.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "main.h"
#include "px_defs.h"
#include "px_compiler.h"
#include "px_cli.h"
#include "px_cli_cmds.h"
#include "px_vt100.h"
#include "px_at25s.h"
#include "px_systmr.h"
#include "px_rtc.h"
#include "px_board.h"
#include "px_uart.h"
//#include "px_uart_stdio.h"
#include "px_usb_cdc_stdio.h"
#include "px_adc.h"
#include "px_spi.h"
#include "px_i2c.h"
#include "px_sysclk.h"
#include "usb_device.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("main");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
px_uart_handle_t px_uart2_dbg_handle;
px_spi_handle_t  px_spi_sf_handle;
px_i2c_handle_t  px_i2c_handle;
px_adc_handle_t  px_adc_handle;

uint8_t          main_buf[MAIN_BUF_SIZE];

bool             main_usb_connected_event_flag;

/* _____LOCAL VARIABLES______________________________________________________ */             
/// CLI splash text on start up. Back slashes ('\') have to be escaped ('\\')
static const char main_cli_init_str[] =
    " ___________   ______   ______     _____ TM\n"
    "/_____   /  \\ |  ____| |  ___ \\   / __  \\\n"
    "\\    /  /   / | |__    | |___) | | | /   |\n"
    " \\  /  /   /  |  __|   |  __  /  | |/ /| |\n"
    "  \\/  /___/   | |____  | |  \\ \\  |   /_| |\n"
    "   \\_____/    |______| |_|   \\_\\  \\_____/\n"
    "    \\   /    PICONOMIX - Embedded Elegance\n"
    "     \\_/             https://piconomix.com\n\n";

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
    //px_adc_init();

    // Open UART1
    px_uart_open2(&px_uart2_dbg_handle,
                  PX_UART_NR_2,
                  115200, 
                  PX_UART_DATA_BITS_8, 
                  PX_UART_PARITY_NONE, 
                  PX_UART_STOP_BITS_1);
#if 0
    // Connect stdin and stdout to UART1
    px_uart_stdio_init(&px_uart2_dbg_handle);
#endif
    // Connect stdin and stdout to USB CDC circular buffers
    px_usb_cdc_stdio_init();

    // Start USB driver
    MX_USB_Device_Init();

    // Open I2C1
    px_i2c_open(&px_i2c_handle, PX_I2C_NR_1, 0x3c);
    // Open ADC
    //px_adc_open(&px_adc_handle, PX_ADC_NR_1);

    // Initialise AT25S Serial Flash driver
    px_spi_open2(&px_spi_sf_handle,
                 PX_SPI_NR_1,
                 PX_BOARD_SPI1_CS_SF,
                 px_spi_util_baud_hz_to_clk_div(PX_AT25S_MAX_SPI_CLOCK_HZ),
                 PX_AT25S_SPI_MODE, 
                 PX_AT25S_SPI_DATA_ORDER,
                 0x00);
    px_at25s_init(&px_spi_sf_handle);
    // Resume Serial Flash in case it was left in a power down state and the
    // processor reset
    px_at25s_resume_from_deep_power_down();

    // Success
    return true;
}

/* _____PUBLIC FUNCTIONS_____________________________________________________ */
int main(void)
{
    uint8_t data;

    // Initialize board and peripheral drivers
    main_init();
    // Enable LED
    PX_USR_LED_ON();
    // Report debug output
    PX_LOG_I("PX-ZER0 STM32C0 " __DATE__ " " __TIME__);
    // Beep
    px_board_buzzer_on(4000);
    px_board_delay_ms(100);
    px_board_buzzer_off();
    // Initialize CLI
    px_cli_init(px_cli_cmd_list, main_cli_init_str);

    // Loop forever
    while(true)
    {
        // USB connected event?
        if(main_usb_connected_event_flag)
        {
            PX_LOG_I("USB Connect event. (Re)initialising CLI");
            main_usb_connected_event_flag = false;
            // (Re)initialise CLI
            px_cli_init(px_cli_cmd_list, main_cli_init_str);
        }
        // Received byte over USB?
        if(px_usb_cdc_stdio_rd_u8(&data))
        {
            // Pass received byte to Command Line Interpreter module
            px_cli_on_rx_char((char)data);
        }
        // Put core into SLEEP mode until an interrupt occurs
        __WFI();
    }
}

void main_usb_event_connected(void)
{
    main_usb_connected_event_flag = true;
}

void main_log_putchar(char data)
{
    px_uart_putchar(&px_uart2_dbg_handle, data);
}

void main_log_print(const char * str)
{
    size_t nr_of_bytes_written;
    size_t nr_of_bytes_to_write = strlen(str);

    while(nr_of_bytes_to_write != 0)
    {
        nr_of_bytes_written = px_uart_wr(&px_uart2_dbg_handle, str, nr_of_bytes_to_write);

        if(nr_of_bytes_written == nr_of_bytes_to_write) return;

        str                  += nr_of_bytes_written;
        nr_of_bytes_to_write -= nr_of_bytes_written;
    }
}

void main_log_timestamp(char * str)
{
#if 0
    px_rtc_date_time_t date_time;
    px_rtc_date_time_rd(&date_time);
    sprintf(str, "%02hu:%02hu:%02hu",
                 (unsigned short)date_time.hour,
                 (unsigned short)date_time.min,
                 (unsigned short)date_time.sec);
#else
    {
    uint8_t  i;
    uint8_t  j;
    char     timestamp[12];

    sprintf(timestamp, "%08lu", px_sysclk_get_tick_count());

    i = 0;
    j = 0;
    while(i < 5)
    {
        str[i++] = timestamp[j++];
    }
    // Insert
    str[i++] = '.';
    while(i < 9)
    {
        str[i++] = timestamp[j++];
    }
    str[i] = '\0';
}
#endif
}

int _close(int file)
{
	return -1;
}

int _fstat(int file, struct stat *st)
{
	st->st_mode = S_IFCHR;
	return 0;
}

int _isatty(int file)
{
	return 1;
}

int _lseek(int file, int ptr, int dir)
{
	return 0;
}

void Error_Handler(void)
{
    while(1)
    {
    }
}

