/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2017 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          Piconomix STM32 Hero Board graphic animation example
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
#include "px_gfx_resources.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static px_spi_handle_t px_spi_lcd_handle;

static const px_gfx_img_t * img_array[] =
{
    &px_gfx_img_clown1,
    &px_gfx_img_clown2,
    &px_gfx_img_clown3,
    &px_gfx_img_clown4,
    &px_gfx_img_clown5,
    &px_gfx_img_clown6,
    &px_gfx_img_clown7,
    &px_gfx_img_clown8,
    NULL,
};

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____PUBLIC FUNCTIONS_____________________________________________________ */
int main(void)
{
    uint8_t i;
    const px_gfx_img_t * img;

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

    // Enable LCD backlight
    PX_LCD_BACKLIGHT_ON();

    // Repeat forever
    i = 0;
    for(;;)
    {
        // Clear display area
        px_gfx_clear_frame();
        // Draw image
        img = img_array[i];
        px_gfx_align_set(PX_GFX_ALIGN_MID);
        px_gfx_draw_img(img,
                        PX_GFX_DISP_SIZE_X / 2,
                        PX_GFX_DISP_SIZE_Y / 2 - 1);
        // Update display
        px_gfx_update_frame();
        // Delay
        px_board_delay_ms(100);
        // Next image
        if(++i == 8)
        {
            // Start at first image
            i = 0;
        }
    }
}
