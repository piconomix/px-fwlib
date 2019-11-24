#ifndef __PX_GFX_CFG_H__
#define __PX_GFX_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          px_gfx_cfg.h : Simple monochrome graphics library config
    Author(s):      Pieter Conradie
    Creation Date:  2018-03-29

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

/// Display size X in pixels
#define PX_GFX_DISP_SIZE_X          128

/// Display size Y in pixels
#define PX_GFX_DISP_SIZE_Y          64

/// Maximum allocated string size
#define PX_GFX_CFG_STR_BUFFER_SIZE  32

/// Default font
#define PX_GFX_CFG_DEFAULT_FONT     px_gfx_font_5x7

/* _____DEFINITIONS__________________________________________________________ */

#endif // #ifndef __PX_GFX_CFG_H__
