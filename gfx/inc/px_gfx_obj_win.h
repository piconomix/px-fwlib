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
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
 
    Title:          px_gfx_obj_win.h : Graphical window object
    Author(s):      Pieter Conradie
    Creation Date:  2019-11-24

============================================================================= */

/** 
 *  @ingroup GFX
 *  @defgroup PX_GFX_OBJ_WINDOW px_gfx_obj_win.h : Graphical window object
 *  
 *  File(s):
 *  - gfx/inc/px_gfx_obj_win.h 
 *  - gfx/src/px_gfx_obj_win.c
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"
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
    px_gfx_xy_t    x;                           ///< Left
    px_gfx_xy_t    y;                           ///< Top
    px_gfx_xy_t    width;                       ///< Width
    px_gfx_xy_t    height;                      ///< Height
    px_gfx_color_t color_bg;                    ///< Background color
} px_gfx_obj_win_prop_t;

/// Window object data structure
typedef struct
{
    px_gfx_obj_t                  obj;          ///< Common object properties
    const px_gfx_obj_win_prop_t * prop;         ///< Additional window properties
    px_gfx_obj_t *                obj_first;    ///< Pointer to first object in child linked list
} px_gfx_obj_win_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */
/// Properties for a full screen window
extern const px_gfx_obj_win_prop_t px_gfx_obj_win_prop_full_disp;

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
px_gfx_obj_handle_t           px_gfx_obj_win_create   (const px_gfx_obj_win_prop_t * prop);
const px_gfx_obj_win_prop_t * px_gfx_obj_win_prop_get (const px_gfx_obj_handle_t     obj);
void                          px_gfx_obj_win_prop_set (px_gfx_obj_handle_t           obj,
                                                       const px_gfx_obj_win_prop_t * prop);
void                          px_gfx_obj_win_add_child(px_gfx_obj_handle_t           obj_win,
                                                       px_gfx_obj_handle_t           obj_child);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
