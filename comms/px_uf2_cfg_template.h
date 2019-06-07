#ifndef __PX_UF2_CFG_H__
#define __PX_UF2_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.
 
    Title:          px_uf2.h : Microsoft UF2 bootloader over USB MSD (Mass Storage Device) configuration
    Author(s):      Pieter Conradie
    Creation Date:  2019-05-25

============================================================================= */

/** 
 *  @addtogroup PX_UF2
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */

/* _____DEFINITIONS__________________________________________________________ */
/// Start address of flash to write to (reserved space for bootloader)
#define PX_UF2_CFG_FLASH_START_ADR  0x00004000

/// Flash size
#define PX_UF2_CFG_FLASH_SIZE       0x00020000

/// Info file version text
#define PX_UF2_CFG_INFO_VERSION     "1.0.0 F"

/// Info file model text
#define PX_UF2_CFG_INFO_MODEL       "PX-HERO"

/// Info file board ID text
#define PX_UF2_CFG_INFO_BOARD_ID    "STM32L072RB"

/// index.htm file URL
#define PX_UF2_CFG_INDEX_URL        "https://piconomix.com/fwlib/index.html"

/// Family ID
#define PX_UF2_CFG_FAMILY_ID        0xe892273c
/// @}
#endif // #ifndef __PX_UF2_CFG_H__
