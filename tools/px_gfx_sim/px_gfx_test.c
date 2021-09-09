#include <stdio.h>
#include <string.h>

#include "px_gfx.h"
#include "px_gfx_objects.h"
#include "px_gfx_resources.h"

px_gfx_obj_handle_t obj_win;
px_gfx_obj_handle_t obj_graph;
px_gfx_xy_t         obj_graph_data_y[128];
px_gfx_obj_handle_t obj_label_val;
px_gfx_obj_handle_t obj_label_unit;
char                obj_label_val_str[16];
float               temp_deg;

static const px_gfx_obj_graph_prop_t obj_graph_prop = 
{
    .x        = 0,
    .y        = 14,
    .width    = PX_GFX_DISP_SIZE_X,
    .height   = 50,
    .color_fg = PX_GFX_COLOR_ON,
    .color_bg = PX_GFX_COLOR_OFF,
    .data_y   = obj_graph_data_y,
};

static const px_gfx_obj_label_prop_t obj_label_prop_val = 
{
    .x        = 60,
    .y        = 20,
    .str      = obj_label_val_str,
    .font     = &px_gfx_font_11x14,
    .color_fg = PX_GFX_COLOR_ON,
    .color_bg = PX_GFX_COLOR_OFF,
    .align    = PX_GFX_ALIGN_BOT_RIGHT,
};

static const px_gfx_obj_label_prop_t obj_label_prop_unit = 
{
    .x        = 62,
    .y        = 20,
    .str      = "C",
    .font     = &px_gfx_font_11x14,
    .color_fg = PX_GFX_COLOR_ON,
    .color_bg = PX_GFX_COLOR_TRANSPARENT,
    .align    = PX_GFX_ALIGN_BOT_LEFT,
};

void px_gfx_test_start(void)
{
    // Initialise graphics
    px_gfx_init();
    // Create full display window
    obj_win = px_gfx_obj_win_create(&px_gfx_obj_win_prop_full_disp);
    // Create objects
    obj_graph = px_gfx_obj_graph_create(&obj_graph_prop);
    memset(obj_graph_data_y, 0, sizeof(obj_graph_data_y));
    obj_graph_data_y[10] = 10;
    obj_graph_data_y[20] = 20;
    temp_deg = 0.0;
    sprintf(obj_label_val_str, "%2.1f", temp_deg);
    obj_label_val  = px_gfx_obj_label_create(&obj_label_prop_val);
    obj_label_unit = px_gfx_obj_label_create(&obj_label_prop_unit);
    // Add objects to window
    px_gfx_obj_win_add_child(obj_win, obj_graph);
    px_gfx_obj_win_add_child(obj_win, obj_label_val);
    px_gfx_obj_win_add_child(obj_win, obj_label_unit);
}

void px_gfx_test_update(void)
{
    // Update value label
    temp_deg += 0.1f;
    sprintf(obj_label_val_str, "%2.1f", temp_deg);
    px_gfx_obj_update_set(obj_label_val);
    // Draw window
    px_gfx_obj_draw(obj_win);
    // Update display
    px_gfx_draw_update();
}
