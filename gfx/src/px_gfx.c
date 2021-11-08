/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
 
    Title:          px_gfx.h : Basic monochrome graphics library
    Author(s):      Pieter Conradie
    Creation Date:  2018-05-07

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <string.h>
#include <stdarg.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_gfx.h"
#include "px_gfx_disp.h"
#include "px_gfx_fonts.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("px_gfx");

/// Graphic drawing properties
typedef struct
{
    px_gfx_color_t          color_fg;       ///< Foreground color
    px_gfx_color_t          color_bg;       ///< Background color
    px_gfx_align_t          align;          ///< Alignment
    bool                    vp_active;      ///< View port active flag
    px_gfx_view_port_t      vp;             ///< View port
    const px_gfx_font_t *   font;           ///< Current font
} px_gfx_draw_prop_t;

/// Graphics state definition
typedef struct
{
    px_gfx_draw_prop_t      draw_prop;
    px_gfx_area_t           update_area;    ///< Area of frame to be updated
} px_gfx_t;

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
/// Graphic state
static px_gfx_t px_gfx;

/// Default drawing attributes
static const px_gfx_draw_prop_t px_gfx_draw_prop_default =
{
    .color_fg   = PX_GFX_COLOR_ON,
    .color_bg   = PX_GFX_COLOR_OFF,
    .align      = PX_GFX_ALIGN_TOP_LEFT,
    .vp_active  = false,
    .vp =
    {
        .x      = 0,
        .y      = 0,
        .width  = PX_GFX_DISP_SIZE_X,
        .height = PX_GFX_DISP_SIZE_Y,
        .xy_ref = PX_GFX_XY_REF_REL,
    },
    .font       = &PX_GFX_CFG_DEFAULT_FONT,
};

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void px_gfx_update_area_reset(void)
{
    px_gfx.update_area.x1 = PX_GFX_X_MAX + 1;
    px_gfx.update_area.x2 = PX_GFX_X_MIN - 1;
    px_gfx.update_area.y1 = PX_GFX_Y_MAX + 1;
    px_gfx.update_area.y2 = PX_GFX_Y_MIN - 1;
}

static bool px_gfx_update_area_is_set(void)
{
    if(px_gfx.update_area.x1 <= PX_GFX_X_MAX)
    {
        return true;
    }
    else
    {
        return false;
    }
}

static void px_gfx_update_area(px_gfx_xy_t x1,
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
    if(px_gfx.update_area.x1 > x1)  px_gfx.update_area.x1 = x1;
    if(px_gfx.update_area.x2 < x2)  px_gfx.update_area.x2 = x2;
    if(px_gfx.update_area.y1 > y1)  px_gfx.update_area.y1 = y1;
    if(px_gfx.update_area.y2 < y2)  px_gfx.update_area.y2 = y2;
}

static px_gfx_xy_t px_gfx_vp_adjust_x(px_gfx_xy_t x)
{
    if(  (px_gfx.draw_prop.vp_active) && (px_gfx.draw_prop.vp.xy_ref == PX_GFX_XY_REF_REL)  )
    {
        return (x + px_gfx.draw_prop.vp.x);
    }
    else
    {
        return x;
    }
}

static px_gfx_xy_t px_gfx_vp_adjust_y(px_gfx_xy_t y)
{
    if(  (px_gfx.draw_prop.vp_active) && (px_gfx.draw_prop.vp.xy_ref == PX_GFX_XY_REF_REL)  )
    {
        return (y + px_gfx.draw_prop.vp.y);
    }
    else
    {
        return y;
    }
}

static void px_gfx_vp_draw_pixel(px_gfx_xy_t    x,
                                 px_gfx_xy_t    y,
                                 px_gfx_color_t color)
{
    // View port active?
    if(px_gfx.draw_prop.vp_active)
    {
        // X and Y coordinate inside viewport?
        if(  (x <  px_gfx.draw_prop.vp.x                             )
           ||(x >= px_gfx.draw_prop.vp.x + px_gfx.draw_prop.vp.width )
           ||(y <  px_gfx.draw_prop.vp.y                             )
           ||(y >= px_gfx.draw_prop.vp.y + px_gfx.draw_prop.vp.height)  )
        {
            // No
            return;
        }
    }
    // X and Y coordinate inside display area?
    if(  (x < PX_GFX_X_MIN) 
       ||(x > PX_GFX_X_MAX)
       ||(y < PX_GFX_Y_MIN)
       ||(y > PX_GFX_Y_MAX)  )
    {
        // No
        return;
    }
    // Draw pixel
    px_gfx_disp_buf_pixel(x, y, color);
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_gfx_init(void)
{
	px_gfx_draw_prop_reset();
    px_gfx_buf_clear();
}

void px_gfx_buf_clear(void)
{
    px_gfx_update_area_reset();
    px_gfx_update_area(0, 0, PX_GFX_X_MAX, PX_GFX_DISP_SIZE_Y);
    px_gfx_disp_buf_clear();
}

void px_gfx_draw(void)
{
    px_gfx_update_area(0, 0, PX_GFX_X_MAX, PX_GFX_DISP_SIZE_Y);
    px_gfx_disp_update(&px_gfx.update_area);
    px_gfx_update_area_reset();
}

void px_gfx_draw_update(void)
{
    if(px_gfx_update_area_is_set())
    {
        px_gfx_disp_update(&px_gfx.update_area);
        px_gfx_update_area_reset();
    }
}

bool px_gfx_update_area_get(px_gfx_area_t * area)
{
    memcpy(area, &px_gfx.update_area, sizeof(px_gfx_area_t));

    return px_gfx_update_area_is_set();
}

void px_gfx_draw_prop_reset(void)
{
    memcpy(&px_gfx.draw_prop, &px_gfx_draw_prop_default, sizeof(px_gfx.draw_prop));
}

px_gfx_color_t px_gfx_color_fg_set(px_gfx_color_t color)
{
    px_gfx_color_t old        = px_gfx.draw_prop.color_fg;
    px_gfx.draw_prop.color_fg = color;
    return old;
}

px_gfx_color_t px_gfx_color_bg_set(px_gfx_color_t color)
{
    px_gfx_color_t old        = px_gfx.draw_prop.color_bg;
    px_gfx.draw_prop.color_bg = color;
    return old;
}

px_gfx_align_t px_gfx_align_set(px_gfx_align_t align)
{
    px_gfx_align_t old      = px_gfx.draw_prop.align;
    px_gfx.draw_prop.align  = align;
    return old;
}

void px_gfx_view_port_set(px_gfx_xy_t     x,
                          px_gfx_xy_t     y,
                          px_gfx_xy_t     width,
                          px_gfx_xy_t     height,
                          px_gfx_xy_ref_t xy_ref)
{
    px_gfx.draw_prop.vp.x      = x;
    px_gfx.draw_prop.vp.y      = y;
    px_gfx.draw_prop.vp.width  = width;
    px_gfx.draw_prop.vp.height = height;
    px_gfx.draw_prop.vp.xy_ref = xy_ref;
    px_gfx.draw_prop.vp_active = true;
}

void px_gfx_view_port_reset(void)
{
    memcpy(&px_gfx.draw_prop.vp, &px_gfx_draw_prop_default.vp, sizeof(px_gfx.draw_prop.vp));
	px_gfx.draw_prop.vp_active = false;
}

const px_gfx_font_t * px_gfx_font_set(const px_gfx_font_t * font)
{
    const px_gfx_font_t * old = px_gfx.draw_prop.font;
    px_gfx.draw_prop.font     = font;
    return old;
}

void px_gfx_draw_pixel(px_gfx_xy_t    x,
                       px_gfx_xy_t    y)
{
    // Adjust coordinates relative to viewport
    x = px_gfx_vp_adjust_x(x);
    y = px_gfx_vp_adjust_y(y);
    // Update dirty area
    px_gfx_update_area(x, y, x, y);
    // Draw pixel
    px_gfx_vp_draw_pixel(x, y, px_gfx.draw_prop.color_fg);
}

void px_gfx_draw_line(px_gfx_xy_t    x1,
                      px_gfx_xy_t    y1,
                      px_gfx_xy_t    x2,
                      px_gfx_xy_t    y2)
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
        px_gfx_draw_line_ver(x1, y1, y2 - y1 + 1);
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
        px_gfx_draw_line_hor(x1, y1, x2 - x1 + 1);
    }
    else
    {
        // http://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
        px_gfx_xy_t  x, y, dx, dy, err;
        bool         ystep, swap_xy;

        // Adjust coordinates relative to viewport
        x1 = px_gfx_vp_adjust_x(x1);
        y1 = px_gfx_vp_adjust_y(y1);
        x2 = px_gfx_vp_adjust_x(x2);
        y2 = px_gfx_vp_adjust_y(y2);
        // Update dirty area
        px_gfx_update_area(x1, y1, x2, y2);
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
                px_gfx_vp_draw_pixel(y, x, px_gfx.draw_prop.color_fg);
            }
            else
            {
                // No
                px_gfx_vp_draw_pixel(x, y, px_gfx.draw_prop.color_fg);
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
                          px_gfx_xy_t    width)
{
    px_gfx_xy_t i;

    // Adjust coordinates relative to viewport
    x = px_gfx_vp_adjust_x(x);
    y = px_gfx_vp_adjust_y(y);
    // Update dirty area
    px_gfx_update_area(x, y, x + width - 1, y);
    // Draw horizontal line
    for(i = 0; i < width; i++)
    {
        px_gfx_vp_draw_pixel(x, y, px_gfx.draw_prop.color_fg);
        x++;
    }
}

void px_gfx_draw_line_ver(px_gfx_xy_t    x,
                          px_gfx_xy_t    y,
                          px_gfx_xy_t    height)
{
    px_gfx_xy_t j;

    // Adjust coordinates relative to viewport
    x = px_gfx_vp_adjust_x(x);
    y = px_gfx_vp_adjust_y(y);
    // Update dirty area
    px_gfx_update_area(x, y, x, y + height - 1);
    // Draw vertical line
    for(j = 0; j < height; j++)
    {
        px_gfx_vp_draw_pixel(x, y, px_gfx.draw_prop.color_fg);
        y++;
    }
}

void px_gfx_draw_rect(px_gfx_xy_t    x,
                      px_gfx_xy_t    y,
                      px_gfx_xy_t    width,
                      px_gfx_xy_t    height)
{
    // Top
    px_gfx_draw_line_hor(x, y, width);
    // Bottom
    px_gfx_draw_line_hor(x, y + height - 1, width);
    // Left
    px_gfx_draw_line_ver(x, y, height);
    // Right
    px_gfx_draw_line_ver(x + width - 1, y, height);
}

void px_gfx_draw_fill_fg(px_gfx_xy_t    x,
                         px_gfx_xy_t    y,
                         px_gfx_xy_t    width,
                         px_gfx_xy_t    height)
{
    px_gfx_xy_t i, j;

    // Adjust coordinates relative to viewport
    x = px_gfx_vp_adjust_x(x);
    y = px_gfx_vp_adjust_y(y);
    // Update dirty area
    px_gfx_update_area(x, y, x + width - 1, y + height - 1);
    // Draw fill
    for(j = y; j < y + height; j++)
    {
        for(i = x; i < x + width; i++)
        {
            px_gfx_vp_draw_pixel(i, j, px_gfx.draw_prop.color_fg);
        }
    }    
}

void px_gfx_draw_fill_bg(px_gfx_xy_t    x,
                         px_gfx_xy_t    y,
                         px_gfx_xy_t    width,
                         px_gfx_xy_t    height)
{
    px_gfx_xy_t i, j;

    // Adjust coordinates relative to viewport
    x = px_gfx_vp_adjust_x(x);
    y = px_gfx_vp_adjust_y(y);
    // Update dirty area
    px_gfx_update_area(x, y, x + width - 1, y + height - 1);
    // Draw fill
    for(j = y; j < y + height; j++)
    {
        for(i = x; i < x + width; i++)
        {
            px_gfx_vp_draw_pixel(i, j, px_gfx.draw_prop.color_bg);
        }
    }    
}

void px_gfx_draw_circ(px_gfx_xy_t    x,
                      px_gfx_xy_t    y,
                      px_gfx_xy_t    radius)
{
    // https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
    // https://rosettacode.org/wiki/Bitmap/Midpoint_circle_algorithm#C
    px_gfx_xy_t f     = 1 - radius;
    px_gfx_xy_t ddf_x = 1;
    px_gfx_xy_t ddf_y = -2 * radius;
    px_gfx_xy_t dx    = 0;
    px_gfx_xy_t dy    = radius;

    // Adjust coordinates relative to viewport
    x = px_gfx_vp_adjust_x(x);
    y = px_gfx_vp_adjust_y(y);
    // Update dirty area
    px_gfx_update_area(x - radius, y - radius,
                       x + radius, y + radius);
    // 0 deg (12 o'clock)
    px_gfx_vp_draw_pixel(x,            y + radius, px_gfx.draw_prop.color_fg);
    // 90 deg (3 o'clock)
    px_gfx_vp_draw_pixel(x + radius,   y,          px_gfx.draw_prop.color_fg);
    // 180 deg (6 o'clock)
    px_gfx_vp_draw_pixel(x,            y - radius, px_gfx.draw_prop.color_fg);
    // 270 deg (9 o'clock)
    px_gfx_vp_draw_pixel(x - radius,   y,          px_gfx.draw_prop.color_fg);
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
        px_gfx_vp_draw_pixel(x + dx, y + dy, px_gfx.draw_prop.color_fg);
        px_gfx_vp_draw_pixel(x - dx, y + dy, px_gfx.draw_prop.color_fg);
        px_gfx_vp_draw_pixel(x + dx, y - dy, px_gfx.draw_prop.color_fg);
        px_gfx_vp_draw_pixel(x - dx, y - dy, px_gfx.draw_prop.color_fg);
        px_gfx_vp_draw_pixel(x + dy, y + dx, px_gfx.draw_prop.color_fg);
        px_gfx_vp_draw_pixel(x - dy, y + dx, px_gfx.draw_prop.color_fg);
        px_gfx_vp_draw_pixel(x + dy, y - dx, px_gfx.draw_prop.color_fg);
        px_gfx_vp_draw_pixel(x - dy, y - dx, px_gfx.draw_prop.color_fg);
    }
}

void px_gfx_draw_img(const px_gfx_img_t * img,
                     px_gfx_xy_t          x,
                     px_gfx_xy_t          y)
{
    px_gfx_xy_t     i;
    px_gfx_xy_t     j;
    uint8_t         mask;
	const uint8_t * data = img->data;

    // Set alignment
    if(px_gfx.draw_prop.align & PX_GFX_ALIGN_H_MID)
    {
        x -= (img->width + 1) / 2 - 1;
    }
    if(px_gfx.draw_prop.align & PX_GFX_ALIGN_H_RIGHT)
    {
        x -= img->width;
    }
    if(px_gfx.draw_prop.align & PX_GFX_ALIGN_V_MID)
    {
        y -= img->height / 2 - 1;
    }
    if(px_gfx.draw_prop.align & PX_GFX_ALIGN_V_BOT)
    {
        y -= img->height - 1;
    }
    // Adjust coordinates relative to viewport
    x = px_gfx_vp_adjust_x(x);
    y = px_gfx_vp_adjust_y(y);
    // Update dirty area
    px_gfx_update_area(x, y, x + img->width - 1, y + img->height - 1);
    // Repeat for each row
    for(j = 0; j < img->height; j++)
    {
        // Start at most significant bit
        mask = 1<<7;
        // Repeat for each pixel in row
        for(i = 0; i < img->width; i++)
        {
            if(*data & mask)
            {
                // Foreground color pixel
                px_gfx_vp_draw_pixel(x + i, y + j, px_gfx.draw_prop.color_fg);
            }
            else
            {
                // Does pixel need to be drawn?
                if(px_gfx.draw_prop.color_bg != PX_GFX_COLOR_TRANSPARENT)
                {
                    // Background color pixel
                    px_gfx_vp_draw_pixel(x + i, y + j, px_gfx.draw_prop.color_bg);
                }                
            }
            // Next bit
            mask >>= 1;
            // Finished with byte?
            if(mask == 0x00)
            {
                // Next byte
                data++;
                // Start again at most significant bit
                mask = 1<<7;
            }
        }
        // Ended on 8-bit boundary?
        if(mask != (1 << 7))
        {
            // No. Ignore rest of bits in byte
            data++;
        }
    }   
}

void px_gfx_draw_char(px_gfx_xy_t           x,
                      px_gfx_xy_t           y,
                      char                  glyph)
{
    px_gfx_img_t          img;
    const px_gfx_font_t * font        = px_gfx.draw_prop.font;
    const uint8_t *       data        = font->data;
	int                   width_bytes = (font->width + 7) / 8;

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
    px_gfx_draw_img(&img, x, y);
}

void px_gfx_draw_str(px_gfx_xy_t           x,
                     px_gfx_xy_t           y,
                     const char *          str)
{
    const px_gfx_font_t * font = px_gfx.draw_prop.font;

    // Save alignment
    px_gfx_align_t align = px_gfx.draw_prop.align;
    // No string?
    if((str == NULL) || (strlen(str) == 0))
    {
        return;
    }
    // Set alignment
    if(px_gfx.draw_prop.align & PX_GFX_ALIGN_H_MID)
    {
        x -= (font->width * (px_gfx_xy_t)strlen(str)) / 2;
    }
    if(px_gfx.draw_prop.align & PX_GFX_ALIGN_H_RIGHT)
    {
        x -= (font->width * (px_gfx_xy_t)strlen(str)) - 2;
    }
    if(px_gfx.draw_prop.align & PX_GFX_ALIGN_V_MID)
    {
        y -= (font->height - 1) / 2;
    }
    if(px_gfx.draw_prop.align & PX_GFX_ALIGN_V_BOT)
    {
        y -= font->height - 2;
    }
    px_gfx.draw_prop.align = PX_GFX_ALIGN_TOP_LEFT;
    // Draw each glyph
    while(*str != '\0')
    {
        px_gfx_draw_char(x, y, *str++);
        x += font->width;
    }
    // Restore alignment
    px_gfx.draw_prop.align = align;
}

void px_gfx_printf(px_gfx_xy_t           x,
                   px_gfx_xy_t           y,
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
    // Draw string
    px_gfx_draw_str(x, y, str);
}

