/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
 
    Title:          px_cli_cmds_lcd.h : CLI commands for LCD
    Author(s):      Pieter Conradie
    Creation Date:  2018-05-09

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <string.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_cli.h"
#include "px_board.h"
#include "px_gfx.h"
#include "px_gfx_res.h"
#include "px_dbg.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_DBG_DECL_NAME("cli_cmds_lcd");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static const char* px_cli_cmd_fn_lcd_clr(uint8_t argc, char* argv[])
{
    px_gfx_buf_clear();
    px_gfx_draw_update();

    return NULL;
}

static const char* px_cli_cmd_fn_lcd_backlight(uint8_t argc, char* argv[])
{
    // <on|off>
    switch(px_cli_util_argv_to_option(0, "on\0off\0"))
    {
    case 0: 
        PX_LCD_BACKLIGHT_ON();
        break;
    case 1: 
        PX_LCD_BACKLIGHT_OFF();
        break;
    default:
        return "Error: Invalid parameter";
    }

    return NULL;
}

static const char* px_cli_cmd_fn_lcd_pixel(uint8_t argc, char* argv[])
{
    px_gfx_xy_t     x;
    px_gfx_xy_t     y;
    px_gfx_color_t color;

    // <x>
    if(!px_cli_util_argv_to_u8(0, 0, PX_GFX_DISP_SIZE_X - 1))
    {
        printf("Error. <x> must be 0 to %u\n", PX_GFX_DISP_SIZE_X - 1);
        return NULL;
    }
    x = px_cli_argv_val.u8;

    // <y>
    if(!px_cli_util_argv_to_u8(1, 0, PX_GFX_DISP_SIZE_Y - 1))
    {
        printf("Error. <y> must be 0 to %u\n", PX_GFX_DISP_SIZE_Y - 1);
        return NULL;
    }
    y = px_cli_argv_val.u8;

    // [color:0|1|i]
    if(argc > 2)
    {
        switch(argv[2][0])
        {
        case '0':
            color = PX_GFX_COLOR_OFF;
            break;
        case '1':
            color = PX_GFX_COLOR_ON;
            break;
        case 'i':
            color = PX_GFX_COLOR_INVERT;
            break;
        default:
            return "Error. [color:0|1|i] must be 0 or 1 or i (invert)";
        }
    }
    else
    {
        color = PX_GFX_COLOR_ON;
    }

    px_gfx_color_fg_set(color);
    px_gfx_draw_pixel(x, y);
    px_gfx_draw_update();

    return NULL;
}

static const char* px_cli_cmd_fn_lcd_line(uint8_t argc, char* argv[])
{
    px_gfx_xy_t     x1;
    px_gfx_xy_t     y1;
    px_gfx_xy_t     x2;
    px_gfx_xy_t     y2;
    px_gfx_color_t color;

    // <x1>
    if(!px_cli_util_argv_to_u8(0, 0, PX_GFX_DISP_SIZE_X - 1))
    {
        printf("Error. <x1> must be 0 to %u\n", PX_GFX_DISP_SIZE_X - 1);
        return NULL;
    }
    x1 = px_cli_argv_val.u8;

    // <y1>
    if(!px_cli_util_argv_to_u8(1, 0, PX_GFX_DISP_SIZE_Y - 1))
    {
        printf("Error. <y1> must be 0 to %u\n", PX_GFX_DISP_SIZE_Y - 1);
        return NULL;
    }
    y1 = px_cli_argv_val.u8;

    // <x2>
    if(!px_cli_util_argv_to_u8(2, 0, PX_GFX_DISP_SIZE_X - 1))
    {
        printf("Error. <x2> must be 0 to %u\n", PX_GFX_DISP_SIZE_X - 1);
        return NULL;
    }
    x2 = px_cli_argv_val.u8;

    // <y2>
    if(!px_cli_util_argv_to_u8(3, 0, PX_GFX_DISP_SIZE_Y - 1))
    {
        printf("Error. <y2> must be 0 to %u\n", PX_GFX_DISP_SIZE_Y - 1);
        return NULL;
    }
    y2 = px_cli_argv_val.u8;

    // [color:0|1|i]
    if(argc > 4)
    {
        switch(argv[4][0])
        {
        case '0':
            color = PX_GFX_COLOR_OFF;
            break;
        case '1':
            color = PX_GFX_COLOR_ON;
            break;
        case 'i':
            color = PX_GFX_COLOR_INVERT;
            break;
        default:
            return "Error. [color:0|1|i] must be 0 or 1 or i (invert)";
        }
    }
    else
    {
        color = PX_GFX_COLOR_ON;
    }

    px_gfx_color_fg_set(color);
    px_gfx_draw_line(x1, y1, x2, y2);
    px_gfx_draw_update();

    return NULL;
}

static const char* px_cli_cmd_fn_lcd_rect(uint8_t argc, char* argv[])
{
    px_gfx_xy_t     x;
    px_gfx_xy_t     y;
    px_gfx_xy_t     w;
    px_gfx_xy_t     h;
    px_gfx_color_t color;

    // <x>
    if(!px_cli_util_argv_to_u8(0, 0, PX_GFX_DISP_SIZE_X - 1))
    {
        printf("Error. <x> must be 0 to %u\n", PX_GFX_DISP_SIZE_X - 1);
        return NULL;
    }
    x = px_cli_argv_val.u8;

    // <y>
    if(!px_cli_util_argv_to_u8(1, 0, PX_GFX_DISP_SIZE_Y - 1))
    {
        printf("Error. <y> must be 0 to %u\n", PX_GFX_DISP_SIZE_Y - 1);
        return NULL;
    }
    y = px_cli_argv_val.u8;

    // <w>
    if(!px_cli_util_argv_to_u8(2, 1, PX_GFX_DISP_SIZE_X))
    {
        printf("Error. <w> must be 1 to %u\n", PX_GFX_DISP_SIZE_X);
        return NULL;
    }
    w = px_cli_argv_val.u8;

    // <h>
    if(!px_cli_util_argv_to_u8(3, 1, PX_GFX_DISP_SIZE_Y))
    {
        printf("Error. <h> must be 1 to %u\n", PX_GFX_DISP_SIZE_Y);
        return NULL;
    }
    h = px_cli_argv_val.u8;

    // [color:0|1|i]
    if(argc > 4)
    {
        switch(argv[4][0])
        {
        case '0':
            color = PX_GFX_COLOR_OFF;
            break;
        case '1':
            color = PX_GFX_COLOR_ON;
            break;
        case 'i':
            color = PX_GFX_COLOR_INVERT;
            break;
        default:
            return "Error. [color:0|1|i] must be 0 or 1 or i (invert)";
        }
    }
    else
    {
        color = PX_GFX_COLOR_ON;
    }

    px_gfx_color_fg_set(color);
    px_gfx_draw_rect(x, y, w, h);
    px_gfx_draw_update();

    return NULL;
}

static const char* px_cli_cmd_fn_lcd_fill(uint8_t argc, char* argv[])
{
    px_gfx_xy_t     x;
    px_gfx_xy_t     y;
    px_gfx_xy_t     w;
    px_gfx_xy_t     h;
    px_gfx_color_t color;

    // <x>
    if(!px_cli_util_argv_to_u8(0, 0, PX_GFX_DISP_SIZE_X - 1))
    {
        printf("Error. <x> must be 0 to %u\n", PX_GFX_DISP_SIZE_X - 1);
        return NULL;
    }
    x = px_cli_argv_val.u8;

    // <y>
    if(!px_cli_util_argv_to_u8(1, 0, PX_GFX_DISP_SIZE_Y - 1))
    {
        printf("Error. <y> must be 0 to %u\n", PX_GFX_DISP_SIZE_Y - 1);
        return NULL;
    }
    y = px_cli_argv_val.u8;

    // <w>
    if(!px_cli_util_argv_to_u8(2, 1, PX_GFX_DISP_SIZE_X))
    {
        printf("Error. <w> must be 1 to %u\n", PX_GFX_DISP_SIZE_X);
        return NULL;
    }
    w = px_cli_argv_val.u8;

    // <h>
    if(!px_cli_util_argv_to_u8(3, 1, PX_GFX_DISP_SIZE_Y))
    {
        printf("Error. <h> must be 1 to %u\n", PX_GFX_DISP_SIZE_Y);
        return NULL;
    }
    h = px_cli_argv_val.u8;

    // [color:0|1|i]
    if(argc > 4)
    {
        switch(argv[4][0])
        {
        case '0':
            color = PX_GFX_COLOR_OFF;
            break;
        case '1':
            color = PX_GFX_COLOR_ON;
            break;
        case 'i':
            color = PX_GFX_COLOR_INVERT;
            break;
        default:
            return "Error. [color:0|1|i] must be 0 or 1 or i (invert)";
        }
    }
    else
    {
        color = PX_GFX_COLOR_ON;
    }

    px_gfx_color_fg_set(color);
    px_gfx_draw_fill_fg(x, y, w, h);
    px_gfx_draw_update();

    return NULL;
}

static const char* px_cli_cmd_fn_lcd_circle(uint8_t argc, char* argv[])
{
    px_gfx_xy_t     x;
    px_gfx_xy_t     y;
    px_gfx_xy_t     r;
    px_gfx_color_t color;

    // <x>
    if(!px_cli_util_argv_to_u8(0, 0, PX_GFX_DISP_SIZE_X - 1))
    {
        printf("Error. <x> must be 0 to %u\n", PX_GFX_DISP_SIZE_X - 1);
        return NULL;
    }
    x = px_cli_argv_val.u8;

    // <y>
    if(!px_cli_util_argv_to_u8(1, 0, PX_GFX_DISP_SIZE_Y - 1))
    {
        printf("Error. <y> must be 0 to %u\n", PX_GFX_DISP_SIZE_Y - 1);
        return NULL;
    }
    y = px_cli_argv_val.u8;

    // <r>
    if(!px_cli_util_argv_to_u8(2, 1, PX_MAX(PX_GFX_DISP_SIZE_X, PX_GFX_DISP_SIZE_X)))
    {
        printf("Error. <w> must be 1 to %u\n", PX_MAX(PX_GFX_DISP_SIZE_X, PX_GFX_DISP_SIZE_X));
        return NULL;
    }
    r = px_cli_argv_val.u8;

    // [color:0|1|i]
    if(argc > 3)
    {
        switch(argv[3][0])
        {
        case '0':
            color = PX_GFX_COLOR_OFF;
            break;
        case '1':
            color = PX_GFX_COLOR_ON;
            break;
        case 'i':
            color = PX_GFX_COLOR_INVERT;
            break;
        default:
            return "Error. [color:0|1|i] must be 0 or 1 or i (invert)";
        }
    }
    else
    {
        color = PX_GFX_COLOR_ON;
    }

    px_gfx_color_fg_set(color);
    px_gfx_draw_circ(x, y, r);
    px_gfx_draw_update();

    return NULL;
}

static const char* px_cli_cmd_fn_lcd_str(uint8_t argc, char* argv[])
{
    px_gfx_xy_t     x;
    px_gfx_xy_t     y;
    px_gfx_color_t color;

    // <x>
    if(!px_cli_util_argv_to_u8(0, 0, PX_GFX_DISP_SIZE_X - 1))
    {
        printf("Error. <x> must be 0 to %u\n", PX_GFX_DISP_SIZE_X - 1);
        return NULL;
    }
    x = px_cli_argv_val.u8;

    // <y>
    if(!px_cli_util_argv_to_u8(1, 0, PX_GFX_DISP_SIZE_Y - 1))
    {
        printf("Error. <y> must be 0 to %u\n", PX_GFX_DISP_SIZE_Y - 1);
        return NULL;
    }
    y = px_cli_argv_val.u8;

    // [color:0|1|i]
    if(argc > 3)
    {
        switch(argv[3][0])
        {
        case '0':
            color = PX_GFX_COLOR_OFF;
            break;
        case '1':
            color = PX_GFX_COLOR_ON;
            break;
        case 'i':
            color = PX_GFX_COLOR_INVERT;
            break;
        default:
            return "Error. [color:0|1|i] must be 0 or 1 or i (invert)";
        }
    }
    else
    {
        color = PX_GFX_COLOR_ON;
    }

    px_gfx_color_fg_set(color);
    px_gfx_draw_str(x, y, argv[2]);
    px_gfx_draw_update();

    return NULL;
}

// Create CLI command structures
PX_CLI_CMD_CREATE(px_cli_cmd_lcd_clr,       "clr",  0, 0,   "",                             "Clear LCD display")
PX_CLI_CMD_CREATE(px_cli_cmd_lcd_backlight, "bl",   1, 1,   "<on|off>",                     "Enable/disable LCD backlight")
PX_CLI_CMD_CREATE(px_cli_cmd_lcd_pixel,     "p",    2, 3,   "<x> <y> [color:0|1|i]",        "Draw pixel")
PX_CLI_CMD_CREATE(px_cli_cmd_lcd_line,      "l",    4, 5,   "<x1> <y1> <x2> <y2> [color]",  "Draw line")
PX_CLI_CMD_CREATE(px_cli_cmd_lcd_rect,      "r",    4, 5,   "<x> <y> <w> <h> [color]",      "Draw rectangle")
PX_CLI_CMD_CREATE(px_cli_cmd_lcd_fill,      "f",    4, 5,   "<x> <y> <w> <h> [color]",      "Draw fill")
PX_CLI_CMD_CREATE(px_cli_cmd_lcd_circle,    "c",    3, 4,   "<x> <y> <r> [color]",          "Draw circle")
PX_CLI_CMD_CREATE(px_cli_cmd_lcd_str,       "s",    3, 4,   "<x> <y> <\"str\"> [color]",    "Draw string")

PX_CLI_GROUP_CREATE(px_cli_group_lcd, "lcd")
    PX_CLI_CMD_ADD(px_cli_cmd_lcd_clr,          px_cli_cmd_fn_lcd_clr)
    PX_CLI_CMD_ADD(px_cli_cmd_lcd_backlight,    px_cli_cmd_fn_lcd_backlight)
    PX_CLI_CMD_ADD(px_cli_cmd_lcd_pixel,        px_cli_cmd_fn_lcd_pixel)
    PX_CLI_CMD_ADD(px_cli_cmd_lcd_line,         px_cli_cmd_fn_lcd_line)
    PX_CLI_CMD_ADD(px_cli_cmd_lcd_rect,         px_cli_cmd_fn_lcd_rect)
    PX_CLI_CMD_ADD(px_cli_cmd_lcd_fill,         px_cli_cmd_fn_lcd_fill)
    PX_CLI_CMD_ADD(px_cli_cmd_lcd_circle,       px_cli_cmd_fn_lcd_circle)
    PX_CLI_CMD_ADD(px_cli_cmd_lcd_str,          px_cli_cmd_fn_lcd_str)
PX_CLI_GROUP_END()
