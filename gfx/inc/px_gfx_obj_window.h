#ifndef __PX_GFX_OBJ_WINDOW_H__
#define __PX_GFX_OBJ_WINDOW_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
 
    Title:          px_gfx_obj_window.h : Graphical window object
    Author(s):      Pieter Conradie
    Creation Date:  2019-11-24

============================================================================= */

/** 
 *  @ingroup GFX
 *  @defgroup PX_GFX_OBJ_WINDOW px_gfx_obj_window.h : Graphical window object
 *  
 *  File(s):
 *  - gfx/px_gfx_obj_window.h 
 *  - gfx/px_gfx_obj_window.c
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_gfx.h"
#include "px_gfx_obj.h"

#ifdef __cplusplus
extern "C"
{
#endif
/* _____DEFINITIONS__________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Window object properties
typedef struct
{
    px_gfx_xy_t    x;               ///< X coordinate
    px_gfx_xy_t    y;               ///< Y coordinate
    px_gfx_xy_t    width;           ///< Right
    px_gfx_xy_t    height;          ///< Bottom
    px_gfx_color_t color_bg;        ///< Background color
} px_gfx_obj_window_prop_t;

/// Window object data structure
typedef struct
{
    px_gfx_obj_t                     obj;
    const px_gfx_obj_window_prop_t * prop;
    px_gfx_obj_t *                   obj_first;
} px_gfx_obj_window_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */
/// Properties for a full screen window
const px_gfx_obj_window_prop_t px_gfx_obj_window_prop_full_disp;

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
px_gfx_obj_handle_t              px_gfx_obj_window_create   (const px_gfx_obj_window_prop_t * prop);
const px_gfx_obj_window_prop_t * px_gfx_obj_window_prop_get (const px_gfx_obj_handle_t        obj);
void                             px_gfx_obj_window_prop_set (px_gfx_obj_handle_t              obj,
                                                             const px_gfx_obj_window_prop_t * prop);
void                             px_gfx_obj_window_add_child(px_gfx_obj_handle_t              obj_window,
                                                             px_gfx_obj_handle_t              obj_child);

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif // #ifndef __PX_GFX_OBJ_WINDOW_H__
