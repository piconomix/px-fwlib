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
 *  A basic monochrome (1 bit per pixel) graphics library.
 *  
 *  File(s):
 *  - gfx/px_gfx.h 
 *  - gfx/px_gfx_cfg_template.h
 *  - gfx/px_gfx.c 
 *  - gfx/px_gfx_lcd.h 
 *  - gfx/px_gfx_lcd_st7567_jhd12864.c 
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
#if (   !defined(PX_GFX_DISP_SIZE_X        ) \
     || !defined(PX_GFX_DISP_SIZE_Y        ) \
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
typedef int16_t px_gfx_xy_t;

/// @name Convenience coordinate definitions
//@{
#define PX_GFX_X_MIN    0
#define PX_GFX_X_MID    (PX_GFX_DISP_SIZE_X / 2)
#define PX_GFX_X_MAX    (PX_GFX_DISP_SIZE_X - 1)
#define PX_GFX_Y_MIN    0
#define PX_GFX_Y_MID    (PX_GFX_DISP_SIZE_Y / 2)
#define PX_GFX_Y_MAX    (PX_GFX_DISP_SIZE_Y - 1)
//@}

/// Foreground and background color definitions
typedef enum
{
    PX_GFX_COLOR_ON,
    PX_GFX_COLOR_OFF,
    PX_GFX_COLOR_INVERT,
    PX_GFX_COLOR_TRANSPARENT,
} px_gfx_color_t;

/// Alignment definition
typedef enum
{
    PX_GFX_ALIGN_V_TOP   = 0x00,    ///< Vertical   (y) top
    PX_GFX_ALIGN_V_MID   = 0x01,    ///< Vertical   (y) middle
    PX_GFX_ALIGN_V_BOT   = 0x02,    ///< Vertical   (y) bottom
    PX_GFX_ALIGN_H_LEFT  = 0x00,    ///< Horizontal (x) left
    PX_GFX_ALIGN_H_MID   = 0x10,    ///< Horizontal (x) middle
    PX_GFX_ALIGN_H_RIGHT = 0x20,    ///< Horizontal (x) right
} px_gfx_align_t;

/// @name Convenience alignment combination definitions
//@{
#define PX_GFX_ALIGN_TOP_LEFT   (px_gfx_align_t)(PX_GFX_ALIGN_V_TOP | PX_GFX_ALIGN_H_LEFT )
#define PX_GFX_ALIGN_TOP_MID    (px_gfx_align_t)(PX_GFX_ALIGN_V_TOP | PX_GFX_ALIGN_H_MID  )
#define PX_GFX_ALIGN_TOP_RIGHT  (px_gfx_align_t)(PX_GFX_ALIGN_V_TOP | PX_GFX_ALIGN_H_RIGHT)
#define PX_GFX_ALIGN_MID        (px_gfx_align_t)(PX_GFX_ALIGN_V_MID | PX_GFX_ALIGN_H_MID  )
#define PX_GFX_ALIGN_BOT_LEFT   (px_gfx_align_t)(PX_GFX_ALIGN_V_BOT | PX_GFX_ALIGN_H_LEFT )
#define PX_GFX_ALIGN_BOT_MID    (px_gfx_align_t)(PX_GFX_ALIGN_V_BOT | PX_GFX_ALIGN_H_MID  )
#define PX_GFX_ALIGN_BOT_RIGHT  (px_gfx_align_t)(PX_GFX_ALIGN_V_BOT | PX_GFX_ALIGN_H_RIGHT)
//@}

/// Image definition
typedef struct
{
    px_gfx_xy_t     width;
    px_gfx_xy_t     height;
    const uint8_t * data;
} px_gfx_img_t;

/// Font definition
typedef struct
{
    px_gfx_xy_t     width;
    px_gfx_xy_t     height;
    const uint8_t * data;
} px_gfx_font_t;

/// Area definition
typedef struct
{
    px_gfx_xy_t x1;             ///< Left
    px_gfx_xy_t y1;             ///< Top
    px_gfx_xy_t x2;             ///< Right
    px_gfx_xy_t y2;             ///< Bottom
} px_gfx_area_t;

/// X Y coordinate reference
typedef enum
{
    PX_GFX_XY_REF_REL = 0,      ///< Coordinate references are relative to view port
    PX_GFX_XY_REF_ABS,          ///< Coordinate references are absolute (relative to display)    
} px_gfx_xy_ref_t;

/// View port definition
typedef struct
{
    px_gfx_xy_t     x;          ///< Left
    px_gfx_xy_t     y;          ///< Top
    px_gfx_xy_t     width;      ///< Right
    px_gfx_xy_t     height;     ///< Bottom
    px_gfx_xy_ref_t xy_ref;     ///< Coordinate reference
} px_gfx_view_port_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
void px_gfx_init                (void);

void px_gfx_clear_frame         (void);
void px_gfx_draw_frame          (void);
void px_gfx_update_frame        (void);
bool px_gfx_update_area_get     (px_gfx_area_t * area);

void           px_gfx_draw_attr_reset   (void);
px_gfx_color_t px_gfx_color_fg_set      (px_gfx_color_t  color);
px_gfx_color_t px_gfx_color_bg_set      (px_gfx_color_t  color);
px_gfx_align_t px_gfx_align_set         (px_gfx_align_t  align);
void           px_gfx_view_port_set     (px_gfx_xy_t     x,
                                         px_gfx_xy_t     y,
                                         px_gfx_xy_t     width,
                                         px_gfx_xy_t     height,
                                         px_gfx_xy_ref_t xy_ref);
void           px_gfx_view_port_reset   (void);

void px_gfx_draw_pixel(px_gfx_xy_t    x,
                       px_gfx_xy_t    y);

void px_gfx_draw_line(px_gfx_xy_t    x1,
                      px_gfx_xy_t    y1,
                      px_gfx_xy_t    x2,
                      px_gfx_xy_t    y2);

void px_gfx_draw_line_hor(px_gfx_xy_t    x,
                          px_gfx_xy_t    y,
                          px_gfx_xy_t    width);

void px_gfx_draw_line_ver(px_gfx_xy_t    x,
                          px_gfx_xy_t    y,
                          px_gfx_xy_t    height);

void px_gfx_draw_rect(px_gfx_xy_t    x,
                      px_gfx_xy_t    y,
                      px_gfx_xy_t    width,
                      px_gfx_xy_t    height);

void px_gfx_draw_fill_fg(px_gfx_xy_t    x,
                         px_gfx_xy_t    y,
                         px_gfx_xy_t    width,
                         px_gfx_xy_t    height);

void px_gfx_draw_fill_bg(px_gfx_xy_t    x,
                         px_gfx_xy_t    y,
                         px_gfx_xy_t    width,
                         px_gfx_xy_t    height);

void px_gfx_draw_circ(px_gfx_xy_t    x,
                      px_gfx_xy_t    y,
                      px_gfx_xy_t    radius);

void px_gfx_draw_img(const px_gfx_img_t *  img,
                     px_gfx_xy_t           x,
                     px_gfx_xy_t           y);

void px_gfx_draw_char(const px_gfx_font_t * font,
                      px_gfx_xy_t           x,
                      px_gfx_xy_t           y,
                      char                  glyph);

void px_gfx_draw_str(const px_gfx_font_t * font,
                     px_gfx_xy_t           x,
                     px_gfx_xy_t           y,
                     const char *          str);

void px_gfx_printf(const px_gfx_font_t * font,
                   px_gfx_xy_t           x,
                   px_gfx_xy_t           y,
                   const char *          format, ...);

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif // #ifndef __PX_GFX_H__
