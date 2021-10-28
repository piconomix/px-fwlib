#ifndef __PX_EEPROM_H__
#define __PX_EEPROM_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_eeprom.h : Internal EEPROM write routines
    Author(s):      Pieter Conradie
    Creation Date:  2018-11-21

============================================================================= */

/** 
 *  @ingroup STM32
 *  @defgroup STM32_EEPROM px_eeprom.h : Internal EEPROM write routines
 *  
 *  This module can erase a page and write a half page of internal EEPROM.
 *  
 *  File(s):
 *  - arch/arm/stm32/inc/px_eeprom.h
 *  - arch/arm/stm32/src/px_eeprom.c 
 *   
 *  @note 
 *  The flash erase and program functions must be executed from SRAM, not EEPROM.
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_stm32cube.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
/// Start address of EEPROM
#define PX_EEPROM_BASE_ADR      DATA_EEPROM_BASE

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Unlock EEPROM for programming and erase.
 *  
 */
void px_eeprom_unlock(void);

/**
 *  Lock EEPROM to prevent programming or erase.
 *  
 */
void px_eeprom_lock(void);

/**
 *  Function to erase a word (not really required).
 *  
 *  @param address  Page address
 */
void px_eeprom_erase_word(const uint32_t address);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
