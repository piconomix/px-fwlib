/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md

    Title:          Piconomix STM32 Hero Board USB Mass Storage Device example
    Author(s):      Pieter Conradie
    Creation Date:  2019-05-24
 
============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "main.h"
#include "px_defines.h"
#include "px_systmr.h"
#include "px_board.h"
#include "px_uart.h"
#include "px_spi.h"
#include "px_sysclk.h"
#include "px_sd.h"
#include "usb_device.h"
#include "px_dbg.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_DBG_DECL_NAME("main");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
px_uart_handle_t px_uart1_handle;
px_spi_handle_t  px_spi_sd_handle;
uint8_t          main_buffer[MAIN_BUFFER_SIZE];
bool             main_usb_connected_event_flag;
uint32_t         main_sd_capacity_blocks;


/* _____LOCAL VARIABLES______________________________________________________ */             
px_sd_csd_t main_sd_csd;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static bool main_init(void)
{
    // Initialize modules
    px_board_init();
    px_sysclk_init();
    px_uart_init();
    px_spi_init();    

    // Open UART1
    px_uart_open2(&px_uart1_handle,
                  PX_UART_NR_1,
                  115200, 
                  PX_UART_DATA_BITS_8, 
                  PX_UART_PARITY_NONE, 
                  PX_UART_STOP_BITS_1); 

    // Initialise SD Card driver
    px_spi_open2(&px_spi_sd_handle,
                 PX_SPI_NR_1,
                 PX_BOARD_SPI1_CS_SD,
                 px_spi_util_baud_hz_to_clk_div(PX_SD_MAX_SPI_CLOCK_HZ),
                 PX_SD_SPI_MODE, 
                 PX_SD_SPI_DATA_ORDER,
                 PX_SD_SPI_MO_DUMMY_BYTE);
    px_sd_init(&px_spi_sd_handle);

    // Start USB driver
    MX_USB_DEVICE_Init();

    // Success
    return true;
}

bool main_sd_reset(void)
{
    main_sd_capacity_blocks = 0;

    if(!px_sd_reset())
    {
        PX_DBG_E("Failed to reset SD card");
        return false;
    }

    if(!px_sd_read_csd(&main_sd_csd))
    {
        PX_DBG_E("Failed to read CSD");
        return false;
    }

    main_sd_capacity_blocks = px_sd_get_capacity_in_blocks(&main_sd_csd);

    return true;
}

/* _____PUBLIC FUNCTIONS_____________________________________________________ */
int main(void)
{
    // Initialize board and peripheral drivers
    main_init();

    // Enable LED
    PX_USR_LED_ON();

    // Is SD card inserted?
    if(PX_SD_CARD_DETECTED())
    {
        main_sd_reset();
    }

    for(;;)
    {
        // USB connected event?
        if(main_usb_connected_event_flag)
        {
            PX_DBG_I("USB Connect event");
            main_usb_connected_event_flag = false;            
        }        

        // Put core into SLEEP mode until an interrupt occurs
        __WFI();
    }
}

void main_usb_event_connected(void)
{
    main_usb_connected_event_flag = true;
}

void main_dbg_put_char(char data)
{
    // New line character?
    if(data == '\n')
    {
        // Prepend a carriage return
        main_dbg_put_char('\r');
    }

    px_uart_put_char(&px_uart1_handle, data);
}

void main_dbg_timestamp(char * str)
{
    sprintf(str, "%08lu", (uint32_t)px_sysclk_get_tick_count());
}
