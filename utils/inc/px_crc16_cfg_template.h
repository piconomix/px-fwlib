#ifndef __PX_CRC16_CFG_H__
#define __PX_CRC16_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          px_crc16_cfg.h : 16-bit CRC calculator configuration
    Author(s):      Pieter Conradie
    Creation Date:  2019-08-05

============================================================================= */

/** 
 *  @addtogroup PX_CRC16
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

/* _____DEFINITIONS__________________________________________________________ */
/// Generate table in RAM to speed up CRC calculation
#define PX_CRC16_RAM_TABLE  0

/// Use table in ROM to speed up CRC calculation
#define PX_CRC16_ROM_TABLE  0

/// @}
#endif
