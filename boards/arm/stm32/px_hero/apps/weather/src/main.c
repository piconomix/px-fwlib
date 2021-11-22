/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          Piconomix STM32 Hero Board CLI application
    Author(s):      Pieter Conradie
    Creation Date:  2018-03-01
 
============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "main.h"
#include "px_defines.h"
#include "px_compiler.h"
#include "px_at25s.h"
#include "px_systmr.h"
#include "px_rtc.h"
#include "px_board.h"
#include "px_uart.h"
#include "px_uart_stdio.h"
#include "px_spi.h"
#include "px_i2c.h"
#include "px_sysclk.h"
#include "px_btn.h"
#include "px_sd.h"
#include "px_lcd_st7567_jhd12864.h"
#include "px_gfx.h"
#include "px_gfx_res.h"
#include "px_bme280.h"
#include "px_gfx.h"
#include "px_gfx_objects.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("main");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
px_uart_handle_t px_uart1_handle;
px_spi_handle_t  px_spi_sf_handle;
px_spi_handle_t  px_spi_sd_handle;
px_spi_handle_t  px_spi_lcd_handle;
px_i2c_handle_t  px_i2c_bme280_handle;

uint8_t          main_buffer[MAIN_BUFFER_SIZE];

/* _____LOCAL VARIABLES______________________________________________________ */             
// Debounced buttons
static px_btn_t px_btn_1;
static px_btn_t px_btn_2;
static px_btn_t px_btn_3;
static px_btn_t px_btn_4;
static px_btn_t px_btn_5;
static px_btn_t px_btn_6;
// Timer
static px_systmr_t tmr;

// Graphic objects
static px_gfx_obj_handle_t obj_win;
static px_gfx_obj_handle_t obj_label_temp;
static px_gfx_obj_handle_t obj_label_temp_val;
static char                obj_label_temp_val_str[16];
static px_gfx_obj_handle_t obj_label_press;
static px_gfx_obj_handle_t obj_label_press_val;
static char                obj_label_press_val_str[16];
static px_gfx_obj_handle_t obj_label_hum;
static px_gfx_obj_handle_t obj_label_hum_val;
static char                obj_label_hum_val_str[16];

static const px_gfx_obj_label_prop_t obj_label_prop_temp = 
{
    .x        = PX_GFX_X_MID,
    .y        = 0,
    .str      = "TEMPERATURE",
    .font     = &px_gfx_font_3x5,
    .color_fg = PX_GFX_COLOR_ON,
    .color_bg = PX_GFX_COLOR_OFF,
    .align    = PX_GFX_ALIGN_TOP_MID,
};

static const px_gfx_obj_label_prop_t obj_label_prop_temp_val = 
{
    .x        = PX_GFX_X_MID,
    .y        = 6,
    .str      = obj_label_temp_val_str,
    .font     = &px_gfx_font_11x14,
    .color_fg = PX_GFX_COLOR_ON,
    .color_bg = PX_GFX_COLOR_OFF,
    .align    = PX_GFX_ALIGN_TOP_MID,
};

static const px_gfx_obj_label_prop_t obj_label_prop_press = 
{
    .x        = PX_GFX_X_MID,
    .y        = 21,
    .str      = "PRESSURE",
    .font     = &px_gfx_font_3x5,
    .color_fg = PX_GFX_COLOR_ON,
    .color_bg = PX_GFX_COLOR_OFF,
    .align    = PX_GFX_ALIGN_TOP_MID,
};

static const px_gfx_obj_label_prop_t obj_label_prop_press_val = 
{
    .x        = PX_GFX_X_MID,
    .y        = 27,
    .str      = obj_label_press_val_str,
    .font     = &px_gfx_font_11x14,
    .color_fg = PX_GFX_COLOR_ON,
    .color_bg = PX_GFX_COLOR_OFF,
    .align    = PX_GFX_ALIGN_TOP_MID,
};

static const px_gfx_obj_label_prop_t obj_label_prop_hum = 
{
    .x        = PX_GFX_X_MID,
    .y        = 42,
    .str      = "HUMIDITY",
    .font     = &px_gfx_font_3x5,
    .color_fg = PX_GFX_COLOR_ON,
    .color_bg = PX_GFX_COLOR_OFF,
    .align    = PX_GFX_ALIGN_TOP_MID,
};

static const px_gfx_obj_label_prop_t obj_label_prop_hum_val = 
{
    .x        = PX_GFX_X_MID,
    .y        = 48,
    .str      = obj_label_hum_val_str,
    .font     = &px_gfx_font_11x14,
    .color_fg = PX_GFX_COLOR_ON,
    .color_bg = PX_GFX_COLOR_OFF,
    .align    = PX_GFX_ALIGN_TOP_MID,
};

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
                  PX_UART_NR_1,
                  115200, 
                  PX_UART_DATA_BITS_8, 
                  PX_UART_PARITY_NONE, 
                  PX_UART_STOP_BITS_1);
    // Connect stdin and stdout to UART1
    px_uart_stdio_init(&px_uart1_handle);
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
    // Open handle to BME280 sensor
    px_i2c_open(&px_i2c_bme280_handle, PX_I2C_NR_1, PX_BME280_I2C_SLA_ADR);

    // Success
    return true;
}

static void main_gfx_start(void)
{
    // Initialise graphics
    px_gfx_init();
    PX_LCD_BACKLIGHT_ON();
    // Create full display window
    obj_win = px_gfx_obj_win_create(&px_gfx_obj_win_prop_full_disp);
    // Create objects
    obj_label_temp      = px_gfx_obj_label_create(&obj_label_prop_temp);
    obj_label_temp_val  = px_gfx_obj_label_create(&obj_label_prop_temp_val);
    obj_label_press     = px_gfx_obj_label_create(&obj_label_prop_press);
    obj_label_press_val = px_gfx_obj_label_create(&obj_label_prop_press_val);
    obj_label_hum       = px_gfx_obj_label_create(&obj_label_prop_hum);
    obj_label_hum_val   = px_gfx_obj_label_create(&obj_label_prop_hum_val);
    // Add objects to window
    px_gfx_obj_win_add_child(obj_win, obj_label_temp);
    px_gfx_obj_win_add_child(obj_win, obj_label_temp_val);
    px_gfx_obj_win_add_child(obj_win, obj_label_press);
    px_gfx_obj_win_add_child(obj_win, obj_label_press_val);
    px_gfx_obj_win_add_child(obj_win, obj_label_hum);
    px_gfx_obj_win_add_child(obj_win, obj_label_hum_val);
}

static void main_gfx_update_temp(int32_t temp)
{
    sprintf(obj_label_temp_val_str, "%2.2f C", (float)temp / 100.0f);
    px_gfx_obj_update_set(obj_label_temp_val, true);
}

static void main_gfx_update_press(uint32_t press)
{
    sprintf(obj_label_press_val_str, "%lu Pa", press);
    px_gfx_obj_update_set(obj_label_press_val, true);
}

static void main_gfx_update_hum(uint32_t hum)
{
    sprintf(obj_label_hum_val_str, "%lu %%", hum / 1024);
    px_gfx_obj_update_set(obj_label_hum_val, true);
}

/* _____PUBLIC FUNCTIONS_____________________________________________________ */
int main(void)
{
    // Initialize board and peripheral drivers
    main_init();
    // Report debug output
    PX_LOG_TRACE("Debug enabled\n");
    // Initialize debounced buttons
    px_btn_init(&px_btn_1, false);
    px_btn_init(&px_btn_2, false);
    px_btn_init(&px_btn_3, false);
    px_btn_init(&px_btn_4, false);
    px_btn_init(&px_btn_5, false);
    px_btn_init(&px_btn_6, false);
    // Beep
    px_board_buzzer_on(4000);
    px_board_delay_ms(100);
    px_board_buzzer_off();
    // Initialise and detect BME280 sensor
    px_bme280_init(&px_i2c_bme280_handle);

    // Initialise graphics
    main_gfx_start();    

    // Start display update timer
    px_systmr_start(&tmr, PX_SYSTMR_MS_TO_TICKS(1000));
    // Loop forever
    while(true)
    {
        // Update display?
        if(px_systmr_has_expired(&tmr))
        {
            px_bme280_data_t data;            
            
            // Restart update timer
            px_systmr_reset(&tmr);
            // Get new BME280 data
            if(px_bme280_read(&data))
            {
                // Convert data to values
                int32_t  temp  = px_bme280_temperature(&data);
                uint32_t press = px_bme280_pressure(&data);
                uint32_t hum   = px_bme280_humidity(&data);
                // Update values
                main_gfx_update_temp(temp);
                main_gfx_update_press(press);
                main_gfx_update_hum(hum);
                // Draw window
                px_gfx_obj_draw(obj_win);
                // Update display
                px_gfx_draw_update();
            }
        }
        // Put core into SLEEP mode until an interrupt occurs
        __WFI();
    }
}

void main_log_putchar(char data)
{
    // New line character?
    if(data == '\n')
    {
        // Prepend a carriage return
        main_log_putchar('\r');
    }
    px_uart_putchar(&px_uart1_handle, data);
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
