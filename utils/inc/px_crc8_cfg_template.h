#ifndef __PX_CRC8_CFG_H__
#define __PX_CRC8_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2020 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          px_crc8.h : 8-bit CRC calculator configuration
    Author(s):      Pieter Conradie
    Creation Date:  2020-10-23

============================================================================= */

/**
 *  @addtogroup PX_CRC8
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"

/* _____DEFINITIONS__________________________________________________________ */
/// Generate table in RAM to speed up CRC calculation
#define PX_CRC8_RAM_TABLE  0

/// Use table in ROM to speed up CRC calculation
#define PX_CRC8_ROM_TABLE  0

/// @}
#endif
