/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2017 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          Piconomix STM32 Hero Board graphic LCD example
    Author(s):      Pieter Conradie
    Creation Date:  2017-11-13
 
============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include "px_defs.h"

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_board.h"
#include "px_spi.h"
#include "px_lcd_st7567_jhd12864.h"
#include "px_gfx.h"
#include "px_gfx_res.h"
#include "px_sysclk.h"
#include "px_systmr.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static px_spi_handle_t px_spi_lcd_handle;

static px_systmr_t tmr;
static px_gfx_xy_t ship_x;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____PUBLIC FUNCTIONS_____________________________________________________ */
static bool main_init(void)
{
    // Initialize modules
    px_board_init();
    px_sysclk_init();
    px_spi_init();
    // Initialise LCD driver
    px_spi_open2(&px_spi_lcd_handle,
                 PX_SPI_NR_2,
                 PX_BOARD_SPI2_CS_LCD,
                 px_spi_util_baud_hz_to_clk_div(PX_LCD_MAX_SPI_CLOCK_HZ),
                 PX_LCD_SPI_MODE, 
                 PX_LCD_SPI_DATA_ORDER,
                 0x00);
    px_lcd_init(&px_spi_lcd_handle);
    // Success
    return true;
}

int main(void)
{
    px_gfx_xy_t i;

    // Initialize drivers
    main_init();

    // Initialise graphics
    PX_LCD_BACKLIGHT_ON();
    px_gfx_init();

    // Start frame timer (20 frames per second)
    px_systmr_start(&tmr, PX_SYSTMR_MS_TO_TICKS(50));

    // Loop forever
    while(true)
    {
        // Left button pressed?
        if(px_gpio_in_is_lo(&px_gpio_lcd_btn_1_lt))
        {
            if(ship_x > 0)
            {
                ship_x--;   // Move ship left
            }
        }
        // Right button pressed?
        if(px_gpio_in_is_lo(&px_gpio_lcd_btn_2_rt))
        {
            if(ship_x < (PX_GFX_X_MAX - px_gfx_img_ship.width - 1))
            {
                ship_x++;   // Move ship right
            }
        }
        // Clear display buffer
        px_gfx_buf_clear();
        // Draw aliens
        for(i = 0; i < 8; i++)
        {
            px_gfx_draw_img(i * 8,  8,  &px_gfx_img_alien1);
            px_gfx_draw_img(i * 8,  16, &px_gfx_img_alien2);
            px_gfx_draw_img(i * 8,  24, &px_gfx_img_alien2);
            px_gfx_draw_img(i * 8,  32, &px_gfx_img_alien3);
            px_gfx_draw_img(i * 8,  40, &px_gfx_img_alien3);
        }
        // Draw bases
        px_gfx_draw_img(12, PX_GFX_Y_MAX - 10, &px_gfx_img_base);
        px_gfx_draw_img(31, PX_GFX_Y_MAX - 10, &px_gfx_img_base);
        px_gfx_draw_img(50, PX_GFX_Y_MAX - 10, &px_gfx_img_base);
        px_gfx_draw_img(69, PX_GFX_Y_MAX - 10, &px_gfx_img_base);
        // Draw ship
        px_gfx_draw_img(ship_x,  PX_GFX_Y_MAX - 5, &px_gfx_img_ship);
        // Wait until it is time to draw new frame
        while(!px_systmr_has_expired(&tmr)) {;}
        px_systmr_reset(&tmr);
        // Update display
        px_gfx_draw_update();
    }
}
