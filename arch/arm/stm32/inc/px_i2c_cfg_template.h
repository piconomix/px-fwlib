#ifndef __PX_I2C_CFG_H__
#define __PX_I2C_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_i2c_cfg.h : I2C Peripheral Driver configuration
    Author(s):      Pieter Conradie
    Creation Date:  2018-03-29

============================================================================= */

/** 
 *  @addtogroup STM32_I2C
 */
/// @{

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
 */
#define PX_I2C_CFG_TIMINGR 0x20302e37

/// @}
#endif
