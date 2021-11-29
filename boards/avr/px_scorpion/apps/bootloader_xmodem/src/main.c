/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2008 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_xmodem_bootloader : Microchip AVR XMODEM bootloader
    Author(s):      Pieter Conradie
    Creation Date:  2007-04-27
 
============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <avr/io.h>
#include <avr/wdt.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"
#include "px_xmodem.h"
#include "px_board.h"
#include "px_uart.h"
#include "px_tmr.h"
#include "px_flash.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
#define PX_FLASH_BOOT_PAGE_START    (FLASH_BOOT_START / PX_FLASH_PAGE_SIZE)
#define PX_FLASH_START_PAGE         0

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static uint8_t  px_flash_buf[PX_FLASH_PAGE_SIZE];
static uint16_t px_flash_page;

#if (PX_FLASH_PAGE_SIZE < 256)
static uint8_t  px_flash_buf_index;
#else
static uint16_t px_flash_buf_index;
#endif

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
/// Handler function that is called when an XMODEM packet is received
static void main_on_rx_data(const uint8_t * data, uint8_t bytes_received)
{
    // Repeat untill all received bytes have been buffered
    while(bytes_received != 0)
    {
        // Fill received byte into flash buffer
        px_flash_buf[px_flash_buf_index++] = *data++;

        // Flash buffer full?
        if(px_flash_buf_index >= PX_FLASH_PAGE_SIZE)
        {
            PX_LED_OFF();

            // Write flash page
            px_flash_wr_page(px_flash_page, px_flash_buf);

            PX_LED_ON();
        
            // Next page
            px_flash_page++;
            // Wrap if in bootloader section
            if(px_flash_page >= PX_FLASH_BOOT_PAGE_START)
            {
                px_flash_page = 0;
            }

            // Reset index
            px_flash_buf_index = 0;
        }

        // Next received byte
        bytes_received--;
    }
}

/* _____PUBLIC FUNCTIONS_____________________________________________________ */
void reset(void) PX_ATTR_NORETURN;

int main(void) PX_ATTR_SECTION(".init9") PX_ATTR_NAKED;
int main(void)
{
    // Disable Watchdog (in case it has been enabled)
    MCUSR = 0;
    wdt_disable();

    // Initialise board
    px_board_init();
#if 0
    // User button not pressed?
    if(!PB_IS_PRESSED())
    {
        // Skip bootloader
        reset();
    }
#endif
    // Enable LED
    PX_LED_ON();

    // Initialise modules
    px_tmr_init();
    px_uart_init();

    // Wait 100 ms
    px_tmr_wait(TMR_MS_TO_TICKS(100));

    // Receive new flash content via XMODEM-CRC protocol
    px_flash_page      = PX_FLASH_START_PAGE;
    px_flash_buf_index = 0;

    if(px_xmodem_receive_file(&main_on_rx_data))
    {
        // Last received page incomplete?
        if(px_flash_buf_index != 0)
        {
#ifdef BOOT_CLEAR_REST_OF_FLASH
            // Clear rest of flash page
            uint16_t i = px_flash_buf_index;
            while(i < PX_FLASH_PAGE_SIZE)
            {
                px_flash_buf[i++] = 0xFF;
            }
#endif
            // Write last (incomplete) flash page
            px_flash_wr_page(px_flash_page, px_flash_buf);

#ifdef BOOT_CLEAR_REST_OF_FLASH
            // Next page
            px_flash_page++;
#endif
        }

#ifdef BOOT_CLEAR_REST_OF_FLASH
        // Clear rest of flash pages
        while(px_flash_page < PX_FLASH_BOOT_PAGE_START)
        {
            px_flash_erase_page(px_flash_page);
            // Next page
            px_flash_page++;
        }
#endif
    }

    // Jump to start of application (Reset Vector @ 0x0000)
	reset();
}
