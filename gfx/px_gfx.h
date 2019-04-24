#ifndef __PX_GFX_H__
#define __PX_GFX_H__
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

/** 
 *  @ingroup GFX
 *  @defgroup PX_GFX px_gfx.h : Basic monochrome graphics library
 *  
 *  A basic monochrome (1 bit per pixel) graphics library for the Truly 
 *  128 x 64 LCD.
 *  
 *  File(s):
 *  - gfx/px_gfx.h 
 *  - gfx/px_gfx_cfg_template.h 
 *  - gfx/px_gfx.c
 *  
 *  Excellent tool to convert images and fonts: 
 *  [Ruison.com LCD Image Converter](https://github.com/riuson/lcd-image-converter)
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

// Include project specific config. See "px_gfx_cfg_template.h"
#include "px_gfx_cfg.h"

// Check that all project specific options have been specified in "px_gfx_cfg.h"
#if (   !defined(PX_GFX_DISP_SIZE_X) \
     || !defined(PX_GFX_DISP_SIZE_Y) \
     || !defined(PX_GFX_CFG_STR_BUFFER_SIZE)  )
      )
#error "One or more options not defined in 'px_gfx_cfg.h'"
#endif

#ifdef __cplusplus
extern "C"
{
#endif
/* _____DEFINITIONS__________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Size definition of an X or Y coordinate
typedef uint8_t px_gfx_xy_t;

typedef enum
{
    PX_GFX_COLOR_ON,
    PX_GFX_COLOR_OFF,
    PX_GFX_COLOR_INVERT,
} px_gfx_color_t;

typedef struct
{
    px_gfx_xy_t     width;
    px_gfx_xy_t     height;
    const uint8_t * data;
} px_gfx_img_t;

typedef struct
{
    px_gfx_xy_t     width;
    px_gfx_xy_t     height;
    const uint8_t * data;
} px_gfx_font_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
void px_gfx_init        (void);
void px_gfx_update      (void);
void px_gfx_clr_scr     (void);

void px_gfx_draw_pixel(px_gfx_xy_t    x,
                       px_gfx_xy_t    y,
                       px_gfx_color_t color);

void px_gfx_draw_line(px_gfx_xy_t    x1,
                      px_gfx_xy_t    y1,
                      px_gfx_xy_t    x2,
                      px_gfx_xy_t    y2,
                      px_gfx_color_t color);

void px_gfx_draw_line_hor(px_gfx_xy_t    x,
                          px_gfx_xy_t    y,
                          px_gfx_xy_t    width,
                          px_gfx_color_t color);

void px_gfx_draw_line_ver(px_gfx_xy_t    x,
                          px_gfx_xy_t    y,
                          px_gfx_xy_t    height,
                          px_gfx_color_t color);

void px_gfx_draw_rect(px_gfx_xy_t    x,
                      px_gfx_xy_t    y,
                      px_gfx_xy_t    width,
                      px_gfx_xy_t    height,
                      px_gfx_color_t color);

void px_gfx_draw_fill(px_gfx_xy_t    x,
                      px_gfx_xy_t    y,
                      px_gfx_xy_t    width,
                      px_gfx_xy_t    height,
                      px_gfx_color_t color);

void px_gfx_draw_circ(px_gfx_xy_t    x,
                      px_gfx_xy_t    y,
                      px_gfx_xy_t    radius,
                      px_gfx_color_t color);

void px_gfx_draw_img(px_gfx_xy_t     x,
                     px_gfx_xy_t     y,
                     px_gfx_xy_t     width,
                     px_gfx_xy_t     height,
                     px_gfx_color_t  color,
                     const uint8_t * data);

void px_gfx_draw_char(px_gfx_font_t * font,
                      px_gfx_xy_t     x,
                      px_gfx_xy_t     y,
                      px_gfx_color_t  color,
                      char            glyph);

void px_gfx_draw_str(px_gfx_font_t * font,
                     px_gfx_xy_t     x,
                     px_gfx_xy_t     y,
                     px_gfx_color_t  color,
                     const char *    str);

void px_gfx_printf(px_gfx_font_t * font,
                   px_gfx_xy_t     x,
                   px_gfx_xy_t     y,
                   px_gfx_color_t  color,
                   const char *    format, ...);

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif // #ifndef __PX_GFX_H__
