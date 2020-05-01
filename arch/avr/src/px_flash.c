/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2006 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          px_flash.h : Internal FLASH write routines
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <avr/boot.h>
#include <avr/interrupt.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_flash.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
BOOTLOADER_SECTION void px_flash_erase_page(const uint16_t page)
{
    // Calculate 32-bit address from specified page
    uint32_t adr = ((uint32_t)page) * PX_FLASH_PAGE_SIZE;

    // Save status of global interrupts
    uint8_t sreg = SREG;

    // Disable interrupts
    cli();

    // Erase page
    boot_page_erase(adr);

    // Wait until SPM command has finished
    boot_spm_busy_wait();

    // Re-enable RWW-section
    boot_rww_enable();

    // Restore status of global interrupts
    SREG = sreg;
}

BOOTLOADER_SECTION void px_flash_wr_page(const uint16_t page, const uint8_t* data)
{
    uint16_t i;

    // Calculate 32-bit address from specified page
    uint32_t adr = ((uint32_t)page) * PX_FLASH_PAGE_SIZE;

    // Save status of global interrupts
    uint8_t sreg = SREG;

    // Disable interrupts
    cli();

    // Clear the page first
    px_flash_erase_page(page);

    // Fill write data with 16-bit data
    for(i=0; i<PX_FLASH_PAGE_SIZE; i+=2)
    {
        // Fetch little endian word of data
        uint16_t word;
        word  = *data++;
        word += (*data++) << 8;

        boot_page_fill(adr+i, word);
    }

    // Commit data to flash page
    boot_page_write(adr);   

    // Wait until SPM command has finished
    boot_spm_busy_wait();

    // Re-enable RWW-section
    boot_rww_enable();

    // Restore status of global interrupts
    SREG = sreg;
}

