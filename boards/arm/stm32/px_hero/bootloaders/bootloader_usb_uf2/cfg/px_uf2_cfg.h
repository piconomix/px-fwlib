#ifndef __PX_UF2_CFG_H__
#define __PX_UF2_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
 
    Title:          px_uf2.h : Microsoft UF2 bootloader over USB MSC (Mass Storage Class) configuration
    Author(s):      Pieter Conradie
    Creation Date:  2019-05-25

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */

/* _____DEFINITIONS__________________________________________________________ */
/// Start address of flash to write to (reserved space for bootloader)
#define PX_UF2_CFG_FLASH_START_ADR  0x00004000

/// Flash size
#define PX_UF2_CFG_FLASH_SIZE       0x00020000

/// Info file version text
#define PX_UF2_CFG_INFO_VERSION     "v1.0.0 F"

/// Info file model text
#define PX_UF2_CFG_INFO_MODEL       "Piconomix PX-HERO"

/// Info file board ID text
#define PX_UF2_CFG_INFO_BOARD_ID    "STM32L072RB-PXHERO-v1"

/// index.htm file URL
#define PX_UF2_CFG_INDEX_URL        "https://piconomix.com/px-fwlib/index.html"

/// FAT16 volume label
#define PX_UF2_CFG_VOLUME_LABEL     "HERO-BOOT "

/// Family ID
#define PX_UF2_CFG_FAMILY_ID        0xe892273c

#endif
