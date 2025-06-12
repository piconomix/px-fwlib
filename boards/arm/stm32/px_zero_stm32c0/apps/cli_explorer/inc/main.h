#ifndef __MAIN_H__
#define __MAIN_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2025 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          PX-ZER0 STM32C0 CLI application
    Author(s):      Pieter Conradie
    Creation Date:  2025-05-25

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"
#include "px_uart.h"
#include "px_spi.h"
#include "px_i2c.h"
#include "px_adc.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
#define MAIN_BUF_SIZE 512

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
extern px_uart_handle_t px_uart2_dbg_handle;
extern px_spi_handle_t  px_spi_sf_handle;
extern px_i2c_handle_t  px_i2c_handle;
extern px_adc_handle_t  px_adc_handle;

extern uint8_t          main_buf[MAIN_BUF_SIZE];

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
void main_usb_event_connected(void);
void main_log_putchar        (char data);
void main_log_print          (const char * str);
void main_log_timestamp      (char * str);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

#endif
