#ifndef __PX_LCD_ST7565P_TSE2G0330E_CFG_H__
#define __PX_LCD_ST7565P_TSE2G0330E_CFG_H__
/* =============================================================================
     _____   _____   _____   ____    _   _    ____    __  __   _____  __   __
    |  __ \ |_   _| / ____| / __ \  | \ | |  / __ \  |  \/  | |_   _| \ \ / /
    | |__) |  | |  | |     | |  | | |  \| | | |  | | | \  / |   | |    \ V /
    |  ___/   | |  | |     | |  | | | . ` | | |  | | | |\/| |   | |     > < 
    | |      _| |_ | |____ | |__| | | |\  | | |__| | | |  | |  _| |_   / . \
    |_|     |_____| \_____| \____/  |_| \_|  \____/  |_|  |_| |_____| /_/ \_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.
 
    Title:          px_lcd_st7565p_tse2g0330e.h : Truly TSE2G0330-E 128x64 monochrome LCD wtih Sitronix ST7565P driver configuration
    Author(s):      Pieter Conradie
    Creation Date:  2018-09-10

============================================================================= */

/** 
 *  @addtogroup PX_LCD_ST7565P_TSE2G0330E
 */ 
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_board.h"

/* _____DEFINITIONS__________________________________________________________ */
/// Macro to set LCD reset pin low
#define PX_LCD_CFG_RST_LO()     px_gpio_pin_set_lo(&px_gpio_lcd_rst)

/// Macro to set LCD reset pin high
#define PX_LCD_CFG_RST_HI()     px_gpio_pin_set_hi(&px_gpio_lcd_rst)

/// Macro to set RS pin low
#define PX_LCD_CFG_RS_LO()      px_gpio_pin_set_lo(&px_gpio_lcd_rs)

/// Macro to set RS pin high
#define PX_LCD_CFG_RS_HI()      px_gpio_pin_set_hi(&px_gpio_lcd_rs)

/// Configuration option to rotate mapping of LCD by 180 degrees
#define PX_LCD_CFG_ROT_180_DEG  1

/// @}
#endif // #ifndef __PX_LCD_ST7565P_TSE2G0330E_CFG_H__
