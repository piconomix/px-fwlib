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
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("px_gfx_disp_sim");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
/// Allocate space for frame buffer [row(y)][col(x)]
uint8_t px_gfx_frame_buf[PX_GFX_DISP_SIZE_Y][PX_GFX_DISP_SIZE_X];

extern void px_gfx_disp_sim_draw(const px_gfx_area_t * area);

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_gfx_disp_buf_clear(void)
{
    // Clear display buffer
    memset(px_gfx_frame_buf, 0, sizeof(px_gfx_frame_buf));
}

void px_gfx_disp_buf_pixel(px_gfx_xy_t    x,
                               px_gfx_xy_t    y,
                               px_gfx_color_t color)
{
    uint8_t * data;

    // Calculate address in display buffer
    data = &px_gfx_frame_buf[y][x];

    switch(color)
    {
    case PX_GFX_COLOR_ON:
        *data = 1;
        break;
    case PX_GFX_COLOR_OFF:
        *data = 0;
        break;
    case PX_GFX_COLOR_INVERT:
        *data ^= 1;
        break;
    default:
        break;
    }
}

void px_gfx_disp_update(const px_gfx_area_t * area)
{
    px_gfx_disp_sim_draw(area);
}

void px_gfx_disp_dbg_report_buf(void)
{
    px_gfx_xy_t x, y;

    for(y=0; y<PX_GFX_DISP_SIZE_Y; y++)
    {
        for(x=0; x<PX_GFX_DISP_SIZE_X; x++)
        {
            if(px_gfx_frame_buf[y][x])
            {
                putchar('1');
            }
            else
            {
                putchar('0');
            }
        }
        putchar('\n');
    }
}

