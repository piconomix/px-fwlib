#ifndef __MAIN_H__
#define __MAIN_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          Piconomix STM32 Hero Board CLI application
    Author(s):      Pieter Conradie
    Creation Date:  2018-03-01

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_uart.h"
#include "px_spi.h"
#include "px_i2c.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
#define MAIN_BUFFER_SIZE 512

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
extern px_uart_handle_t px_uart1_handle;
extern px_spi_handle_t  px_spi_sf_handle;
extern px_spi_handle_t  px_spi_sd_handle;
extern px_spi_handle_t  px_spi_lcd_handle;
extern px_i2c_handle_t  px_i2c_bme280_handle;

extern uint8_t          main_buffer[MAIN_BUFFER_SIZE];

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
void main_log_putchar          (char data);
void main_log_timestamp         (char * str);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

#endif
