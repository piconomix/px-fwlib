#ifndef __PX_I2C_CFG_H__
#define __PX_I2C_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2014 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_i2c_cfg.h : I2C Peripheral Driver configuration
    Author(s):      Pieter Conradie
    Creation Date:  2014-01-16

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_gpio.h"
#include "px_board.h"

/* _____DEFINITIONS__________________________________________________________ */
/// Enable/disable support for I2C0 peripheral
#define PX_I2C_CFG_I2C0_EN 1

/// I2C Bit rate in Hz
#define PX_I2C_CFG_BIT_RATE_HZ 100000ul

/// [px_i2c_twi.c] TWI clock prescaler (sized according to desired TWI and microcontroller clock)
#define PX_I2C_CFG_TWI_PRESCALER_VAL   1

/// @name I2C Clock GPIO pin manipulation macros
//@{
#define PX_I2C_CFG_SCL_SET_HIZ()   PX_GPIO_DIR_SET_IN(PX_GPIO_I2C_SCL)
#define PX_I2C_CFG_SCL_SET_LO()    PX_GPIO_DIR_SET_OUT(PX_GPIO_I2C_SCL)
#define PX_I2C_CFG_SCL_IS_HI()     PX_GPIO_PIN_IS_HI(PX_GPIO_I2C_SCL)
#define PX_I2C_CFG_SCL_IS_LO()     PX_GPIO_PIN_IS_LO(PX_GPIO_I2C_SCL)
//@}

/// @name I2C Data GPIO pin manipulation macros
//@{
#define PX_I2C_CFG_SDA_SET_HIZ()   PX_GPIO_DIR_SET_IN(PX_GPIO_I2C_SDA)
#define PX_I2C_CFG_SDA_SET_LO()    PX_GPIO_DIR_SET_OUT(PX_GPIO_I2C_SDA)
#define PX_I2C_CFG_SDA_IS_HI()     PX_GPIO_PIN_IS_HI(PX_GPIO_I2C_SDA)
#define PX_I2C_CFG_SDA_IS_LO()     PX_GPIO_PIN_IS_LO(PX_GPIO_I2C_SDA)
//@}

#endif
