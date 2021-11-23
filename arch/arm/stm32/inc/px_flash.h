#ifndef __PX_FLASH_H__
#define __PX_FLASH_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_flash.h : Internal FLASH write routines
    Author(s):      Pieter Conradie
    Creation Date:  2018-11-21

============================================================================= */

/** 
 *  @ingroup STM32
 *  @defgroup STM32_FLASH px_flash.h : Internal FLASH write routines
 *  
 *  This module can erase a page and write a half page of internal FLASH.
 *  
 *  File(s):
 *  - arch/arm/stm32/inc/px_flash.h
 *  - arch/arm/stm32/src/px_flash.c 
 *   
 *  @note 
 *  The flash erase and program functions must be executed from SRAM, not FLASH.
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"
#include "px_stm32cube.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
#define PX_FLASH_BASE_ADR                 FLASH_BASE

#if STM32G0
#define PX_FLASH_PAGE_SIZE                2048
#define PX_FLASH_ROW_SIZE                 256
#define PX_FLASH_ROW_SIZE_WORDS           (PX_FLASH_ROW_SIZE / 4)
#define PX_FLASH_ROW_SIZE_DOUBLE_WORDS    (PX_FLASH_ROW_SIZE / 8)
#endif

#if STM32L0
#define PX_FLASH_PAGE_SIZE                128
#define PX_FLASH_HALF_PAGE_SIZE           (PX_FLASH_PAGE_SIZE / 2)
#define PX_FLASH_HALF_PAGE_SIZE_WORDS     (PX_FLASH_HALF_PAGE_SIZE / 4)
#endif

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Unlock FLASH for programming and erase.
 *  
 */
void px_flash_unlock(void);

/**
 *  Lock FLASH to prevent programming or erase.
 *  
 */
void px_flash_lock(void);

/**
 *  Function to erase a page.
 *  
 *  @param address  Page address
 */
void px_flash_erase_page(const uint32_t address);

#if STM32L0
/**
 *  Function to write a half page.
 *  
 *  @param address   Half page address
 *  @param data      Pointer to buffer containing data to write
 */
void px_flash_wr_half_page(const uint32_t address, const  uint32_t * data);
#endif

#if STM32G0
/**
 *  Function to write a row.
 *  
 *  @param address   Row address
 *  @param data      Pointer to buffer containing data to write
 */
void px_flash_wr_row(const uint32_t address, const  uint32_t * data);
#endif

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
