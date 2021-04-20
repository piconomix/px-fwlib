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

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_gfx_obj_label.h"
#include "px_gfx_fonts.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("px_gfx_obj_label");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void px_gfx_obj_label_event_handler(px_gfx_obj_handle_t obj, 
                                           px_gfx_obj_event_t  event,
                                           void *              data)
{
    px_gfx_obj_label_t *            obj_label = (px_gfx_obj_label_t *)obj;
    const px_gfx_obj_label_prop_t * prop;

    // Sanity checks
    PX_LOG_ASSERT(obj             != NULL);
    PX_LOG_ASSERT(obj->obj_type   == PX_GFX_OBJ_TYPE_LABEL);

    // Get pointer to properties
    prop = obj_label->prop;
    PX_LOG_ASSERT(prop != NULL);

    // Handle event
    switch(event)
    {
    case PX_GFX_OBJ_EVENT_DRAW:
        // Update flag set?
        if(obj->update)
        {
            // Reset flag
            obj->update = false;
            // Draw label
            px_gfx_font_set(prop->font);
            px_gfx_color_fg_set(prop->color_fg);
            px_gfx_color_bg_set(prop->color_bg);
            px_gfx_align_set(prop->align);
            px_gfx_draw_str(prop->x, prop->y, prop->str);
        }
        break;
    }
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
px_gfx_obj_handle_t px_gfx_obj_label_create(const px_gfx_obj_label_prop_t * prop)
{
    px_gfx_obj_label_t * obj_label;

    // Create label object
    obj_label = (px_gfx_obj_label_t *)_px_gfx_obj_create(PX_GFX_OBJ_TYPE_LABEL,
                                                         sizeof(*obj_label), 
                                                         &px_gfx_obj_label_event_handler);
    PX_LOG_ASSERT(obj_label != NULL);
    PX_LOG_ASSERT(prop      != NULL);

    // Set pointer to properties
    obj_label->prop = prop;
	
	return (px_gfx_obj_handle_t)obj_label;
}

const px_gfx_obj_label_prop_t * px_gfx_obj_label_prop_get(const px_gfx_obj_handle_t obj)
{
    px_gfx_obj_label_t * obj_label = (px_gfx_obj_label_t *)obj;

    // Sanity checks
    PX_LOG_ASSERT(obj           != NULL);
    PX_LOG_ASSERT(obj->obj_type == PX_GFX_OBJ_TYPE_LABEL);
    
    return obj_label->prop;
}

void px_gfx_obj_label_prop_set(px_gfx_obj_handle_t             obj,
                               const px_gfx_obj_label_prop_t * prop)
{
    px_gfx_obj_label_t * obj_label = (px_gfx_obj_label_t *)obj;

    // Sanity checks
    PX_LOG_ASSERT(obj           != NULL);
    PX_LOG_ASSERT(obj->obj_type == PX_GFX_OBJ_TYPE_LABEL);

    obj_label->prop = prop;
}

