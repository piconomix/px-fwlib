#ifndef __PX_GFX_OBJ_H__
#define __PX_GFX_OBJ_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
 
    Title:          px_gfx_obj.h : Graphical object layer on drawing layer
    Author(s):      Pieter Conradie
    Creation Date:  2019-06-30

============================================================================= */

/** 
 *  @ingroup GFX
 *  @defgroup PX_GFX_OBJ px_gfx_obj.h : Graphical object layer
 *  
 *  Graphical object layer on primitive drawing layer.
 *  
 *  File(s):
 *  - gfx/inc/px_gfx_obj.h 
 *  - gfx/src/px_gfx_obj.c
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_gfx.h"

#ifdef __cplusplus
extern "C"
{
#endif
/* _____DEFINITIONS__________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */
/**
 *  List of object types
 */
typedef enum
{
    PX_GFX_OBJ_TYPE_UNKNOWN = 0,
    PX_GFX_OBJ_TYPE_WINDOW  = 1,
    PX_GFX_OBJ_TYPE_LABEL   = 2,
    PX_GFX_OBJ_TYPE_GRAPH   = 3,
} px_gfx_obj_type_t;

/**
 *  Event types
 */
typedef enum
{
    PX_GFX_OBJ_EVENT_DRAW,
} px_gfx_obj_event_t;

/** 
 *  Declaration of the object handle
 */
typedef struct px_gfx_obj_s * px_gfx_obj_handle_t;

/**
 *  Definition of a pointer to the object's event handler function
 *  
 *  @param obj      object handle
 *  @param event    event that object must handle
 *  @param data     (optional) event data
 */
typedef void (*px_gfx_obj_event_handler_t)(px_gfx_obj_handle_t obj, 
                                           px_gfx_obj_event_t  event,
                                           void *              data);

/// Object data structure
typedef struct px_gfx_obj_s
{
    px_gfx_obj_type_t          obj_type;        ///< Object type
    bool                       visible;         ///< Flag to indicate if object is visible
    bool                       update;          ///< Flag to indicate if object has been updated and must be drawn
    px_gfx_obj_event_handler_t event_handler;   ///< Object event function handler
    struct px_gfx_obj_s *      obj_next;        ///< Pointer to next object in linked list
} px_gfx_obj_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
px_gfx_obj_handle_t _px_gfx_obj_create(px_gfx_obj_type_t          obj_type,
                                       size_t                     size,
                                       px_gfx_obj_event_handler_t event_handler);

bool           px_gfx_obj_visible_get (px_gfx_obj_handle_t  obj);
void           px_gfx_obj_visible_set (px_gfx_obj_handle_t  obj);
void           px_gfx_obj_visible_clr (px_gfx_obj_handle_t  obj);

bool           px_gfx_obj_update_get  (px_gfx_obj_handle_t  obj);
void           px_gfx_obj_update_set  (px_gfx_obj_handle_t  obj);
void           px_gfx_obj_update_clr  (px_gfx_obj_handle_t  obj);

void           px_gfx_obj_draw        (px_gfx_obj_handle_t  obj);

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif // #ifndef __PX_GFX_OBJ_H__
