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
#include "px_defines.h"

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_board.h"
#include "px_spi.h"
#include "px_lcd_st7567_jhd12864.h"
#include "px_gfx.h"
#include "px_gfx_res.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static px_spi_handle_t px_spi_lcd_handle;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____PUBLIC FUNCTIONS_____________________________________________________ */
int main(void)
{
    // Initialize board
    px_board_init();

    // Initialise LCD driver
    px_spi_init();
    px_spi_open2(&px_spi_lcd_handle,
                 PX_SPI_NR_2,
                 PX_BOARD_SPI2_CS_LCD,
                 px_spi_util_baud_hz_to_clk_div(PX_LCD_MAX_SPI_CLOCK_HZ),
                 PX_LCD_SPI_MODE, 
                 PX_LCD_SPI_DATA_ORDER,
                 0x00);
    px_lcd_init(&px_spi_lcd_handle);
    PX_LCD_BACKLIGHT_ON();

    // Initialise graphics
    px_gfx_init();

    // Draw pixels
    px_gfx_draw_pixel(PX_GFX_X_MID, PX_GFX_Y_MID - 5);
    px_gfx_draw_pixel(PX_GFX_X_MID, PX_GFX_Y_MID + 5);
    // Draw lines
    px_gfx_draw_line(0, 0, PX_GFX_X_MAX, PX_GFX_Y_MAX);
    px_gfx_draw_line(0, PX_GFX_Y_MAX, PX_GFX_X_MAX, 0);
    // Draw rectangle
    px_gfx_draw_rect(0, 0, PX_GFX_DISP_SIZE_X, PX_GFX_DISP_SIZE_Y);
    // Draw circle
    px_gfx_draw_circ(PX_GFX_X_MID, PX_GFX_Y_MID, 20);
    // Draw text
    px_gfx_draw_str(30, 2, "Hello World!");
    // Draw image
    px_gfx_draw_img(PX_GFX_X_MID + 30, PX_GFX_Y_MID - 15, &px_gfx_img_dino);

    // Update display
    px_gfx_draw_update();

    // Block forever
    while(true) {;}
}
