/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
 
    Title:          px_gfx_obj_win.h : Graphical window object
    Author(s):      Pieter Conradie
    Creation Date:  2019-11-24

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_gfx_obj_win.h"
#include "px_dbg.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_DBG_DECL_NAME("px_gfx_obj_win");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
const px_gfx_obj_win_prop_t px_gfx_obj_win_prop_full_disp = 
{
    .x        = 0,
    .y        = 0,
    .width    = PX_GFX_DISP_SIZE_X,
    .height   = PX_GFX_DISP_SIZE_Y,
    .color_bg = PX_GFX_COLOR_OFF,
};

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void px_gfx_obj_win_event_handler(px_gfx_obj_handle_t obj, 
                                         px_gfx_obj_event_t  event,
                                         void *              data)
{
    px_gfx_obj_win_t *            obj_win = (px_gfx_obj_win_t *)obj;
    const px_gfx_obj_win_prop_t * prop;
    px_gfx_obj_t *                obj_child;

    // Sanity checks
    PX_DBG_ASSERT(obj           != NULL);
    PX_DBG_ASSERT(obj->obj_type == PX_GFX_OBJ_TYPE_WIN);
    // Get pointer to properties
    prop = obj_win->prop;
    PX_DBG_ASSERT(prop != NULL);
    // Handle event
    switch(event)
    {
    case PX_GFX_OBJ_EVENT_DRAW:
        // Draw flag set?
        if(obj->update)
        {
            if(prop->color_bg != PX_GFX_COLOR_TRANSPARENT)
            {
                // Draw window background fill
                px_gfx_color_bg_set(prop->color_bg);
                px_gfx_draw_fill_bg(prop->x,
                                    prop->y,
                                    prop->width,
                                    prop->height);
            }
        }
        // View port specified?
        if(  (prop->x      != 0)
           ||(prop->y      != 0)
           ||(prop->width  != PX_GFX_DISP_SIZE_X)
           ||(prop->height != PX_GFX_DISP_SIZE_Y)  )
        {
            // All object coordinates are relative to window
            px_gfx_view_port_set(prop->x,
                                 prop->y,
                                 prop->width,
                                 prop->height,
                                 PX_GFX_XY_REF_REL);
        }
        // Start at first child in window list
        obj_child = obj_win->obj_first;
        // Repeat until all objects in window list have been drawn
        while(obj_child != NULL)
        {
            // Has window (parent object) been redrawn?
            if(obj->update)
            {
                // Set child object's draw flag
                obj_child->update = true;
            }
            // Draw object
            px_gfx_obj_draw(obj_child);
            // Next child in list
            obj_child = obj_child->obj_next;
        }
        // Reset draw flag
        obj->update = false;
        break;
    }
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
px_gfx_obj_handle_t px_gfx_obj_win_create(const px_gfx_obj_win_prop_t * prop)
{
    px_gfx_obj_win_t * obj_win;

    // Create window object
    obj_win = (px_gfx_obj_win_t *)_px_gfx_obj_create(PX_GFX_OBJ_TYPE_WIN,
                                                     sizeof(*obj_win), 
                                                     &px_gfx_obj_win_event_handler);
    // Sanity checks
    PX_DBG_ASSERT(obj_win != NULL);
    PX_DBG_ASSERT(prop    != NULL);
    // Set pointer to properties
    obj_win->prop = prop;

	return (px_gfx_obj_handle_t)obj_win;
}

const px_gfx_obj_win_prop_t * px_gfx_obj_win_prop_get(const px_gfx_obj_handle_t obj)
{
    const px_gfx_obj_win_t * obj_win = (px_gfx_obj_win_t *)obj;

    // Sanity checks
    PX_DBG_ASSERT(obj           != NULL);
    PX_DBG_ASSERT(obj->obj_type == PX_GFX_OBJ_TYPE_WIN);
    
    return obj_win->prop;
}

void px_gfx_obj_win_prop_set(px_gfx_obj_handle_t           obj,
                             const px_gfx_obj_win_prop_t * prop)
{
    px_gfx_obj_win_t * obj_win = (px_gfx_obj_win_t *)obj;

    // Sanity checks
    PX_DBG_ASSERT(obj           != NULL);
    PX_DBG_ASSERT(obj->obj_type == PX_GFX_OBJ_TYPE_LABEL);

    obj_win->prop = prop;
}

void px_gfx_obj_win_add_child(px_gfx_obj_handle_t obj_win, 
                              px_gfx_obj_handle_t obj_child)
{
    px_gfx_obj_win_t * obj_parent = (px_gfx_obj_win_t *)obj_win;
    px_gfx_obj_t *     obj_next;

    // Sanity checks
    PX_DBG_ASSERT(obj_win           != NULL);
    PX_DBG_ASSERT(obj_win->obj_type == PX_GFX_OBJ_TYPE_WIN);
    // List empty?
    if(obj_parent->obj_first == NULL)
    {
        // Add first child
        obj_parent->obj_first = obj_child;
        return;
    }
    // Find last object in list
    obj_next = obj_parent->obj_first;
    while(obj_next->obj_next != NULL)
    {
        obj_next = obj_next->obj_next;
    }
    // Add child to end of list
    obj_next->obj_next = obj_child;
}
