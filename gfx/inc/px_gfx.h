#ifndef __PX_GFX_H__
#define __PX_GFX_H__
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

/** 
 *  @ingroup GFX
 *  @defgroup PX_GFX px_gfx.h : Basic monochrome graphics library
 *  
 *  A basic monochrome (1 bit per pixel) graphics library.
 *  
 *  File(s):
 *  - gfx/inc/px_gfx.h 
 *  - gfx/inc/px_gfx_cfg_template.h
 *  - gfx/src/px_gfx.c 
 *  - gfx/inc/px_gfx_disp.h 
 *  - gfx/src/px_gfx_disp_st7567_jhd12864.c 
 *  
 *  Tool to convert images and fonts:
 *  [Ruison.com LCD Image Converter](https://github.com/riuson/lcd-image-converter) 
 *   
 *  Tool template files: 
 *  - gfx/px_gfx_font.tmpl 
 *  - gfx/px_gfx_image.tmpl 
 *   
 *  Tool preset file: 
 *  - px_gfx_preset.xml
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"

// Include project specific configuration. See "px_gfx_cfg_template.h"
#include "px_gfx_cfg.h"

// Check that all project specific options have been specified in "px_gfx_cfg.h"
#if (   !defined(PX_GFX_DISP_SIZE_X        ) \
     || !defined(PX_GFX_DISP_SIZE_Y        ) \
     || !defined(PX_GFX_CFG_STR_BUFFER_SIZE) \
     || !defined(PX_GFX_CFG_DEFAULT_FONT   ) )
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
/// @{
#define PX_GFX_X_MIN    0
#define PX_GFX_X_MID    (PX_GFX_DISP_SIZE_X / 2)
#define PX_GFX_X_MAX    (PX_GFX_DISP_SIZE_X - 1)
#define PX_GFX_Y_MIN    0
#define PX_GFX_Y_MID    (PX_GFX_DISP_SIZE_Y / 2)
#define PX_GFX_Y_MAX    (PX_GFX_DISP_SIZE_Y - 1)
/// @}

/// Foreground and background color definitions
typedef enum
{
    PX_GFX_COLOR_OFF,
    PX_GFX_COLOR_ON,
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
/// @{
#define PX_GFX_ALIGN_TOP_LEFT   (px_gfx_align_t)(PX_GFX_ALIGN_V_TOP | PX_GFX_ALIGN_H_LEFT )
#define PX_GFX_ALIGN_TOP_MID    (px_gfx_align_t)(PX_GFX_ALIGN_V_TOP | PX_GFX_ALIGN_H_MID  )
#define PX_GFX_ALIGN_TOP_RIGHT  (px_gfx_align_t)(PX_GFX_ALIGN_V_TOP | PX_GFX_ALIGN_H_RIGHT)
#define PX_GFX_ALIGN_MID        (px_gfx_align_t)(PX_GFX_ALIGN_V_MID | PX_GFX_ALIGN_H_MID  )
#define PX_GFX_ALIGN_BOT_LEFT   (px_gfx_align_t)(PX_GFX_ALIGN_V_BOT | PX_GFX_ALIGN_H_LEFT )
#define PX_GFX_ALIGN_BOT_MID    (px_gfx_align_t)(PX_GFX_ALIGN_V_BOT | PX_GFX_ALIGN_H_MID  )
#define PX_GFX_ALIGN_BOT_RIGHT  (px_gfx_align_t)(PX_GFX_ALIGN_V_BOT | PX_GFX_ALIGN_H_RIGHT)
/// @}

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
    px_gfx_xy_t     width;      ///< Width
    px_gfx_xy_t     height;     ///< Height
    px_gfx_xy_ref_t xy_ref;     ///< Coordinate reference
} px_gfx_view_port_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise graphics library.
 */
void px_gfx_init(void);

/**
 *  Clear display buffer.
 */
void px_gfx_buf_clear(void);

/**
 *  Redraw whole display using display buffer.
 */
void px_gfx_draw(void);

/**
 *  Update display with total area that has changed in display buffer.
 *  
 */
void px_gfx_draw_update(void);

/**
 *  Get total area of frame that has changed.
 *  
 *  @param area     Pointer to structure to contain area that has changed
 *  
 *  @retval true    frame buffer has changed
 *  @retval false   frame buffer has stayed the same (no change)
 */
bool px_gfx_update_area_get(px_gfx_area_t * area);

/**
 *  Reset drawing properties to default.
 */
void px_gfx_draw_prop_reset(void);

/**
 *  Set the new foreground drawing color.
 *  
 *  @param color            New drawing color
 *  
 *  @return px_gfx_color_t  Old drawing color
 */
px_gfx_color_t px_gfx_color_fg_set(px_gfx_color_t color);

/**
 *  Set the new background drawing color.
 *  
 *  @param color            New drawing color
 *  
 *  @return px_gfx_color_t  Old drawing color
 */
px_gfx_color_t px_gfx_color_bg_set(px_gfx_color_t color);

/**
 *  Set the new alignment.
 *  
 *  @param align            New alignment
 *  
 *  @return px_gfx_align_t  Old alignment
 */
px_gfx_align_t px_gfx_align_set(px_gfx_align_t align);

/**
 *  Set a drawing view port.
 *  
 *  Pixels outside the view port is clipped. Drawing coordinates can relative to
 *  the top left corner of the view port with the #PX_GFX_XY_REF_REL option or
 *  absolute (relative to the top left corner of the display) using the
 *  #PX_GFX_XY_REF_ABS option.
 *  
 *  @param x        Top left X coordinate of view port
 *  @param y        Top left Y coordinate of view port
 *  @param width    Width of view port
 *  @param height   Height of view port
 *  @param xy_ref   Coordinate are absolute (to display) or relative to view port
 */
void px_gfx_view_port_set(px_gfx_xy_t     x,
                          px_gfx_xy_t     y,
                          px_gfx_xy_t     width,
                          px_gfx_xy_t     height,
                          px_gfx_xy_ref_t xy_ref);

/**
 *  Reset view port.
 *  
 *  The view port is set to the whole display.
 */
void px_gfx_view_port_reset(void);

/**
 *  Set the new font.
 *  
 *  @param font             New font
 *  
 *  @return px_gfx_font_t * Old font
 */
const px_gfx_font_t * px_gfx_font_set(const px_gfx_font_t * font);

/**
 *  Draw a pixel using the current foreground color.
 *  
 *  @param x    X coordinate of pixel
 *  @param y    Y coordinate of pixel
 */
void px_gfx_draw_pixel(px_gfx_xy_t x,
                       px_gfx_xy_t y);

/**
 *  Draw a line using the current foreground color.
 *  
 *  @param x1   X coordinate of start point of the line
 *  @param y1   Y coordinate of start point of the line
 *  @param x2   X coordinate of end point of the line
 *  @param y2   Y coordinate of end point of the line
 */
void px_gfx_draw_line(px_gfx_xy_t x1,
                      px_gfx_xy_t y1,
                      px_gfx_xy_t x2,
                      px_gfx_xy_t y2);

/**
 *  Draw a horizontal line using the current foreground color.
 *  
 *  @param x        X coordinate of left point of the line
 *  @param y        X coordinate of left point of the line
 *  @param width    Width of the line (right)
 */
void px_gfx_draw_line_hor(px_gfx_xy_t x,
                          px_gfx_xy_t y,
                          px_gfx_xy_t width);

/**
 *  Draw a vertical line using the current foreground color.
 *  
 *  @param x        X coordinate of top point of the line
 *  @param y        X coordinate of top point of the line
 *  @param height   Height of the line (down)
 */
void px_gfx_draw_line_ver(px_gfx_xy_t x,
                          px_gfx_xy_t y,
                          px_gfx_xy_t height);

/**
 *  Draw a rectangle using the current foreground color.
 *  
 *  @param x        X coordinate of top left point of rectangle
 *  @param y        Y coordinate of top left point of rectangle
 *  @param width    Width of rectangle (right)
 *  @param height   Height of rectangle (down)
 */
void px_gfx_draw_rect(px_gfx_xy_t x,
                      px_gfx_xy_t y,
                      px_gfx_xy_t width,
                      px_gfx_xy_t height);

/**
 *  Draw a solid rectangular fill in the current foreground color.
 *  
 *  @param x        X coordinate of top left point of fill
 *  @param y        Y coordinate of top left point of fill
 *  @param width    Width of fill (right)
 *  @param height   Height of fill (down)
 */
void px_gfx_draw_fill_fg(px_gfx_xy_t x,
                         px_gfx_xy_t y,
                         px_gfx_xy_t width,
                         px_gfx_xy_t height);

/**
 *  Draw a solid rectangular fill in the current background color.
 *  
 *  @param x        X coordinate of top left point of fill
 *  @param y        Y coordinate of top left point of fill
 *  @param width    Width of fill (right)
 *  @param height   Height of fill (down)
 */
void px_gfx_draw_fill_bg(px_gfx_xy_t x,
                         px_gfx_xy_t y,
                         px_gfx_xy_t width,
                         px_gfx_xy_t height);

/**
 *  Draw a circle using the current foreground color.
 *  
 *  @param x        X coordinate of the circle center.
 *  @param y        X coordinate of the circle center.
 *  @param radius   Radius of the circle.
 */
void px_gfx_draw_circ(px_gfx_xy_t x,
                      px_gfx_xy_t y,
                      px_gfx_xy_t radius);

/**
 *  Draw a graphic image using the current foreground and background color.
 *  
 *  @param x    X coordinate of image
 *  @param y    Y coordinate of image
 *  @param img  Pointer to image structure
 */
void px_gfx_draw_img(px_gfx_xy_t          x,
                     px_gfx_xy_t          y,
                     const px_gfx_img_t * img);

/**
 *  Draw a font character using the current foreground color.
 *  
 *  @param x        X coordinate of font character
 *  @param y        Y coordinate of font character
 *  @param glyph    Character in font to draw
 */
void px_gfx_draw_char(px_gfx_xy_t x,
                      px_gfx_xy_t y,
                      char        glyph);

/**
 *  Draw a font string using the current foreground color.
 *  
 *  @param x        X coordinate of starting point of string
 *  @param y        Y coordinate of starting point of string
 *  @param str      String to draw
 */
void px_gfx_draw_str(px_gfx_xy_t  x,
                     px_gfx_xy_t  y,
                     const char * str);

/**
 *  Draw a formatted font string using the current foreground color.
 *  
 *  @param x        X coordinate of starting point of string
 *  @param y        Y coordinate of starting point of string
 *  @param format   Format string
 */
void px_gfx_printf(px_gfx_xy_t  x,
                   px_gfx_xy_t  y,
                   const char * format, ...);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
