#ifndef __PX_EEPROM_H__
#define __PX_EEPROM_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
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
    
    Title:          px_eeprom.h : Internal EEPROM write routines
    Author(s):      Pieter Conradie
    Creation Date:  2018-11-21

============================================================================= */

/** 
 *  @ingroup STM32
 *  @defgroup AVR_EEPROM px_eeprom.h : Internal EEPROM write routines
 *  
 *  This module can erase a page and write a half page of internal EEPROM.
 *  
 *  File(s):
 *  - arch/arm/stm32/px_eeprom.h
 *  - arch/arm/stm32/px_eeprom.c 
 *   
 *  @note 
 *  The flash erase and program functions must be executed from SRAM, not EEPROM.
 *  
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_lib_stm32cube.h"

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

/// @}
#ifdef __cplusplus
}
#endif

#endif // #ifndef __PX_EEPROM_H__
