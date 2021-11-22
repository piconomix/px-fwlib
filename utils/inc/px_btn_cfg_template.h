#ifndef __PX_BTN_CFG_H__
#define __PX_BTN_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2021 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          px_btn.h : Digital button input configuration
    Author(s):      Pieter Conradie
    Creation Date:  2021-11-22

============================================================================= */

/** 
 *  @addtogroup PX_BTN
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

/* _____DEFINITIONS__________________________________________________________ */
/// Max HI counter value (counter starts at 0)
#define PX_BTN_CFG_COUNT_MAX        9

/// Low threshold for valid LO
#define PX_BTN_CFG_THRESHOLD_LO     2

/// High threshold for valid HI
#define PX_BTN_CFG_THRESHOLD_HI     7

/// Count threshold for long LO/ HI; Set to 0 to disable option
#define PX_BTN_CFG_LONG_COUNT       300

/// Maximum number of button clicks to detect; Minimum value is 1
#define PX_BTN_CFG_CLICKS_MAX       2

/// A button press count equal or longer than this value is considered a click
#define PX_BTN_CFG_CLICK_WIN_MIN    5

/// A button press count equal or shorter than this value is considered a click
#define PX_BTN_CFG_CLICK_WIN_MAX    20

/// @}
#endif
