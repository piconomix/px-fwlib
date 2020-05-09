/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          px_flash.h : Internal FLASH write routines
    Author(s):      Pieter Conradie
    Creation Date:  2018-11-21

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_flash.h"
#include "px_lib_stm32cube.h"
#include "px_dbg.h"

#ifdef STM32G0
#warning "Flash driver needs porting and testing"
#endif

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_DBG_DECL_NAME("px_flash");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
PX_ATTR_RAMFUNC void px_flash_unlock(void)
{
    uint32_t primask;

    // Save interrupt status
    primask = __get_PRIMASK();
    // Disable interrupts
    px_interrupts_disable();
#if STM32G0
    // Unlock
    FLASH->KEYR = 0x45670123;
    FLASH->KEYR = 0xcdef89ab;
#else
    // Unlock PELOCK
    FLASH->PEKEYR = 0x89abcdef;
    FLASH->PEKEYR = 0x02030405;
    // Unlock PRGLOCK
    FLASH->PRGKEYR = 0x8c9daebf;
    FLASH->PRGKEYR = 0x13141516;
#endif
    // Restore interrupt status
    __set_PRIMASK(primask);
}

PX_ATTR_RAMFUNC void px_flash_lock(void)
{
    uint32_t primask;

    // Save interrupt status
    primask = __get_PRIMASK();
    // Disable interrupts
    px_interrupts_disable();
#if STM32G0
    // Lock
    FLASH->CR |= FLASH_CR_LOCK;
#else
    // Lock PRGLOCK
    FLASH->PECR |= FLASH_PECR_PRGLOCK;
    // Lock PELOCK
    FLASH->PECR |= FLASH_PECR_PELOCK;
#endif
    // Restore interrupt status
    __set_PRIMASK(primask);
}


#if STM32L0
PX_ATTR_RAMFUNC void px_flash_erase_page(const uint32_t address)
{
    uint32_t primask;

    // Save interrupt status
    primask = __get_PRIMASK();
    // Disable interrupts
    px_interrupts_disable();
    
    // Enable page erase
    FLASH->PECR |= (FLASH_PECR_PROG | FLASH_PECR_ERASE);

    // Write 32-bit value in page to start erase sequence
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
    FLASH->PECR &= ~(FLASH_PECR_ERASE | FLASH_PECR_PROG);

    // Restore interrupt status
    __set_PRIMASK(primask);
}

PX_ATTR_RAMFUNC void px_flash_wr_half_page(const uint32_t address, const uint32_t * data)
{
    uint8_t i;
    uint32_t primask;

    // Save interrupt status
    primask = __get_PRIMASK();
    // Disable interrupts
    px_interrupts_disable();
    // Enable half page programming
    FLASH->PECR |= (FLASH_PECR_PROG | FLASH_PECR_FPRG);
    // Write half page by copying data (32-bits at a time)
    // The same address can be used as it is incremented internally
    for (i = 0; i < PX_FLASH_HALF_PAGE_SIZE_WORDS; i++)
    {
        *(uint32_t*)(address) = *data++;
    }
    // Wait until programming has finished (not busy)
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
    // Disable half page programming
    FLASH->PECR &= ~(FLASH_PECR_PROG | FLASH_PECR_FPRG);
    // Restore interrupt status
    __set_PRIMASK(primask);
}
#endif

#if STM32G0
PX_ATTR_RAMFUNC void px_flash_erase_page(const uint32_t address)
{
    uint32_t primask;
    uint32_t flash_cr;
    uint8_t  page;

    // Save interrupt status
    primask = __get_PRIMASK();
    // Disable interrupts
    px_interrupts_disable();
    
    // Set page to erase
    flash_cr   = FLASH->CR & ~FLASH_CR_PNB;
    page       = (address >> 11) & 0x3f;
    flash_cr  |= page << FLASH_CR_PNB_Pos;
    FLASH->CR  = flash_cr;        
    // Enable page erase
    FLASH->CR |= FLASH_CR_PER;
    // Start page erase
    FLASH->CR |= FLASH_CR_STRT;    
    // Wait until erase has finished (not busy)
    while ((FLASH->SR & FLASH_SR_BSY1) != 0)
    {
        continue;
    }
    // Disable page erase
    FLASH->CR &= ~FLASH_CR_PER;

    // Restore interrupt status
    __set_PRIMASK(primask);
}

PX_ATTR_RAMFUNC void px_flash_wr_row(const uint32_t address, const uint32_t * data)
{
    uint8_t    i;
    uint32_t   primask;
    uint32_t * dest = (uint32_t *)address;

    // Save interrupt status
    primask = __get_PRIMASK();
    // Disable interrupts
    px_interrupts_disable();
    // Enable fast programming
    FLASH->CR |= FLASH_CR_FSTPG;
    // Write row by copying data (64-bits at a time)
    for (i = 0; i < PX_FLASH_ROW_SIZE_DOUBLE_WORDS; i++)
    {
        *dest++ = *data++;
        *dest++ = *data++;
    }
    // Wait until programming has finished (not busy)
    while ((FLASH->SR & FLASH_SR_BSY1) != 0)
    {
        continue;
    }
    // EOP (End Of Programming) Flag is set?
    while ((FLASH->SR & FLASH_SR_EOP) != 0)
    {
        // Reset EOP flag
        FLASH->SR = FLASH_SR_EOP;
    }    
    // Disable fast programming
    FLASH->CR &= ~FLASH_CR_FSTPG;
    // Restore interrupt status
    __set_PRIMASK(primask);
}
#endif