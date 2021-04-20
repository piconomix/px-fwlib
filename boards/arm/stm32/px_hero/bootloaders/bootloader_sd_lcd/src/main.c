/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md

    Title:          Piconomix STM32 Hero Board Bootloader application
    Author(s):      Pieter Conradie
    Creation Date:  2019-03-24
 
============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <string.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "main.h"
#include "px_defines.h"
#include "px_board.h"
#include "px_sysclk.h"
#include "px_systmr.h"
#include "px_uart.h"
#include "px_uart_stdio.h"
#include "px_spi.h"
#include "px_flash.h"
#include "px_sd.h"
#include "px_uf2.h"
#include "px_lcd_st7567_jhd12864.h"
#include "px_gfx.h"
#include "px_gfx_fonts.h"
#include "px_debounce.h"
#include "ff.h"
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
#define MAIN_MAGIC_DOUBLE_TAP 0xa59b71dc

// Maximum number of files
#define MAIN_FILES_MAX          8
// Maximum file name size (8.3 and zero termination)
#define MAIN_FILE_NAME_SIZE_MAX 13

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
px_spi_handle_t  px_spi_sd_handle;
px_spi_handle_t  px_spi_lcd_handle;

#if PX_LOG
/// UART handle
px_uart_handle_t px_uart_handle;
#endif

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

// Chan Fs Filesystem object
static FATFS chan_fs_fatfs;
// Chan Fs Directory object
static DIR chan_fs_dir;
// Chan Fs File information object
static FILINFO chan_fs_file_info;
// Chan Fs File object
static FIL chan_fs_file;
// File data buffer
static uint8_t chan_fs_buf[512];

// List of files found on SD card
static char main_files[MAIN_FILES_MAX][MAIN_FILE_NAME_SIZE_MAX];
static int  main_nr_of_files;

// Debounced button presses
static px_debounce_t main_debounce_btn_up;
static px_debounce_t main_debounce_btn_dn;
static px_debounce_t main_debounce_btn_yes;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */
static void main_fatal_error(const char * error_msg) PX_ATTR_NORETURN;

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void main_beep(void)
{
    px_board_buzzer_on(4000);
    px_board_delay_ms(50);
    px_board_buzzer_off();
}

static void main_display_msg(const char * msg)
{
    px_gfx_buf_clear();
    px_gfx_draw_str(0, 0, msg);
    px_gfx_draw_update();
}

static void main_fatal_error(const char * error_msg)
{
    main_display_msg(error_msg);
    while(true)
    {
        PX_USR_LED_TOGGLE();
        px_board_delay_ms(50);
    }
}

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

static bool main_init(void)
{
    // Initialize modules
    px_board_init();
    px_sysclk_init();
    px_spi_init();    

#if PX_LOG
    // Open UART1
    px_uart_init();
    px_uart_open2(&px_uart_handle,
                  PX_UART_NR_1,
                  115200, 
                  PX_UART_DATA_BITS_8, 
                  PX_UART_PARITY_NONE, 
                  PX_UART_STOP_BITS_1);
    // Connect stdin and stdout to UART1
    px_uart_stdio_init(&px_uart_handle);
#endif
    // Initialise SD Card driver
    px_spi_open2(&px_spi_sd_handle,
                 PX_SPI_NR_1,
                 PX_BOARD_SPI1_CS_SD,
                 px_spi_util_baud_hz_to_clk_div(PX_SD_MAX_SPI_CLOCK_HZ),
                 PX_SD_SPI_MODE, 
                 PX_SD_SPI_DATA_ORDER,
                 PX_SD_SPI_MO_DUMMY_BYTE);
    px_sd_init(&px_spi_sd_handle);
    // Initialise LCD driver
    px_spi_open2(&px_spi_lcd_handle,
                 PX_SPI_NR_2,
                 PX_BOARD_SPI2_CS_LCD,
                 px_spi_util_baud_hz_to_clk_div(PX_LCD_MAX_SPI_CLOCK_HZ),
                 PX_LCD_SPI_MODE, 
                 PX_LCD_SPI_DATA_ORDER,
                 0x00);
    px_lcd_init(&px_spi_lcd_handle);
    // Success
    return true;
}

static void main_sd_mount(void)
{
    // Card inserted?
    if(!PX_SD_CARD_DETECTED())
    {
        main_fatal_error("SD card not inserted");
    }
    // Initialise SD driver
    if(!px_sd_reset())
    {
        main_fatal_error("Could not init SD card");
    }
    // Mount logical drive (immediately)
    if(f_mount(&chan_fs_fatfs, "", 1) != FR_OK)
    {
        main_fatal_error("Unable to mount drive");
    }
}

static void main_sd_ls(void)
{
    main_nr_of_files = 0;

    // Find first file with *.UF2 extension
    if(f_findfirst(&chan_fs_dir, &chan_fs_file_info, "/", "*.UF2") != FR_OK)
    {
        main_fatal_error("No files");
    }

    // Repeat until all files are found
    do
    {
        // No more files?
        if(chan_fs_file_info.fname[0] == '\0')
        {
            // Stop
            break;
        }
        // Copy file name
        strcpy(main_files[main_nr_of_files], chan_fs_file_info.fname);
        // Max files reached?
        if(++main_nr_of_files == MAIN_FILES_MAX)
        {
            // Stop
            break;
        }
    }
    while(f_findnext(&chan_fs_dir, &chan_fs_file_info) == FR_OK);

    // No files?
    if(main_nr_of_files == 0)
    {
        main_fatal_error("No files");
    }    
}

static void main_sort_file_names(void)
{
    int    i, j, k;
    char * file1;
    char * file2;
    char   file_tmp[MAIN_FILE_NAME_SIZE_MAX];

    // Only one file?
    if(main_nr_of_files <= 1)
    {
        return;
    }
    // Bubble sort file names alphabetically
    for(i = 0; i < (main_nr_of_files - 1); i++)
    {
        for(j = 0; j < (main_nr_of_files - i - 1); j++)
        {
            file1 = main_files[j];
            file2 = main_files[j + 1];
            for(k = 0; k < MAIN_FILE_NAME_SIZE_MAX; k++)
            {
                if(  (file1[k] == '.') || (file1[k] == '\0')  )
                {
                    break;
                }
                if(  (file2[k] == '.') || (file2[k] == '\0')  )
                {
                    break;
                }
                if(file1[k] < file2[k])
                {
                    break;
                }
                else if(file1[k] > file2[k])
                {
                    strcpy(file_tmp, file1);
                    strcpy(file1,    file2);
                    strcpy(file2,    file_tmp);
                    break;
                }
            }
        }
    }
}

static void main_gfx_show_files(void)
{
    int  i;
    int  j;
    char file[MAIN_FILE_NAME_SIZE_MAX];

    // Clear display buffer
    px_gfx_buf_clear();
    for(i = 0; i < main_nr_of_files; i++)
    {
        // Copy file name until extension separator is found
        for(j = 0; j < MAIN_FILE_NAME_SIZE_MAX; j++)
        {
            if(main_files[i][j] == '.')
            {
                break;
            }
            file[j] = main_files[i][j];
        }
        // Add zero termination
        file[j] = '\0';
        // Draw file name
        px_gfx_draw_str(px_gfx_font_5x7.width * 2, 
                        i * px_gfx_font_5x7.height, 
                        file);
    }
    // Update display
    px_gfx_draw_update();
}

static int main_gfx_usr_select(void)
{
    int i     = 0;
    int i_old = 0;

    // Init debouncing for buttons
    px_debounce_init(&main_debounce_btn_up,  true);
    px_debounce_init(&main_debounce_btn_dn,  true);
    px_debounce_init(&main_debounce_btn_yes, true);
    // Display arrow next to first file
    px_gfx_draw_char(0, 0, '>');
    px_gfx_draw_update();
    // Repeat until YES button is pressed
    while(true)
    {
        // Updated debounced button states
        px_board_delay_ms(10);
        px_debounce_update(&main_debounce_btn_up,  px_gpio_pin_is_hi(&px_gpio_lcd_btn_3_up));
        px_debounce_update(&main_debounce_btn_dn,  px_gpio_pin_is_hi(&px_gpio_lcd_btn_4_dn));
        px_debounce_update(&main_debounce_btn_yes, px_gpio_pin_is_hi(&px_gpio_lcd_btn_5_yes));
        // UP pressed?
        if(px_debounce_falling_edge_detected(&main_debounce_btn_up))
        {
            // Move arrow up
            main_beep();
            if(i != 0)
            {
                i--;
            }
        }
        // Down pressed?
        if(px_debounce_falling_edge_detected(&main_debounce_btn_dn))
        {
            // Move arrow down
            main_beep();
            if(i < (main_nr_of_files - 1))
            {
                i++;
            }
        }
        // Yes pressed?
        if(px_debounce_falling_edge_detected(&main_debounce_btn_yes))
        {
            // Stop and return user selection
            main_beep();
            return i;
        }
        // Arrow moved?
        if(i != i_old)
        {
            // Erase old arrow
            px_gfx_draw_char(0, i_old * px_gfx_font_5x7.height, ' ');
            // Draw new arrow
            px_gfx_draw_char(0, i     * px_gfx_font_5x7.height, '>');
            px_gfx_draw_update();
            i_old = i;
        }
    }
}

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
    int  file_index;
    UINT bytes_read;

    // Enable +3V3 Hold pin so that board does not switch off while rebooting
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);
    px_gpio_pin_init(&px_gpio_3v3_hold);

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
    main_init();
    px_gfx_init();
    PX_LCD_BACKLIGHT_ON();
    main_beep();

    // Mount SD card
    main_sd_mount();
    // Get list of files
    main_sd_ls();
    // Sort file names
    main_sort_file_names();
    // Display list of files
    main_gfx_show_files();
    // Get file selected
    file_index = main_gfx_usr_select();

    // Initialise UF2 module
    px_uf2_init(&main_wr_flash_block, &main_wr_flash_done);
    // Open UF2 file
    if(f_open(&chan_fs_file, main_files[file_index], FA_READ | FA_OPEN_EXISTING) != FR_OK)
    {
        main_fatal_error("Could not open file");
    }
    // Clear display
    px_gfx_buf_clear();;
    px_gfx_draw_update();
    // Start timer for LED flashing
    px_systmr_start(&main_tmr, PX_SYSTMR_MS_TO_TICKS(100));    
    // Unlock FLASH for erasing and programming
    px_flash_unlock();
    // Stream file data to UF2 module
    do
    {
        // Toggle LED
        if(px_systmr_has_expired(&main_tmr))
        {
            px_systmr_restart(&main_tmr);
            PX_USR_LED_TOGGLE();
        }
        // Read a sector of data (512 bytes)
        if(f_read(&chan_fs_file, chan_fs_buf, sizeof(chan_fs_buf), &bytes_read) != FR_OK)
        {
            main_fatal_error("Could not read file");
        }
        // Pass data to UF2 module to process
        px_uf2_on_wr_sector(0, chan_fs_buf);
    } while(bytes_read == sizeof(chan_fs_buf));

    // Finished?
    if(main_wr_flash_done_flag == false)
    {
        // No
        main_fatal_error("Incomplete");
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

    px_uart_put_char(&px_uart_handle, data);
}

void main_log_timestamp(char * str)
{
    sprintf(str, "%08lu", (uint32_t)px_sysclk_get_tick_count());
}
#endif

