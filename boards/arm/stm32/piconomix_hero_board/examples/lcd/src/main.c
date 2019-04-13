/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2017 Pieter Conradie <https://piconomix.com>
 
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
                 PX_SPI_PER_2,
                 PX_BOARD_SPI2_CS_LCD,
                 px_spi_util_baud_hz_to_clk_div(PX_LCD_MAX_SPI_CLOCK_HZ),
                 PX_LCD_SPI_MODE, 
                 PX_LCD_SPI_DATA_ORDER,
                 0x00);
    px_lcd_init(&px_spi_lcd_handle);

    // Initialise graphics
    px_gfx_init();

    // Draw pixels
    px_gfx_draw_pixel(PX_GFX_DISP_SIZE_X / 2, PX_GFX_DISP_SIZE_Y / 2 - 5, PX_GFX_COLOR_ON);
    px_gfx_draw_pixel(PX_GFX_DISP_SIZE_X / 2, PX_GFX_DISP_SIZE_Y / 2 + 5, PX_GFX_COLOR_ON);

    // Draw lines
    px_gfx_draw_line(0, 0, PX_GFX_DISP_SIZE_X - 1, PX_GFX_DISP_SIZE_Y - 1, PX_GFX_COLOR_ON);
    px_gfx_draw_line(0, PX_GFX_DISP_SIZE_Y - 1, PX_GFX_DISP_SIZE_X - 1, 0, PX_GFX_COLOR_ON);

    // Draw rectangle
    px_gfx_draw_rect(0, 0, PX_GFX_DISP_SIZE_X, PX_GFX_DISP_SIZE_Y, PX_GFX_COLOR_ON);

    // Draw circle
    px_gfx_draw_circ(PX_GFX_DISP_SIZE_X / 2, PX_GFX_DISP_SIZE_Y / 2, 20, PX_GFX_COLOR_ON);

    // Draw text
    px_gfx_draw_str(&px_gfx_font_5x7, 30, 2, PX_GFX_COLOR_ON, "Hello World!");

    // Draw image
    px_gfx_draw_img(PX_GFX_DISP_SIZE_X / 2 + 30, 
                    PX_GFX_DISP_SIZE_Y / 2 - 15, 
                    px_gfx_img_dino_25x30.width, 
                    px_gfx_img_dino_25x30.height,
                    PX_GFX_COLOR_ON,
                    px_gfx_img_dino_25x30.data);

    // Update display
    px_gfx_update();

    // Block forever
    for(;;)
    {
        ;
    }
}
