#ifndef __PX_AT45D_CFG_H__
#define __PX_AT45D_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2014 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          px_at45d_cfg.h : AT45D Peripheral Driver configuration
    Author(s):      Pieter Conradie
    Creation Date:  2014-01-17

============================================================================= */

/** 
 *  @addtogroup PX_AT45D
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

/* _____DEFINITIONS__________________________________________________________ */
/// Specify device
#define PX_AT45D_CFG_DEVICE    PX_AT45DB041

/**
 *  Select power of two page size.
 *  
 *  "Power of 2" binary page size Configuration Register is a user-programmable
 *  nonvolatile register that allows the page size of the main memory to be
 *  configured for binary page size or the DataFlash standard page size.
 *  
 *  See px_at45d_set_page_size_to_pwr_of_two().
 */
#define PX_AT45D_CFG_PWR_OF_TWO_PAGE_SIZE    0

/// @}
#endif // #ifndef __PX_AT45D_CFG_H__
