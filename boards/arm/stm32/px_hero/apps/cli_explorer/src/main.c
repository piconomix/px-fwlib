/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md

    Title:          Piconomix STM32 Hero Board CLI application
    Author(s):      Pieter Conradie
    Creation Date:  2018-03-01
 
============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "main.h"
#include "px_defines.h"
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
#include "px_debounce.h"
#include "px_sd.h"
#include "px_lcd_st7567_jhd12864.h"
#include "px_gfx.h"
#include "px_gfx_res.h"
#include "usb_device.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("main");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
px_uart_handle_t px_uart1_handle;
px_spi_handle_t  px_spi_sf_handle;
px_spi_handle_t  px_spi_sd_handle;
px_spi_handle_t  px_spi_lcd_handle;
px_i2c_handle_t  px_i2c_handle;
px_adc_handle_t  px_adc_handle;
px_dac_handle_t  px_dac_handle;

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

// Debounced buttons
static px_debounce_t px_debounce_pb1;
static px_debounce_t px_debounce_pb2;
static px_debounce_t px_debounce_pb3;
static px_debounce_t px_debounce_pb4;
static px_debounce_t px_debounce_pb5;
static px_debounce_t px_debounce_pb6;

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
    px_adc_init();
    px_dac_init();

    // Open UART1
    px_uart_open2(&px_uart1_handle,
                  PX_UART_NR_1,
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
    px_i2c_open(&px_i2c_handle, PX_I2C_NR_1, 0x3c);

    // Open ADC
    px_adc_open(&px_adc_handle, PX_ADC_NR_1);

    // Open DAC
    px_dac_open(&px_dac_handle, PX_DAC_NR_1);


    // Initialise SD Card driver
    px_spi_open2(&px_spi_sd_handle,
                 PX_SPI_NR_1,
                 PX_BOARD_SPI1_CS_SD,
                 px_spi_util_baud_hz_to_clk_div(PX_SD_MAX_SPI_CLOCK_HZ),
                 PX_SD_SPI_MODE, 
                 PX_SD_SPI_DATA_ORDER,
                 PX_SD_SPI_MO_DUMMY_BYTE);
    px_sd_init(&px_spi_sd_handle);

    // Initialise LCD driver
    px_spi_open2(&px_spi_lcd_handle,
                 PX_SPI_NR_2,
                 PX_BOARD_SPI2_CS_LCD,
                 px_spi_util_baud_hz_to_clk_div(PX_LCD_MAX_SPI_CLOCK_HZ),
                 PX_LCD_SPI_MODE, 
                 PX_LCD_SPI_DATA_ORDER,
                 0x00);
    px_lcd_init(&px_spi_lcd_handle);

    // Initialise AT25S Serial Flash driver
    px_spi_open2(&px_spi_sf_handle,
                 PX_SPI_NR_2,
                 PX_BOARD_SPI2_CS_SF,
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

static void main_report_button(uint8_t button)
{
    putchar('\n');
#if PX_CLI_CFG_COLOR
    // Set font color to green
    printf(PX_VT100_SET_FG_GREEN);
#endif
    printf("PB%u\n", button);
#if PX_CLI_CFG_COLOR
    // Restore font color
    printf(PX_VT100_RST_ALL_ATTRS);
#endif
    putchar('>');
}

static void main_monitor_buttons(void)
{
    // Update debounce state machines with push button states
    px_debounce_update(&px_debounce_pb1, px_gpio_pin_is_hi(&px_gpio_lcd_btn_1_lt));
    px_debounce_update(&px_debounce_pb2, px_gpio_pin_is_hi(&px_gpio_lcd_btn_2_rt));
    px_debounce_update(&px_debounce_pb3, px_gpio_pin_is_hi(&px_gpio_lcd_btn_3_up));
    px_debounce_update(&px_debounce_pb4, px_gpio_pin_is_hi(&px_gpio_lcd_btn_4_dn));
    px_debounce_update(&px_debounce_pb5, px_gpio_pin_is_hi(&px_gpio_lcd_btn_5_yes));
    px_debounce_update(&px_debounce_pb6, px_gpio_pin_is_hi(&px_gpio_lcd_btn_6_no));
    // Button pressed?
    if(px_debounce_falling_edge_detected(&px_debounce_pb1))
    {
        main_report_button(1);
    }
    if(px_debounce_falling_edge_detected(&px_debounce_pb2))
    {
        main_report_button(2);
    }
    if(px_debounce_falling_edge_detected(&px_debounce_pb3))
    {
        main_report_button(3);
    }
    if(px_debounce_falling_edge_detected(&px_debounce_pb4))
    {
        main_report_button(4);
    }
    if(px_debounce_falling_edge_detected(&px_debounce_pb5))
    {
        main_report_button(5);
    }
    if(px_debounce_falling_edge_detected(&px_debounce_pb6))
    {
        main_report_button(6);
    }
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
    PX_LOG_TRACE("Debug enabled\n");

    // Initialize debounced buttons
    px_debounce_init(&px_debounce_pb1, px_gpio_pin_is_hi(&px_gpio_lcd_btn_1_lt));
    px_debounce_init(&px_debounce_pb2, px_gpio_pin_is_hi(&px_gpio_lcd_btn_2_rt));
    px_debounce_init(&px_debounce_pb3, px_gpio_pin_is_hi(&px_gpio_lcd_btn_3_up));
    px_debounce_init(&px_debounce_pb4, px_gpio_pin_is_hi(&px_gpio_lcd_btn_4_dn));
    px_debounce_init(&px_debounce_pb5, px_gpio_pin_is_hi(&px_gpio_lcd_btn_5_yes));
    px_debounce_init(&px_debounce_pb6, px_gpio_pin_is_hi(&px_gpio_lcd_btn_6_no));

    // Beep
    px_board_buzzer_on(4000);
    px_board_delay_ms(100);
    px_board_buzzer_off();

    // Draw LCD splash screen
    PX_LCD_BACKLIGHT_ON();
    px_gfx_init();
    px_gfx_align_set(PX_GFX_ALIGN_MID);
    px_gfx_draw_img(&px_gfx_img_hero_logo, PX_GFX_X_MID, PX_GFX_Y_MID);    
    px_gfx_draw_update();
    px_gfx_draw_prop_reset();

    // Initialize CLI
    px_cli_init(px_cli_cmd_list, main_cli_init_str);

    for(;;)
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
        // Received byte over UART?
        if(px_uart_rd_u8(&px_uart1_handle, &data))
        {
            // Lower case?
            if((data >='a') && (data <='z'))
            {
                // Capitalise
                data = data - 'a' + 'A';
            }
            // Return for loopback testing
            px_uart_wr_u8(&px_uart1_handle, data);
        }
        // Monitor and report button presses
        main_monitor_buttons();

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
    // New line character?
    if(data == '\n')
    {
        // Prepend a carriage return
        main_log_putchar('\r');
    }

    px_uart_put_char(&px_uart1_handle, data);
}

void main_log_timestamp(char * str)
{
    px_rtc_date_time_t date_time;

    px_rtc_date_time_rd(&date_time);

    sprintf(str, "%02hu:%02hu:%02hu",
                 (unsigned short)date_time.hour,
                 (unsigned short)date_time.min,
                 (unsigned short)date_time.sec);
}
