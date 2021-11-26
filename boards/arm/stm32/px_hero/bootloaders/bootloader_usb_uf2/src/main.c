/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          Piconomix STM32 Hero Board Bootloader application
    Author(s):      Pieter Conradie
    Creation Date:  2019-03-24
 
============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <string.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "main.h"
#include "px_defs.h"
#include "px_board.h"
#include "px_sysclk.h"
#include "px_systmr.h"
#include "px_uart.h"
#include "px_flash.h"
#include "usb_device.h"
#include "px_uf2.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("main");

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
#define MAIN_MAGIC_DOUBLE_TAP   0xa59b71dc

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
/// UART handle
px_uart_handle_t px_uart_handle;

/// Location in SRAM that is used as message box between resets
PX_ATTR_SECTION(".noinit") volatile uint32_t main_magic;

#ifdef BOOT_CLEAR_REST_OF_FLASH
/// Highest FLASH address written to + 1
uint32_t main_flash_adr;
#endif

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

/// Flag that is set when last block of flash has been written
volatile bool main_wr_flash_done_flag;

/// Timer for LED flashing
static px_systmr_t main_tmr;

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
/// Handler function that is called when a valid UF2 block is received
void main_wr_flash_block(const uint8_t * data, 
                         uint32_t        adr, 
                         size_t          nr_of_bytes)
{
    // Adjust to start of FLASH
    adr += FLASH_BASE;

    // Repeat until all received bytes have been buffered
    while(nr_of_bytes != 0)
    {
        // Copy bytes into into flash buffer
        memcpy(main_flash_buf.buf_u8, data, PX_FLASH_HALF_PAGE_SIZE);
        // LED off
        PX_USR_LED_OFF();
        // On a new flash page boundary?
        if((adr % PX_FLASH_PAGE_SIZE) == 0)
        {
            // Erase whole flash page first
            px_flash_erase_page(adr);
        }
        // Is this the first half page?
        if(adr == MAIN_APP_ADR_START)
        {
            // Save a copy of the vector table, but do not write it
            memcpy(main_app_vector_table.buf_u8,
                   main_flash_buf.buf_u8, 
                   PX_FLASH_HALF_PAGE_SIZE);
        }
        else
        {
            // Write half flash page
            px_flash_wr_half_page(adr, main_flash_buf.buf_u32);
        }
        // LED on
        PX_USR_LED_ON();
        // Next half page
        adr         += PX_FLASH_HALF_PAGE_SIZE;
        data        += PX_FLASH_HALF_PAGE_SIZE;
        nr_of_bytes -= PX_FLASH_HALF_PAGE_SIZE;
    }

#ifdef BOOT_CLEAR_REST_OF_FLASH
    // Save highest address
    if(main_flash_adr < adr)
    {
        main_flash_adr = adr;
    }
#endif
}

void main_wr_flash_done(void)
{
    main_wr_flash_done_flag = true;
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
            for(uint32_t i = 0x40000; i != 0; i--)
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
    px_uf2_init(&main_wr_flash_block, &main_wr_flash_done);
    
#if PX_LOG
    // Open UART1
    px_uart_init();
    px_uart_open2(&px_uart_handle,
                  PX_UART_NR_1,
                  115200, 
                  PX_UART_DATA_BITS_8, 
                  PX_UART_PARITY_NONE, 
                  PX_UART_STOP_BITS_1);
    PX_LOG_I("Starting USB mass storage bootloader");
#endif

    // Start timer for LED flashing
    px_systmr_start(&main_tmr, PX_SYSTMR_MS_TO_TICKS(250));    

    // Unlock FLASH for erasing and programming
    px_flash_unlock();

    // Start USB driver
    MX_USB_DEVICE_Init();

    // Allow USB Driver to execute until last block of flash has been written
    while(!main_wr_flash_done_flag)
    {
        // Flash LED
        if(px_systmr_has_expired(&main_tmr))
        {
            px_systmr_restart(&main_tmr);
            PX_USR_LED_TOGGLE();
        }
        // Put core into SLEEP mode until an interrupt occurs
        __WFI();
    }

    // Disable LED
    PX_USR_LED_OFF();

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

    // Lock FLASH to prevent accidental erasing and programming
    px_flash_lock();

    // Wait a little bit before resetting, to avoid Windows transmit error
    // https://github.com/Microsoft/px_uf2-samd21/issues/11
    px_board_delay_init();
    px_board_delay_ms(30);

    // Perform software reset
    NVIC_SystemReset();
}

#if PX_LOG
void main_log_putchar(char data)
{
    // New line character?
    if(data == '\n')
    {
        // Prepend a carriage return
        main_log_putchar('\r');
    }
    px_uart_putchar(&px_uart_handle, data);
}

void main_log_timestamp(char * str)
{
    sprintf(str, "%08lu", (uint32_t)px_sysclk_get_tick_count());
}
#endif

