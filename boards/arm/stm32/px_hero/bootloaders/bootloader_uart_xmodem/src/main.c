/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          Piconomix STM32 Hero Board Bootloader application
    Author(s):      Pieter Conradie
    Creation Date:  2018-11-20
 
============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <string.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "main.h"
#include "px_defines.h"
#include "px_compiler.h"
#include "px_systmr.h"
#include "px_board.h"
#include "px_uart.h"
#include "px_sysclk.h"
#include "px_xmodem.h"
#include "px_flash.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
//! [Address mapping of App]
/// Start address of app in FLASH
#define MAIN_APP_ADR_START      (FLASH_BASE + 0x00004000)
/// End address of app in FLASH (out of bounds)
#define MAIN_APP_ADR_END        (FLASH_BASE + 0x00020000)
//! [Address mapping of App]

/// Start address of SRAM
#define MAIN_SRAM_ADR_START     (SRAM_BASE)
/// End address of SRAM (out of bounds)
#define MAIN_SRAM_ADR_END       (SRAM_BASE + SRAM_SIZE_MAX)

/// Magic value stored in SRAM to detect a reset double tap
#define MAIN_MAGIC_DOUBLE_TAP 0xa59b71dc 

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
/// UART handle
px_uart_handle_t px_uart_handle;

/// Location in SRAM that is used as message box between resets
PX_ATTR_SECTION(".noinit") volatile uint32_t main_magic;

/// Next FLASH address that will be written to
uint32_t main_flash_adr;

/// Next byte that will be written to in receive buffer
uint16_t main_flash_buf_index;

/* _____LOCAL VARIABLES______________________________________________________ */
/// Buffer of data to be written to FLASH
static union
{
    uint8_t  buf_u8[PX_FLASH_HALF_PAGE_SIZE];
    uint32_t buf_u32[PX_FLASH_HALF_PAGE_SIZE_WORDS];
} main_flash_buf;

/// Copy of app's vector table to be written after whole image is transferred
static union
{
    uint8_t  buf_u8[PX_FLASH_HALF_PAGE_SIZE];
    uint32_t buf_u32[PX_FLASH_HALF_PAGE_SIZE_WORDS];
} main_app_vector_table;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
//! [Execute App]
static void main_exe_app(void)
{
    uint32_t sp_adr; // app's stack pointer value
    uint32_t pc_adr; // app's reset address value

    // Does vector table at start of app have valid stack pointer value?
    sp_adr = *(uint32_t *)MAIN_APP_ADR_START;
    if( (sp_adr < MAIN_SRAM_ADR_START) || (sp_adr > MAIN_SRAM_ADR_END) )
    {
        // No. Return to bootloader
        return;
    }
    // Does vector table at start of app have valid reset address value?
    pc_adr = *(uint32_t *)(MAIN_APP_ADR_START + 4);
    if( (pc_adr < MAIN_APP_ADR_START) || (pc_adr >= MAIN_APP_ADR_END) )
    {
        // No. Return to bootloader
        return;
    }
    // Reset SRAM command so that bootloader will be executed again after reset
    main_magic = 0;
    // Rebase the stack pointer
    __set_MSP(sp_adr);
    // Rebase vector table to start of app (even though app may do it too)
    SCB->VTOR = MAIN_APP_ADR_START;
    // Jump to application reset address
    __asm__ __volatile__("bx %0\n\t"::"r"(pc_adr));
}
//! [Execute App]

/* _____PUBLIC FUNCTIONS_____________________________________________________ */
/// Handler function that is called when an XMODEM packet is received
void main_on_rx_data(const uint8_t * data, uint8_t bytes_received)
{
    // End of flash reached?
    if(main_flash_adr >= MAIN_APP_ADR_END)
    {
        // Discard rest of data
        return;
    }

    // Repeat until all received bytes have been buffered
    while(bytes_received != 0)
    {
        // Fill received byte into flash buffer
        main_flash_buf.buf_u8[main_flash_buf_index++] = *data++;

        // Flash buffer full?
        if(main_flash_buf_index >= PX_FLASH_HALF_PAGE_SIZE)
        {
            // LED off
            PX_USR_LED_OFF();

            // On a new flash page boundary?
            if((main_flash_adr % PX_FLASH_PAGE_SIZE) == 0)
            {
                // Erase whole flash page first
                px_flash_erase_page(main_flash_adr);
            }
            // Is this the first half page?
            if(main_flash_adr == MAIN_APP_ADR_START)
            {
                // Save a copy of the vector table, but do not write it
                memcpy(main_app_vector_table.buf_u8,
                       main_flash_buf.buf_u8, 
                       PX_FLASH_HALF_PAGE_SIZE);
            }
            else
            {
                // Write half flash page
                px_flash_wr_half_page(main_flash_adr, main_flash_buf.buf_u32);
            }
            // LED on
            PX_USR_LED_ON();
            // Next half page
            main_flash_adr += PX_FLASH_HALF_PAGE_SIZE;
            // Reset buffer
            main_flash_buf_index = 0;
        }
        // Next received byte
        bytes_received--;
    }
}

int main(void)
{
    // Enable +3V3 Hold pin so that board does not switch off while rebooting
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);
    px_gpio_init(&px_gpio_3v3_hold);

    // Is this an external reset (NRST)?
    if(LL_RCC_IsActiveFlag_PINRST())
    {
        // First reset?
        if (main_magic != MAIN_MAGIC_DOUBLE_TAP)
        {
            // Store SRAM magic value to signal that first reset has occured
            main_magic = MAIN_MAGIC_DOUBLE_TAP;
            // Wait ~ 500 ms
            for(uint32_t i = 0x40000; i!=0; i--)
            {
                // Prevent compiler from optimizing and removing empty delay loop
                __asm__ __volatile__("\n\t");
            }
            // Jump to app
            main_exe_app();
        }
        // Execute bootloader...
    }
    else
    {
        // Clear reset flags
        LL_RCC_ClearResetFlags();
        // Reset SRAM magic value
        main_magic = 0;
        // Jump to app
        main_exe_app();
    }

    // Reset SRAM magic value
    main_magic = 0;

    // Initialize modules
    px_board_init();
    px_sysclk_init();
    px_uart_init();

    // Open UART1
    px_uart_open2(&px_uart_handle,
                  PX_UART_NR_1,
                  115200, 
                  PX_UART_DATA_BITS_8, 
                  PX_UART_PARITY_NONE, 
                  PX_UART_STOP_BITS_1);

    // Enable LED
    PX_USR_LED_ON();

    // Unlock FLASH for erasing and programming
    px_flash_unlock();

    // Receive new FLASH content via XMODEM-CRC protocol
    main_flash_adr       = MAIN_APP_ADR_START;
    main_flash_buf_index = 0;

    if(px_xmodem_receive_file(&main_on_rx_data))
    {
        // Last received page incomplete?
        if(main_flash_buf_index != 0)
        {
#ifdef BOOT_CLEAR_REST_OF_FLASH
            // Clear rest of flash page
            uint16_t i = main_flash_buf_index;
            while(i < PX_FLASH_HALF_PAGE_SIZE)
            {
                main_flash_buf.buf_u8[i++] = 0xff;
            }
#endif
            // Must flash page be erased?
            if((main_flash_adr % PX_FLASH_PAGE_SIZE) == 0)
            {
                // Erase whole flash page
                px_flash_erase_page(main_flash_adr);
            }
            // Write last (incomplete) flash page
            px_flash_wr_half_page(main_flash_adr, main_flash_buf.buf_u32);

#ifdef BOOT_CLEAR_REST_OF_FLASH
            // Next half page
            main_flash_adr += PX_FLASH_HALF_PAGE_SIZE;
#endif
        }

#ifdef BOOT_CLEAR_REST_OF_FLASH
        // On a whole page boundary?
        if((main_flash_adr % PX_FLASH_PAGE_SIZE) != 0)
        {
            // No. Advance to start of next page
            main_flash_adr += PX_FLASH_HALF_PAGE_SIZE;
        }
        // Erase rest of flash pages
        while(main_flash_adr < MAIN_APP_ADR_END)
        {
            px_flash_erase_page(main_flash_adr);
            // Next page
            main_flash_adr += PX_FLASH_PAGE_SIZE;
        }
#endif

        // Write vector table last (after whole image has been written)
        px_flash_wr_half_page(MAIN_APP_ADR_START, main_app_vector_table.buf_u32);
    }

    // Lock FLASH to prevent accidental erasing and programming
    px_flash_lock(); 
    
    // Perform software reset
    NVIC_SystemReset();
}
