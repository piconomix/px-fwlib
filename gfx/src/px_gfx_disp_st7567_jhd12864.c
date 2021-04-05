/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
 
    Title:          px_gfx_disp.h : Glue layer to physical display
    Author(s):      Pieter Conradie
    Creation Date:  2019-05-28

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_gfx_disp.h"
#include "px_lcd_st7567_jhd12864.h"
#include "px_dbg.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_DBG_DECL_NAME("px_gfx_disp_st7567_jhd12864");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
/// Allocate space for frame buffer [row(y)][col(x)]
static uint8_t px_gfx_frame_buf[PX_GFX_DISP_SIZE_Y / 8][PX_GFX_DISP_SIZE_X];

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_gfx_disp_buf_clear(void)
{
    // Clear frame buffer
    memset(px_gfx_frame_buf, 0, sizeof(px_gfx_frame_buf));
}

void px_gfx_disp_buf_pixel(px_gfx_xy_t    x,
                           px_gfx_xy_t    y,
                           px_gfx_color_t color)
{
    uint8_t * data;

    // Calculate address in frame buffer
    data = &px_gfx_frame_buf[y / 8][x];

    switch(color)
    {
    case PX_GFX_COLOR_ON:
        *data |= (0x01 << (y % 8));
        break;
    case PX_GFX_COLOR_OFF:
        *data &= ~(0x01 << (y % 8));
        break;
    case PX_GFX_COLOR_INVERT:
        *data ^= (0x01 << (y % 8));
        break;
    default:
        break;
    }
}

void px_gfx_disp_update(const px_gfx_area_t * area)
{
    uint8_t page;

    // Update specified area of display
    for(page = area->y1 / 8; page <= (area->y2 / 8); page++)
    {
        px_lcd_sel_page(page);
        px_lcd_sel_col(area->x1);
        px_lcd_wr_disp_data(&px_gfx_frame_buf[page][area->x1], area->x2 - area->x1 + 1);
    }
}

void px_gfx_disp_dbg_report_buf(void)
{
    px_gfx_xy_t x, y;

    for(y = 0; y < PX_GFX_DISP_SIZE_Y; y++)
    {
        for(x = 0; x < PX_GFX_DISP_SIZE_X; x++)
        {
            if(((px_gfx_frame_buf[y / 8][x]) & (1 << (y % 8))) != 0)
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
