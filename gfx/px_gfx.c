/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
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
 
    Title:          px_gfx.h : Basic monochrome graphics library
    Author(s):      Pieter Conradie
    Creation Date:  2018-05-07

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <string.h>
#include <stdarg.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_gfx.h"
#include "px_lcd_st7567_jhd12864.h"

#include "px_dbg.h"
PX_DBG_DECL_NAME("px_gfx")

/* _____LOCAL DEFINITIONS____________________________________________________ */
/// Size definition internal coordinate calculations
typedef int16_t px_gfx_calc_t;

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
/// Allocate space for frame buffer [row(y)][col(x)]
static uint8_t px_gfx_frame_buf[PX_GFX_DISP_SIZE_Y / 8][PX_GFX_DISP_SIZE_X];

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_gfx_init(void)
{
    px_gfx_clr_scr();
}

void px_gfx_update(void)
{
    px_gfx_xy_t y;

    // Send content of frame buffer to LCD
    for(y = 0; y < (PX_GFX_DISP_SIZE_Y / 8); y++)
    {
        px_lcd_sel_page(y);
        px_lcd_sel_col(0);
        px_lcd_wr_disp_data(px_gfx_frame_buf[y], PX_GFX_DISP_SIZE_X);
    }
}

void px_gfx_clr_scr(void)
{
    memset(px_gfx_frame_buf, 0, sizeof(px_gfx_frame_buf));
}

void px_gfx_draw_pixel(px_gfx_xy_t    x,
                       px_gfx_xy_t    y,
                       px_gfx_color_t color)
{
    uint8_t * frame = &px_gfx_frame_buf[y / 8][x];

    // X or Y coordinate outside display area?
    if((x >= PX_GFX_DISP_SIZE_X) || (y >= PX_GFX_DISP_SIZE_Y))
    {
        // Clip
        return;
    }

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

void px_gfx_draw_line(px_gfx_xy_t    x1,
                      px_gfx_xy_t    y1,
                      px_gfx_xy_t    x2,
                      px_gfx_xy_t    y2,
                      px_gfx_color_t color)
{
    // Vertical line?
    if(x1 == x2)
    {
        // Must y1 and y2 be swapped?
        if(y2 < y1)
        {
            // Yes
            PX_SWAP(px_gfx_xy_t, y1, y2);
        }
        px_gfx_draw_line_ver(x1, y1, y2 - y1 + 1, color);
    }
    // Horisontal line?
    else if(y1 == y2)
    {
        // Must x1 and x2 be swapped?
        if(x2 < x1)
        {
            // Yes
            PX_SWAP(px_gfx_xy_t, x1, x2);
        }
        px_gfx_draw_line_hor(x1, y1, x2 - x1 + 1, color);
    }
    else
    {
        // http://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
        px_gfx_xy_t     x, y, dx, dy;
        px_gfx_calc_t   err;
        bool            ystep, swap_xy;
        
        // Calculate absolute dx
        if(x2 > x1)
        {
            dx = x2 - x1;
        }
        else
        {
            dx = x1 - x2;
        }
        // Calculate absolute dy
        if(y2 > y1)
        {
            dy = y2 - y1;
        }
        else
        {
            dy = y1 - y2;
        }

        // Is rate of change dy greater than rate of change dx?
        if(dy > dx)
        {
            // x & y must be swapped
            swap_xy = true;
            PX_SWAP(px_gfx_xy_t, dx, dy);
            PX_SWAP(px_gfx_xy_t, x1, y1);
            PX_SWAP(px_gfx_xy_t, x2, y2);
        }
        else
        {
            swap_xy = false;
        }

        // Must [x1,y1] & [x2,y2] be swapped?
        if(x1 > x2)
        {
            // Yes
            PX_SWAP(px_gfx_xy_t, x1, x2);
            PX_SWAP(px_gfx_xy_t, y1, y2);
        }

        // Start error with dx / 2
        err = dx >> 1;

        // Positive slope?
        if ( y2 > y1 )
        {
            // Yes. Change of y in positive direction
            ystep = true;
        }
        else
        {
            // No. Change of y in negative direction
            ystep = false;
        }

        // Start at [x1,y1] and finish at [x2,...]
        y = y1;
        for(x = x1; x <= x2; x++)
        {
            // Is x & y swapped?
            if(swap_xy)
            {
                // Yes
                px_gfx_draw_pixel(y, x, color);
            }
            else
            {
                // No
                px_gfx_draw_pixel(x, y, color);
            }
            // Acumulate error for next point
            err -= dy;
            // Error negative?
            if (err < 0)
            {
                // Step y
                if(ystep)
                {
                    // In positive direction
                    y++;
                }
                else
                {
                    // In negative direction
                    y--;
                }
                // Add dx
                err += dx;
            }
        }
    }
}

void px_gfx_draw_line_hor(px_gfx_xy_t    x,
                          px_gfx_xy_t    y,
                          px_gfx_xy_t    width,
                          px_gfx_color_t color)
{
    px_gfx_xy_t i;

    for(i=0; i < width; i++)
    {
        px_gfx_draw_pixel(x + i, y, color);
    }
}

void px_gfx_draw_line_ver(px_gfx_xy_t    x,
                          px_gfx_xy_t    y,
                          px_gfx_xy_t    height,
                          px_gfx_color_t color)
{
    px_gfx_xy_t j;

    for(j=0; j < height; j++)
    {
        px_gfx_draw_pixel(x, y + j, color);
    }
}

void px_gfx_draw_rect(px_gfx_xy_t    x,
                      px_gfx_xy_t    y,
                      px_gfx_xy_t    width,
                      px_gfx_xy_t    height,
                      px_gfx_color_t color)
{
    // Top
    px_gfx_draw_line_hor(x,             y,              width, color);
    // Bottom
    px_gfx_draw_line_hor(x,             y + height - 1, width, color);
    // Left
    px_gfx_draw_line_ver(x,             y,              height, color);
    // Right
    px_gfx_draw_line_ver(x + width - 1, y,              height, color);
}

void px_gfx_draw_fill(px_gfx_xy_t    x,
                      px_gfx_xy_t    y,
                      px_gfx_xy_t    width,
                      px_gfx_xy_t    height,
                      px_gfx_color_t color)
{
    px_gfx_xy_t j;

    for(j=0; j < height; j++)
    {
        px_gfx_draw_line_hor(x, y + j, width, color);
    }    
}

void px_gfx_draw_circ(px_gfx_xy_t    x,
                        px_gfx_xy_t    y,
                        px_gfx_xy_t    radius,
                        px_gfx_color_t color)
{
    // https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
    // https://rosettacode.org/wiki/Bitmap/Midpoint_circle_algorithm#C

    px_gfx_calc_t f     = 1 - radius;
    px_gfx_calc_t ddf_x = 1;
    px_gfx_calc_t ddf_y = -2 * radius;
    px_gfx_calc_t dx    = 0;
    px_gfx_calc_t dy    = radius;

    // 0 deg (12 o'clock)
    px_gfx_draw_pixel(x,            y + radius, color);
    // 90 deg (3 o'clock)
    px_gfx_draw_pixel(x + radius,   y,          color);
    // 180 deg (6 o'clock)
    px_gfx_draw_pixel(x,            y - radius, color);
    // 270 deg (9 o'clock)
    px_gfx_draw_pixel(x - radius,   y,          color);

    // Repeat for each pixel in octant (45 deg pie)
    while(dx < dy)
    {
        // Must y pixel be stepped?
        if(f >= 0)
        {
            // Yes
            dy--;
            // Update decision
            ddf_y += 2;
            f     += ddf_y;
        }
        // Next pixel in x direction
        dx++;
        // Update decision
        ddf_x += 2;
        f     += ddf_x;

        // Draw pixel in each octant (45 deg pie)
        px_gfx_draw_pixel(x + dx, y + dy, color);
        px_gfx_draw_pixel(x - dx, y + dy, color);
        px_gfx_draw_pixel(x + dx, y - dy, color);
        px_gfx_draw_pixel(x - dx, y - dy, color);
        px_gfx_draw_pixel(x + dy, y + dx, color);
        px_gfx_draw_pixel(x - dy, y + dx, color);
        px_gfx_draw_pixel(x + dy, y - dx, color);
        px_gfx_draw_pixel(x - dy, y - dx, color);
    }
}

void px_gfx_draw_img(px_gfx_xy_t     x,
                     px_gfx_xy_t     y,
                     px_gfx_xy_t     width,
                     px_gfx_xy_t     height,
                     px_gfx_color_t  color,
                     const uint8_t * data)
{
    px_gfx_xy_t i;
    px_gfx_xy_t j;
    uint8_t     mask;
   
    // Repeat for each row
    for(j = 0; j < height; j++)
    {
        // Start at most significant bit
        mask = 0x80;
        // Repeat for each pixel in row
        for(i = 0; i < width; i++)
        {
            // Must pixel be drawn?
            if(*data & mask)
            {
                // Yes
                px_gfx_draw_pixel(x + i, y + j, color);
            }
            // Next bit
            mask >>= 1;
            // Finished with byte?
            if(mask == 0x00)
            {
                // Next byte
                data++;
                // Start again at most significant bit
                mask = 0x80;                
            }
        }
        // Ended on 8-bit boundary?
        if(mask != 0x80)
        {
            // No. Ignore rest of bits in byte
            data++;
        }
    }   
}

void px_gfx_draw_char(px_gfx_font_t * font,
                      px_gfx_xy_t     x,
                      px_gfx_xy_t     y,
                      px_gfx_color_t  color,
                      char            glyph)
{
    const uint8_t * data = font->data;

    // Find glyph
    while(*data != 0x00)
    {
        // Match?
        if(*data == glyph)
        {
            break;
        }
        // Next glyph
        data += font->height + 1;
    }

    // Not found?
    if(*data == 0x00)
    {
        return;
    }

    // Advance to start of glyph data
    data++;

    // Draw glyph
    px_gfx_draw_img(x, y, font->width, font->height, color, data);
}

void px_gfx_draw_str(px_gfx_font_t * font,
                     px_gfx_xy_t     x,
                     px_gfx_xy_t     y,
                     px_gfx_color_t  color,
                     const char *    str)
{
    while(*str != '\0')
    {
        px_gfx_draw_char(font, x, y, color, *str++);
        x += font->width + 1;
    }
}

void px_gfx_printf(px_gfx_font_t * font,
                   px_gfx_xy_t     x,
                   px_gfx_xy_t     y,
                   px_gfx_color_t  color,
                   const char *    format, ...)
{
    va_list args;
    char str[PX_GFX_CFG_STR_BUFFER_SIZE];

    // Output user formatted string
    va_start(args, format);
    vsnprintf(str, PX_GFX_CFG_STR_BUFFER_SIZE, format, args);
    va_end(args);

    // Append terminating zero in case of buffer overrun
    str[PX_GFX_CFG_STR_BUFFER_SIZE-1] = '\0';

    px_gfx_draw_str(font, x, y, color, str);
}
