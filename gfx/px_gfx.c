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
#include "px_gfx_lcd.h"
#include "px_dbg.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_DBG_DECL_NAME("px_gfx");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static px_gfx_area_t px_gfx_update_area;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void px_gfx_update_area_reset(void)
{
    px_gfx_update_area.x1 = PX_GFX_X_MAX + 1;
    px_gfx_update_area.x2 = PX_GFX_X_MIN - 1;
    px_gfx_update_area.y1 = PX_GFX_Y_MAX + 1;
    px_gfx_update_area.y2 = PX_GFX_Y_MIN - 1;
}

static bool px_gfx_update_area_is_set(void)
{
    if(px_gfx_update_area.x1 <= PX_GFX_X_MAX)
    {
        return true;
    }
    else
    {
        return false;
    }
}

static void px_gfx_invalidate_area(px_gfx_xy_t x1,
                                   px_gfx_xy_t y1,
                                   px_gfx_xy_t x2,
                                   px_gfx_xy_t y2)
{
    // Swap coordinates if required
    if(x1 > x2) PX_SWAP(px_gfx_xy_t, x1, x2);
    if(y1 > y2) PX_SWAP(px_gfx_xy_t, y1, y2);

    // Outside display area?
    if((x1 < PX_GFX_X_MIN) && (x2 < PX_GFX_X_MIN)) return;
    if((x1 > PX_GFX_X_MAX) && (x2 > PX_GFX_X_MAX)) return;
    if((y1 < PX_GFX_Y_MIN) && (y2 < PX_GFX_Y_MIN)) return;
    if((y1 > PX_GFX_Y_MAX) && (y2 > PX_GFX_Y_MAX)) return;

    // Clip to display if required
    if(x1 < PX_GFX_X_MIN) x1 = PX_GFX_X_MIN;
    if(x2 > PX_GFX_X_MAX) x2 = PX_GFX_X_MAX;
    if(y1 < PX_GFX_Y_MIN) y1 = PX_GFX_Y_MIN;
    if(y2 > PX_GFX_Y_MAX) y2 = PX_GFX_Y_MAX;

    // Update area
    if(px_gfx_update_area.x1 > x1)  px_gfx_update_area.x1 = x1;
    if(px_gfx_update_area.x2 < x2)  px_gfx_update_area.x2 = x2;
    if(px_gfx_update_area.y1 > y1)  px_gfx_update_area.y1 = y1;
    if(px_gfx_update_area.y2 < y2)  px_gfx_update_area.y2 = y2;
}

static void px_gfx_draw_pixel_clipped(px_gfx_xy_t    x,
                                      px_gfx_xy_t    y,
                                      px_gfx_color_t color)
{
    // X and Y coordinate inside display area?
    if(  (x >= PX_GFX_X_MIN) 
       &&(x <= PX_GFX_X_MAX)
       &&(y >= PX_GFX_Y_MIN)
       &&(y <= PX_GFX_Y_MAX)  )
    {
        px_gfx_lcd_draw_pixel(x, y, color);
    }
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_gfx_init(void)
{
    px_gfx_clear();
}

void px_gfx_clear(void)
{
    px_gfx_update_area_reset();
    px_gfx_invalidate_area(0, 0, PX_GFX_X_MAX, PX_GFX_DISP_SIZE_Y);
    px_gfx_lcd_clear();
}

void px_gfx_draw_screen(void)
{
    px_gfx_invalidate_area(0, 0, PX_GFX_X_MAX, PX_GFX_DISP_SIZE_Y);
    px_gfx_lcd_update(&px_gfx_update_area);
    px_gfx_update_area_reset();
}

void px_gfx_update(void)
{
    if(px_gfx_update_area_is_set())
    {
        px_gfx_lcd_update(&px_gfx_update_area);
        px_gfx_update_area_reset();
    }
}

bool px_gfx_update_area_get(px_gfx_area_t * area)
{
    memcpy(area, &px_gfx_update_area, sizeof(px_gfx_area_t));

    return px_gfx_update_area_is_set();
}

void px_gfx_draw_pixel(px_gfx_xy_t    x,
                       px_gfx_xy_t    y,
                       px_gfx_color_t color)
{
    px_gfx_invalidate_area   (x, y, x, y);
    px_gfx_draw_pixel_clipped(x, y, color);
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
        px_gfx_xy_t  x, y, dx, dy, err;
        bool         ystep, swap_xy;

        px_gfx_invalidate_area(x1, y1, x2, y2);
        
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
                px_gfx_draw_pixel_clipped(x, y, color);
            }
            else
            {
                // No
                px_gfx_draw_pixel_clipped(x, y, color);
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

    px_gfx_invalidate_area(x, y, x + width - 1, y);

    for(i=0; i < width; i++)
    {
        px_gfx_draw_pixel_clipped(x, y, color);
        x++;
    }
}

void px_gfx_draw_line_ver(px_gfx_xy_t    x,
                          px_gfx_xy_t    y,
                          px_gfx_xy_t    height,
                          px_gfx_color_t color)
{
    px_gfx_xy_t j;

    px_gfx_invalidate_area(x, y, x, y + height - 1);

    for(j=0; j < height; j++)
    {
        px_gfx_draw_pixel_clipped(x, y, color);
        y++;
    }
}

void px_gfx_draw_rect(px_gfx_xy_t    x,
                      px_gfx_xy_t    y,
                      px_gfx_xy_t    width,
                      px_gfx_xy_t    height,
                      px_gfx_color_t color)
{
    px_gfx_invalidate_area(x, y, x + width - 1, y + height - 1);

    // Top
    px_gfx_draw_line_hor(x, y, width, color);
    // Bottom
    px_gfx_draw_line_hor(x, y + height - 1, width, color);
    // Left
    px_gfx_draw_line_ver(x, y, height, color);
    // Right
    px_gfx_draw_line_ver(x + width - 1, y, height, color);
}

void px_gfx_draw_fill(px_gfx_xy_t    x,
                      px_gfx_xy_t    y,
                      px_gfx_xy_t    width,
                      px_gfx_xy_t    height,
                      px_gfx_color_t color)
{
    px_gfx_xy_t j;

    px_gfx_invalidate_area(x, y, x + width - 1, y + height - 1);

    for(j=0; j < height; j++)
    {
        px_gfx_draw_line_hor(x, y, width, color);
        y++;
    }    
}

void px_gfx_draw_circ(px_gfx_xy_t    x,
                      px_gfx_xy_t    y,
                      px_gfx_xy_t    radius,
                      px_gfx_color_t color)
{
    // https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
    // https://rosettacode.org/wiki/Bitmap/Midpoint_circle_algorithm#C

    px_gfx_xy_t f     = 1 - radius;
    px_gfx_xy_t ddf_x = 1;
    px_gfx_xy_t ddf_y = -2 * radius;
    px_gfx_xy_t dx    = 0;
    px_gfx_xy_t dy    = radius;

    px_gfx_invalidate_area(x - radius + 1, y - radius + 1,
                           x + radius - 1, y + radius - 1);

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
        px_gfx_draw_pixel_clipped(x + dx, y + dy, color);
        px_gfx_draw_pixel_clipped(x - dx, y + dy, color);
        px_gfx_draw_pixel_clipped(x + dx, y - dy, color);
        px_gfx_draw_pixel_clipped(x - dx, y - dy, color);
        px_gfx_draw_pixel_clipped(x + dy, y + dx, color);
        px_gfx_draw_pixel_clipped(x - dy, y + dx, color);
        px_gfx_draw_pixel_clipped(x + dy, y - dx, color);
        px_gfx_draw_pixel_clipped(x - dy, y - dx, color);
    }
}

void px_gfx_draw_img(const px_gfx_img_t * img,
                     px_gfx_xy_t          x,
                     px_gfx_xy_t          y,
                     px_gfx_align_t       align,
                     px_gfx_color_t       color)
{
    px_gfx_xy_t     i;
    px_gfx_xy_t     j;
    uint8_t         mask;
	const uint8_t * data = img->data;

    // Set alignment
    if(align & PX_GFX_ALIGN_H_MID)
    {
        x -= (img->width + 1) / 2 - 1;
    }
    if(align & PX_GFX_ALIGN_H_RIGHT)
    {
        x -= img->width;
    }
    if(align & PX_GFX_ALIGN_V_MID)
    {
        y -= img->height / 2 - 1;
    }
    if(align & PX_GFX_ALIGN_V_BOT)
    {
        y -= img->height - 1;
    }

    px_gfx_invalidate_area(x, y, x + img->width - 1, y + img->height - 1);
   
    // Repeat for each row
    for(j = 0; j < img->height; j++)
    {
        // Start at most significant bit
        mask = 0x80;
        // Repeat for each pixel in row
        for(i = 0; i < img->width; i++)
        {
            // Must pixel be drawn?
            if(*data & mask)
            {
                // Yes
                px_gfx_draw_pixel_clipped(x + i, y + j, color);
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

void px_gfx_draw_char(const px_gfx_font_t * font,
                      px_gfx_xy_t           x,
                      px_gfx_xy_t           y,
                      px_gfx_align_t        align,
                      px_gfx_color_t        color,
                      char                  glyph)
{
    px_gfx_img_t    img;
    const uint8_t * data = font->data;
	int             width_bytes = (font->width + 7) / 8;

    // Find glyph
    while(*data != 0x00)
    {
        // Match?
        if(*data == glyph)
        {
            break;
        }
        // Next glyph
        data += (width_bytes * font->height) + 1;
    }

    // Not found?
    if(*data == 0x00)
    {
        return;
    }

    // Advance to start of glyph data
    data++;

    // Set image parameters
    img.width  = font->width;
    img.height = font->height;
    img.data   = data;

    // Draw glyph
    px_gfx_draw_img(&img, x, y, align, color);
}

void px_gfx_draw_str(const px_gfx_font_t * font,
                     px_gfx_xy_t           x,
                     px_gfx_xy_t           y,
                     px_gfx_align_t        align,
                     px_gfx_color_t        color,
                     const char *          str)
{
    // Set alignment
    if(align & PX_GFX_ALIGN_V_MID)
    {
        y -= font->height / 2 - 1;
    }
    if(align & PX_GFX_ALIGN_V_BOT)
    {
        y -= font->height - 1;
    }
    if(align & PX_GFX_ALIGN_H_MID)
    {
        x -= (font->width + 1) * (px_gfx_xy_t)strlen(str) / 2 - 1;
    }
    if(align & PX_GFX_ALIGN_H_RIGHT)
    {
        x -= (font->width + 1) * (px_gfx_xy_t)strlen(str) - 1;
    }

    // Draw each glyph
    while(*str != '\0')
    {
        px_gfx_draw_char(font, x, y, PX_GFX_ALIGN_TOP_LEFT, color, *str++);
        x += font->width + 1;
    }
}

void px_gfx_printf(const px_gfx_font_t * font,
                   px_gfx_xy_t           x,
                   px_gfx_xy_t           y,
                   px_gfx_align_t        align,
                   px_gfx_color_t        color,
                   const char *          format, ...)
{
    va_list args;
    char str[PX_GFX_CFG_STR_BUFFER_SIZE];

    // Output user formatted string
    va_start(args, format);
    vsnprintf(str, PX_GFX_CFG_STR_BUFFER_SIZE, format, args);
    va_end(args);

    // Append terminating zero in case of buffer overrun
    str[PX_GFX_CFG_STR_BUFFER_SIZE-1] = '\0';

    px_gfx_draw_str(font, x, y, align, color, str);
}

