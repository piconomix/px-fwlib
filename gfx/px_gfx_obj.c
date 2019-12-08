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

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdlib.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_gfx_obj.h"
#include "px_dbg.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_DBG_DECL_NAME("px_gfx_obj");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
px_gfx_obj_handle_t _px_gfx_obj_create(px_gfx_obj_type_t          obj_type,
                                       size_t                     size,
                                       px_gfx_obj_event_handler_t event_handler)
{
    px_gfx_obj_handle_t obj;

    PX_DBG_ASSERT(size >= sizeof(*obj));

    // Allocate memory for object and set fields to zero
    obj = calloc(1, size);
    PX_DBG_ASSERT(obj != NULL);
    // Set object type
    obj->obj_type = obj_type;
    // Set defaults
    obj->visible  = true;
    obj->update     = true;        
    // Set pointer to object event handler function
    obj->event_handler = event_handler;

    return obj;
}

bool px_gfx_obj_visible_get(px_gfx_obj_handle_t obj)
{
    PX_DBG_ASSERT(obj != NULL);

    return obj->visible;
}

void px_gfx_obj_visible_set(px_gfx_obj_handle_t obj)
{
    PX_DBG_ASSERT(obj != NULL);

    obj->visible = true;
}

void px_gfx_obj_visible_clr(px_gfx_obj_handle_t obj)
{
    PX_DBG_ASSERT(obj != NULL);

    obj->visible = false;
}

bool px_gfx_obj_update_get(px_gfx_obj_handle_t obj)
{
    PX_DBG_ASSERT(obj != NULL);

    return obj->update;
}

void px_gfx_obj_update_set(px_gfx_obj_handle_t obj)
{
    PX_DBG_ASSERT(obj != NULL);

    obj->update = true;
}

void px_gfx_obj_update_clr(px_gfx_obj_handle_t obj)
{
    PX_DBG_ASSERT(obj != NULL);

    obj->update = false;
}

void px_gfx_obj_draw(px_gfx_obj_handle_t obj)
{
    PX_DBG_ASSERT(obj != NULL);

    if(obj->visible && obj->event_handler)
    {
        (*obj->event_handler)(obj, PX_GFX_OBJ_EVENT_DRAW, NULL);
        obj->update = false;
    }
}
