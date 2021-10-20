#ifndef __PX_UF2_H__
#define __PX_UF2_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
 
    Title:          px_uf2.h : Microsoft UF2 bootloader over USB MSC (Mass Storage Class)
    Author(s):      Pieter Conradie
    Creation Date:  2019-05-25

============================================================================= */

/** 
 *  @ingroup COMMS
 *  @defgroup PX_UF2 px_uf2.h : Microsoft UF2 bootloader over USB MSC (Mass Storage Class)
 *  
 *  Microsoft UF2 bootloader over USB MSD (Mass Storage Device).
 *  
 *  File(s):
 *  - comms/inc/px_uf2.h 
 *  - comms/inc/px_uf2_cfg_template.h 
 *  - comms/src/px_uf2.c
 *  
 *  References: 
 *  - [One chip to flash them all] (https://makecode.com/blog/one-chip-to-flash-them-all) 
 *  - [USB Flashing Format (UF2)] (https://github.com/Microsoft/uf2) 
 *  - [UF2 Bootloader] (https://github.com/Microsoft/uf2-samdx1) 
 *  - [Microsoft Extensible Firmware Initiative FAT32 File System Specification 1.03] (https://download.microsoft.com/download/1/6/1/161ba512-40e2-4cc9-843a-923143f3456c/fatgen103.doc) 
 *   
 *  This module implements the Microsoft UF2 bootloader communication layer over 
 *  USB MSD (Mass Storage Device). It emulates (fakes) a USB storage device with 
 *  a FAT16 file system to provide info when reading files and writes a new 
 *  app to FLASH if it is formatted using the UF2 tool. 
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

// Include project specific configuration. See "px_uf2_cfg_template.h"
#include "px_uf2_cfg.h"

// Check that all project specific options have been correctly specified
#if (   !defined(PX_UF2_CFG_FLASH_START_ADR) \
     || !defined(PX_UF2_CFG_FLASH_SIZE     ) \
     || !defined(PX_UF2_CFG_INFO_VERSION   ) \
     || !defined(PX_UF2_CFG_INFO_MODEL     ) \
     || !defined(PX_UF2_CFG_INFO_BOARD_ID  ) \
     || !defined(PX_UF2_CFG_INDEX_URL      ) \
     || !defined(PX_UF2_CFG_VOLUME_LABEL   ) \
     || !defined(PX_UF2_CFG_FAMILY_ID      )  )
#error "One or more options not defined in 'px_uf2_cfg.h'"
#endif

#ifdef __cplusplus
extern "C"
{
#endif
/* _____DEFINITIONS__________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */
/**
 *  Definition of a pointer to a function that will be called to write a block 
 *  of data to flash.
 *  
 *  @param data         Pointer to buffer containing data to write
 *  @param adr          Start address to write to
 *  @param nr_of_bytes  Number of bytes to write
 */
typedef void (*px_uf2_on_wr_flash_block_t)(const uint8_t * data, 
                                           uint32_t        adr, 
                                           size_t          nr_of_bytes);

/**
 *  Definition of a pointer to a function that will be called when last block
 *  has been written. 
 */
typedef void (*px_uf2_on_wr_flash_done_t)(void);

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise UF2 module.
 *  
 *  @param on_wr_flash_block    Pointer to function that will be called to write
 *                              a block to FLASH.
 *  @param on_wr_flash_done     Pointer to a function that will be called when 
 *                              the whole UF2 file has been written.
 */
void px_uf2_init(px_uf2_on_wr_flash_block_t on_wr_flash_block,
                 px_uf2_on_wr_flash_done_t  on_wr_flash_done);

/**
 *  Function that must be called when a 512 byte sector is read over USB Mass
 *  Storage.
 *  
 *  @param sector_adr   Sector address to read
 *  @param buf          Pointer to buffer to copy sector data into
 */
void px_uf2_on_rd_sector(uint32_t sector_adr, uint8_t * buf);

/**
 *  Function that must be called when a 512 byte sector is written over USB Mass
 *  Storage.
 *  
 *  @param sector_adr   Sector address to write to
 *  @param buf          Pointer to buffer containing data to write
 */
void px_uf2_on_wr_sector(uint32_t sector_adr, const uint8_t * buf);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/* 
    This work is based on https://github.com/Microsoft/uf2-samdx1
 
    LICENSE:
 
    The MIT License (MIT)

    Copyright (c) Microsoft

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

    Third Party Programs: The software may include third party programs that
    Microsoft, not the third party, licenses to you under this agreement.
    Notices, if any, for the third party programs are included for your
    information only.
 */

/// @}
#endif
