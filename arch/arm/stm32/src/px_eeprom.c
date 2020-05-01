/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          px_eeprom.h : Internal EEPROM write routines
    Author(s):      Pieter Conradie
    Creation Date:  2018-11-21

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_eeprom.h"
#include "px_lib_stm32cube.h"
#include "px_dbg.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_DBG_DECL_NAME("px_eeprom");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
PX_ATTR_RAMFUNC void px_eeprom_unlock(void)
{
    uint32_t primask;

    // Save interrupt status
    primask = __get_PRIMASK();
    // Disable interrupts
    px_interrupts_disable();
    // Unlock PELOCK
    FLASH->PEKEYR = 0x89abcdef;
    FLASH->PEKEYR = 0x02030405;
    // Restore interrupt status
    __set_PRIMASK(primask);
}

PX_ATTR_RAMFUNC void px_eeprom_lock(void)
{
    uint32_t primask;

    // Save interrupt status
    primask = __get_PRIMASK();
    // Disable interrupts
    px_interrupts_disable();
    // Lock PELOCK
    FLASH->PECR |= FLASH_PECR_PELOCK;
    // Restore interrupt status
    __set_PRIMASK(primask);
}

PX_ATTR_RAMFUNC void px_eeprom_erase_word(const uint32_t address)
{
    uint32_t primask;

    // Save interrupt status
    primask = __get_PRIMASK();
    // Disable interrupts
    px_interrupts_disable();
    // Enable page erase
    FLASH->PECR |= (FLASH_PECR_ERASE | FLASH_PECR_DATA);
    // Write 32-bit zero value to specified address
    *(uint32_t *)address = (uint32_t)0;
    // Wait until erase has finished (not busy)
    while ((FLASH->SR & FLASH_SR_BSY) != 0)
    {
        continue;
    }
    // EOP (End Of Programming) Flag set?
    if((FLASH->SR & FLASH_SR_EOP) != 0)
    {
        // Reset EOP flag
        FLASH->SR = FLASH_SR_EOP;
    }
    // Disable page erase
    FLASH->PECR &= ~(FLASH_PECR_ERASE | FLASH_PECR_DATA);
    // Restore interrupt status
    __set_PRIMASK(primask);
}
