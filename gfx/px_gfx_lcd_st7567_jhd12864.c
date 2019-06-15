/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
 
    Title:          px_gfx_lcd.h : Glue layer to physical display
    Author(s):      Pieter Conradie
    Creation Date:  2019-05-28

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_gfx_lcd.h"
#include "px_lcd_st7567_jhd12864.h"
#include "px_dbg.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_DBG_DECL_NAME("px_gfx_lcd_st7567_jhd12864");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
/// Allocate space for frame buffer [row(y)][col(x)]
static uint8_t px_gfx_lcd_frame_buf[PX_GFX_DISP_SIZE_Y / 8][PX_GFX_DISP_SIZE_X];

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_gfx_lcd_clear_frame(void)
{
    // Clear frame buffer
    memset(px_gfx_lcd_frame_buf, 0, sizeof(px_gfx_lcd_frame_buf));
}

void px_gfx_lcd_draw_pixel(px_gfx_xy_t    x,
                           px_gfx_xy_t    y,
                           px_gfx_color_t color)
{
    uint8_t * frame;

    // Calculate address in frame buffer
    frame = &px_gfx_lcd_frame_buf[y / 8][x];

    switch(color)
    {
    case PX_GFX_COLOR_ON:
        *frame |= (0x01 << (y % 8));
        break;
    case PX_GFX_COLOR_OFF:
        *frame &= ~(0x01 << (y % 8));
        break;
    case PX_GFX_COLOR_INVERT:
        *frame ^= (0x01 << (y % 8));
        break;
    default:
        break;
    }
}

void px_gfx_lcd_update(const px_gfx_area_t * area)
{
    uint8_t page;

    // Update area of LCD
    for(page = area->y1 / 8; page <= (area->y2 / 8); page++)
    {
        px_lcd_sel_page(page);
        px_lcd_sel_col(area->x1);
        px_lcd_wr_disp_data(&px_gfx_lcd_frame_buf[page][area->x1], area->x2 - area->x1 + 1);
    }
}

void px_gfx_lcd_dbg_frame_buf_report(void)
{
    px_gfx_xy_t x, y;

    for(y=0; y<PX_GFX_DISP_SIZE_Y; y++)
    {
        for(x=0; x<PX_GFX_DISP_SIZE_X; x++)
        {
            if(((px_gfx_lcd_frame_buf[y / 8][x]) & (1 << (y % 8))) != 0)
            {
                PX_DBG_TRACE("1");
            }
            else
            {
                PX_DBG_TRACE("0");
            }
        }
        PX_DBG_TRACE("\n");
    }
}


