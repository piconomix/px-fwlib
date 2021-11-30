#ifndef __PX_GFX_OBJ_GRAPH_H__
#define __PX_GFX_OBJ_GRAPH_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
 
    Title:          px_gfx_obj_graph.h : Graphical graph object
    Author(s):      Pieter Conradie
    Creation Date:  2019-11-24

============================================================================= */

/** 
 *  @ingroup GFX
 *  @defgroup PX_GFX_OBJ_GRAPH px_gfx_obj_graph.h : Graphical graph object
 *  
 *  Graphical graph object.
 *  
 *  File(s):
 *  - gfx/inc/px_gfx_obj_graph.h 
 *  - gfx/src/px_gfx_obj_graph.c
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
/// Label object properties
typedef struct
{
    px_gfx_xy_t         x;                  ///< X coordinate
    px_gfx_xy_t         y;                  ///< Y coordinate
    px_gfx_xy_t         width;              ///< Right
    px_gfx_xy_t         height;             ///< Bottom
    px_gfx_color_t      color_fg;           ///< Foreground color
    px_gfx_color_t      color_bg;           ///< Background color
    const px_gfx_xy_t * data_y;             ///< Buffer containing Y height data for each X column
} px_gfx_obj_graph_prop_t;

/// Label object data structure
typedef struct
{
    px_gfx_obj_t                    obj;    ///< Common object properties
    const px_gfx_obj_graph_prop_t * prop;   ///< Additional graph properties
} px_gfx_obj_graph_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
px_gfx_obj_handle_t             px_gfx_obj_graph_create  (const px_gfx_obj_graph_prop_t * prop);
const px_gfx_obj_graph_prop_t * px_gfx_obj_graph_prop_get(const px_gfx_obj_handle_t       obj);
void                            px_gfx_obj_graph_prop_set(px_gfx_obj_handle_t             obj,
                                                          const px_gfx_obj_graph_prop_t * prop);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
