/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
 
    Title:          px_gfx_obj_graph.h : Graphical graph object
    Author(s):      Pieter Conradie
    Creation Date:  2019-11-24

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_gfx_obj_graph.h"
#include "px_gfx_fonts.h"
#include "px_dbg.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_DBG_DECL_NAME("px_gfx_obj_graph");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static px_gfx_xy_t px_gfx_obj_graph_scale_y(const px_gfx_obj_graph_prop_t * prop,
                                            px_gfx_xy_t                     y)
{
    return (prop->height - y - 1);
}

static void px_gfx_obj_graph_event_handler(px_gfx_obj_handle_t obj, 
                                           px_gfx_obj_event_t  event,
                                           void *              data)
{
    px_gfx_obj_graph_t *            obj_graph = (px_gfx_obj_graph_t *)obj;
    const px_gfx_obj_graph_prop_t * prop;
    px_gfx_xy_t                     x;
    const px_gfx_xy_t *             data_y;    

    // Sanity checks
    PX_DBG_ASSERT(obj             != NULL);
    PX_DBG_ASSERT(obj->obj_type   == PX_GFX_OBJ_TYPE_GRAPH);

    // Get pointer to properties
    prop = obj_graph->prop;
    PX_DBG_ASSERT(prop != NULL);
    // Get pointer to Y data
    data_y = obj_graph->prop->data_y;
    // Handle event
    switch(event)
    {
    case PX_GFX_OBJ_EVENT_DRAW:
        // Update flag set?
        if(obj->update)
        {
            // Reset flag
            obj->update = false;
            if(prop->color_bg != PX_GFX_COLOR_TRANSPARENT)
            {
                // Draw graph background fill
                px_gfx_color_bg_set(prop->color_bg);
                px_gfx_draw_fill_bg(prop->x,
                                    prop->y,
                                    prop->width,
                                    prop->height);
            }
            // Set clipping view port
            px_gfx_view_port_set(prop->x,
                                 prop->y,
                                 prop->width,
                                 prop->height,
                                 PX_GFX_XY_REF_REL);
            // Draw graph
            px_gfx_color_fg_set(prop->color_fg);
            for(x = 0; x < (prop->width - 1); x++)
            {
                px_gfx_draw_line(x,     px_gfx_obj_graph_scale_y(prop, data_y[x]), 
                                 x + 1, px_gfx_obj_graph_scale_y(prop, data_y[x + 1]));
            }
            // Reset view port
            px_gfx_view_port_reset();
        }
        break;
    }
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
px_gfx_obj_handle_t px_gfx_obj_graph_create(const px_gfx_obj_graph_prop_t * prop)
{
    px_gfx_obj_graph_t * obj_graph;

    // Create graph object
    obj_graph = (px_gfx_obj_graph_t *)_px_gfx_obj_create(PX_GFX_OBJ_TYPE_GRAPH,
                                                         sizeof(*obj_graph), 
                                                         &px_gfx_obj_graph_event_handler);
    PX_DBG_ASSERT(obj_graph != NULL);
    PX_DBG_ASSERT(prop      != NULL);

    // Set pointer to properties
    obj_graph->prop = prop;
	
	return (px_gfx_obj_handle_t)obj_graph;
}

const px_gfx_obj_graph_prop_t * px_gfx_obj_graph_prop_get(const px_gfx_obj_handle_t obj)
{
    px_gfx_obj_graph_t * obj_graph = (px_gfx_obj_graph_t *)obj;

    // Sanity checks
    PX_DBG_ASSERT(obj           != NULL);
    PX_DBG_ASSERT(obj->obj_type == PX_GFX_OBJ_TYPE_GRAPH);
    
    return obj_graph->prop;
}

void px_gfx_obj_graph_prop_set(px_gfx_obj_handle_t             obj,
                               const px_gfx_obj_graph_prop_t * prop)
{
    px_gfx_obj_graph_t * obj_graph = (px_gfx_obj_graph_t *)obj;

    // Sanity checks
    PX_DBG_ASSERT(obj           != NULL);
    PX_DBG_ASSERT(obj->obj_type == PX_GFX_OBJ_TYPE_GRAPH);

    obj_graph->prop = prop;
}

