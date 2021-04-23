#ifndef __PX_DEBOUNCE_CFG_H__
#define __PX_DEBOUNCE_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2015 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          px_debounce_cfg.h : Digital debounce module configuration
    Author(s):      Pieter Conradie
    Creation Date:  2015-08-21

============================================================================= */

/** 
 *  @addtogroup PX_DEBOUNCE
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

/* _____DEFINITIONS__________________________________________________________ */
/// Max HI counter value
#define PX_DEBOUNCE_CFG_COUNT_MAX  10

/// Low watermark threshold for valid LO
#define PX_DEBOUNCE_CFG_THRESHOLD_LO 2

/// High watermark threshold for valid HI
#define PX_DEBOUNCE_CFG_THRESHOLD_HI 8

/// Count threshold for long LO/ HI; Set to 0 to disable option
#define PX_DEBOUNCE_CFG_LONG_COUNT 20

/// @}
#endif
