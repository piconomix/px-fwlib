#ifndef __PX_EXTI_CFG_H__
#define __PX_EXTI_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_exti_cfg.h : EXTI Peripheral Driver configuration
    Author(s):      Pieter Conradie
    Creation Date:  2020-04-13

============================================================================= */

/** 
 *  @addtogroup STM32_EXTI
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"

/* _____DEFINITIONS__________________________________________________________ */
/// @name Individual enable/disable of each external interrupt (0 to 15)
/// @{
#define PX_EXTI_CFG_INT0_EN      1
#define PX_EXTI_CFG_INT1_EN      1
#define PX_EXTI_CFG_INT2_EN      1
#define PX_EXTI_CFG_INT3_EN      1
#define PX_EXTI_CFG_INT4_EN      1
#define PX_EXTI_CFG_INT5_EN      1
#define PX_EXTI_CFG_INT6_EN      1
#define PX_EXTI_CFG_INT7_EN      1
#define PX_EXTI_CFG_INT8_EN      1
#define PX_EXTI_CFG_INT9_EN      1
#define PX_EXTI_CFG_INT10_EN     1
#define PX_EXTI_CFG_INT11_EN     1
#define PX_EXTI_CFG_INT12_EN     1
#define PX_EXTI_CFG_INT13_EN     1
#define PX_EXTI_CFG_INT14_EN     1
#define PX_EXTI_CFG_INT15_EN     1
/// @}

/// @}
#endif
