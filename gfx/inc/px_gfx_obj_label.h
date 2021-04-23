#ifndef __PX_GFX_OBJ_LABEL_H__
#define __PX_GFX_OBJ_LABEL_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
 
    Title:          px_gfx_obj_label.h : Graphical text label object
    Author(s):      Pieter Conradie
    Creation Date:  2019-11-24

============================================================================= */

/** 
 *  @ingroup GFX
 *  @defgroup PX_GFX_OBJ_LABEL px_gfx_obj_label.h : Graphical text label object
 *  
 *  Graphical text label object.
 *  
 *  File(s):
 *  - gfx/inc/px_gfx_obj_label.h 
 *  - gfx/src/px_gfx_obj_label.c
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
/// Label object properties
typedef struct
{
    px_gfx_xy_t           x;        ///< X coordinate
    px_gfx_xy_t           y;        ///< Y coordinate
    const char *          str;
    const px_gfx_font_t * font;
    px_gfx_color_t        color_fg;
    px_gfx_color_t        color_bg;
    px_gfx_align_t        align;
} px_gfx_obj_label_prop_t;

/// Label object data structure
typedef struct
{
    px_gfx_obj_t                    obj;          ///< Common object properties
    const px_gfx_obj_label_prop_t * prop;         ///< Additional label properties
} px_gfx_obj_label_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
px_gfx_obj_handle_t             px_gfx_obj_label_create  (const px_gfx_obj_label_prop_t * prop);
const px_gfx_obj_label_prop_t * px_gfx_obj_label_prop_get(const px_gfx_obj_handle_t       obj);
void                            px_gfx_obj_label_prop_set(px_gfx_obj_handle_t             obj,
                                                          const px_gfx_obj_label_prop_t * prop);


/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif
