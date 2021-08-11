#ifndef __PX_DAC_CFG_H__
#define __PX_DAC_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_dac_cfg.h : DAC Peripheral Driver configuration
    Author(s):      Pieter Conradie
    Creation Date:  2018-11-15

============================================================================= */

/** 
 *  @addtogroup STM32_DAC
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

/* _____DEFINITIONS__________________________________________________________ */
/// Enable/disable support for DAC1 peripheral
#define PX_DAC_CFG_DAC1_EN      1

/// Enable/disable support for DAC1 Channel 1
#define PX_DAC_CFG_DAC1_CH1_EN  1

/// Enable/disable support for DAC1 Channel 2
#define PX_DAC_CFG_DAC1_CH2_EN  0

/// @}
#endif
