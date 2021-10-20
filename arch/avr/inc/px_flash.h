#ifndef __PX_FLASH_H__
#define __PX_FLASH_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2006 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_flash.h : Internal FLASH write routines
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31

============================================================================= */

/** 
 *  @ingroup AVR
 *  @defgroup AVR_FLASH px_flash.h : Internal FLASH write routines
 *  
 *  This component can clear and write a page of internal FLASH.
 *  
 *  File(s):
 *  - arch/avr/inc/px_flash.h
 *  - arch/avr/src/px_flash.c
 *  
 *  @warn_s
 *  These routines must always reside in the bootloader section. If mixing
 *  application code and bootloader code, the prefix BOOTLOADER_SECTION can be
 *  used to place these functions in a section called ".bootloader".
 *  See <avr/boot.h>
 *  @warn_e
 *  
 *  Example of setting ".bootloader" section address in Makefile:
 *  
 *      LDFLAGS += -Wl,--section-start,.bootloader=0x1FF00
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <avr/io.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
/// FLASH Page size in bytes
#define PX_FLASH_PAGE_SIZE  SPM_PAGESIZE

/// Total number of FLASH pages (application and bootloader section)
#define PX_FLASH_NR_OF_PAGES ((PX_FLASHEND + 1) / PX_FLASH_PAGE_SIZE)

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Function to erase a page.
 *  
 *  @param page  0 to (#PX_FLASH_NR_OF_PAGES - 1)
 */
void px_flash_erase_page(const uint16_t page);

/**
 *  Function to write a page.
 *  
 *  @param page      0 to (#PX_FLASH_NR_OF_PAGES - 1)
 *  @param data      Pointer to buffer containing data to write
 */
void px_flash_wr_page(const uint16_t page, const  uint8_t * data);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
