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
#include "px_at25s.h"
#include "px_systmr.h"
#include "px_rtc.h"
#include "px_board.h"
#include "px_uart.h"
#include "px_uart_stdio.h"
#include "px_spi.h"
#include "px_i2c.h"
#include "px_sysclk.h"
#include "px_debounce.h"
#include "px_sd.h"
#include "px_lcd_st7567_jhd12864.h"
#include "px_gfx.h"
#include "px_gfx_res.h"
#include "px_veml6075.h"
#include "px_bme280.h"
#include "px_gfx.h"
#include "px_gfx_objects.h"
#include "px_dbg.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_DBG_DECL_NAME("main");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
px_uart_handle_t px_uart1_handle;
px_spi_handle_t  px_spi_sf_handle;
px_spi_handle_t  px_spi_sd_handle;
px_spi_handle_t  px_spi_lcd_handle;
px_i2c_handle_t  px_i2c_veml6075_handle;
px_i2c_handle_t  px_i2c_bme280_handle;

uint8_t          main_buffer[MAIN_BUFFER_SIZE];

/* _____LOCAL VARIABLES______________________________________________________ */             
// Debounced buttons
static px_debounce_t px_debounce_pb1;
static px_debounce_t px_debounce_pb2;
static px_debounce_t px_debounce_pb3;
static px_debounce_t px_debounce_pb4;
static px_debounce_t px_debounce_pb5;
static px_debounce_t px_debounce_pb6;

static px_systmr_t tmr;

px_gfx_obj_handle_t obj_win;
px_gfx_obj_handle_t obj_graph;
px_gfx_xy_t         obj_graph_data_y[128];
px_gfx_obj_handle_t obj_label_uvi;
px_gfx_obj_handle_t obj_label_uvi_val;
uint16_t            obj_label_uvi_val_u16;
char                obj_label_uvi_val_str[16];

static const px_gfx_obj_graph_prop_t obj_graph_prop = 
{
    .x        = 0,
    .y        = 14,
    .width    = PX_GFX_DISP_SIZE_X,
    .height   = 50,
    .color_fg = PX_GFX_COLOR_ON,
    .color_bg = PX_GFX_COLOR_OFF,
    .data_y   = obj_graph_data_y,
};

static const px_gfx_obj_label_prop_t obj_label_prop_uvi = 
{
    .x        = 0,
    .y        = 13,
    .str      = "UVI",
    .font     = &px_gfx_font_5x7,
    .color_fg = PX_GFX_COLOR_ON,
    .color_bg = PX_GFX_COLOR_OFF,
    .align    = PX_GFX_ALIGN_BOT_LEFT,
};

static const px_gfx_obj_label_prop_t obj_label_prop_val = 
{
    .x        = 66,
    .y        = 13,
    .str      = obj_label_uvi_val_str,
    .font     = &px_gfx_font_11x14,
    .color_fg = PX_GFX_COLOR_ON,
    .color_bg = PX_GFX_COLOR_OFF,
    .align    = PX_GFX_ALIGN_BOT_RIGHT,
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
    // Open handle to UV sensor
    px_i2c_open(&px_i2c_veml6075_handle, PX_I2C_NR_1, PX_VEML6075_I2C_SLA_ADR);
    // Open handle to BME280 sensor
    px_i2c_open(&px_i2c_bme280_handle, PX_I2C_NR_1, PX_BME280_I2C_SLA_ADR);

    // Success
    return true;
}

static void main_gfx_update_uvi_val_str(void)
{
    uint16_t integer   = obj_label_uvi_val_u16 / 10;
    uint16_t remainder = obj_label_uvi_val_u16 % 10;

    if(integer < 10)
    {
        sprintf(obj_label_uvi_val_str, " %u.%u", integer, remainder);
    }
    else
    {
        sprintf(obj_label_uvi_val_str, "%u.%u", integer, remainder);
    }
    px_gfx_obj_update_set(obj_label_uvi_val);
}

static void main_gfx_update_graph_data(void)
{
    px_gfx_xy_t x;
    px_gfx_xy_t y = PX_UDIV_ROUND(obj_label_uvi_val_u16, 5);

    for(x = 0; x < (PX_SIZEOF_ARRAY(obj_graph_data_y) - 1); x++)
    {
        obj_graph_data_y[x] = obj_graph_data_y[x + 1];
    }
    obj_graph_data_y[x] = y;
    px_gfx_obj_update_set(obj_graph);
}

static void main_gfx_start(void)
{
    // Initialise graphics
    px_gfx_init();
    PX_LCD_BACKLIGHT_ON();
    // Create full display window
    obj_win = px_gfx_obj_win_create(&px_gfx_obj_win_prop_full_disp);
    // Create objects
    obj_graph         = px_gfx_obj_graph_create(&obj_graph_prop);
    memset(obj_graph_data_y, 0, sizeof(obj_graph_data_y));
    obj_label_uvi     = px_gfx_obj_label_create(&obj_label_prop_uvi);
    obj_label_uvi_val = px_gfx_obj_label_create(&obj_label_prop_val);
    // Add objects to window
    px_gfx_obj_win_add_child(obj_win, obj_graph);
    px_gfx_obj_win_add_child(obj_win, obj_label_uvi);
    px_gfx_obj_win_add_child(obj_win, obj_label_uvi_val);
}

/* _____PUBLIC FUNCTIONS_____________________________________________________ */
int main(void)
{
    // Initialize board and peripheral drivers
    main_init();
    // Report debug output
    PX_DBG_TRACE("Debug enabled\n");
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
    // Initialise and detect UV sensor
    px_veml6075_init(&px_i2c_veml6075_handle);
    // Initialise and detect BME280 sensor
    px_bme280_init(&px_i2c_bme280_handle);

    // Initialise graphics
    main_gfx_start();    

    // Start display update timer
    px_systmr_start(&tmr, PX_SYSTMR_MS_TO_TICKS(1000));
    for(;;)
    {
        // Update display?
        if(px_systmr_has_expired(&tmr))
        {
            px_bme280_data_t data;            
            
            // Restart update timer
            px_systmr_reset(&tmr);
            // Get new BME280 value
            if(px_bme280_read(&data))
            {
                int32_t  temp  = px_bme280_temperature(&data);
                uint32_t press = px_bme280_pressure(&data);
                uint32_t hum   = px_bme280_humidity(&data);

                // Update display
                px_gfx_buf_clear();
                px_gfx_printf(0,  0, "T: %lu deg C", temp / 100);
                px_gfx_printf(0, 10, "P: %lu Pa", press);
                px_gfx_printf(0, 20, "H: %lu %%", hum / 1024);
                px_gfx_draw_update();
            }
        }
        // Put core into SLEEP mode until an interrupt occurs
        __WFI();
    }
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
