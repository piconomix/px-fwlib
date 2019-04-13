#ifndef __MAIN_H__
#define __MAIN_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.

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
#include "px_adc.h"
#include "px_dac.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS _________________________________________________________ */
#define MAIN_BUFFER_SIZE 512

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
extern px_uart_handle_t px_uart1_handle;
extern px_spi_handle_t  px_spi_sf_handle;
extern px_spi_handle_t  px_spi_sd_handle;
extern px_spi_handle_t  px_spi_lcd_handle;
extern px_i2c_handle_t  px_i2c_handle;
extern px_adc_handle_t  px_adc_handle;
extern px_dac_handle_t  px_dac_handle;

extern uint8_t          main_buffer[MAIN_BUFFER_SIZE];

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
void main_usb_event_connected   (void);
void main_dbg_put_char          (char data);
void main_dbg_timestamp         (char * str);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

#endif // #ifndef __MAIN_H__
