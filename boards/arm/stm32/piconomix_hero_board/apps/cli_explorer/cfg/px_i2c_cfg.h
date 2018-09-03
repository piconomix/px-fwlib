#ifndef __PX_I2C_CFG_H__
#define __PX_I2C_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

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
    
    Title:          px_i2c_cfg.h : I2C Peripheral Driver configuration
    Author(s):      Pieter Conradie
    Creation Date:  2018-03-29

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_gpio.h"
#include "px_board.h"

/* _____DEFINITIONS__________________________________________________________ */
/// Enable/disable support for I2C1 peripheral
#define PX_I2C_CFG_I2C1_EN 1

/// Enable/disable support for I2C2 peripheral
#define PX_I2C_CFG_I2C2_EN 0

/**
 *  I2C peripheral timing register value
 *  Calculated using STM32CubeMX graphical tool:
 *  - Speed Frequency = 100 kHz
 *  - Rise Time = 100 ns
 *  - Fall Time = 10 ns
 *  - Analog Filter = Enabled
 *  
 *  Reference:
 *  [AN4235 I2C timing configuration tool for STM32F3xxxx and ST M32F0xxxx microcontrollers](www.st.com/resource/en/application_note/dm00074956.pdf)
 */
#define PX_I2C_CFG_TIMINGR 0x20302e37

#endif // #ifndef __PX_I2C_CFG_H__
