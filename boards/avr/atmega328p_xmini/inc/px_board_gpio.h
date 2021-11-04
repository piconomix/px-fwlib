#ifndef __PX_BOARD_GPIO_H__
#define __PX_BOARD_GPIO_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2021 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
   
    Title:          board.h : ATmega328P-XMINI Board GPIO pin definitions
    Author(s):      Pieter Conradie
    Creation Date:  2021-09-26

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
#define PX_GPIO_LED         PX_GPIO(B, 5, PX_GPIO_DIR_OUT, PX_GPIO_INIT_LO)
#define PX_GPIO_BTN         PX_GPIO(B, 7, PX_GPIO_DIR_IN, PX_GPIO_INIT_PULL_UP)

#define PX_GPIO_UART_RXD    PX_GPIO(D, 0, PX_GPIO_DIR_IN, PX_GPIO_INIT_PULL_UP)
#define PX_GPIO_UART_TXD    PX_GPIO(D, 1, PX_GPIO_DIR_OUT, PX_GPIO_INIT_HI)

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
static const px_gpio_handle_t px_gpio_led =      {PX_GPIO_LED};
static const px_gpio_handle_t px_gpio_btn =      {PX_GPIO_BTN};
                            
static const px_gpio_handle_t px_gpio_uart_rxd = {PX_GPIO_UART_RXD};
static const px_gpio_handle_t px_gpio_uart_txd = {PX_GPIO_UART_TXD};

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */

/* _____MACROS_______________________________________________________________ */
#define PX_GPIO_B5 PX_GPIO_LED
#define PX_GPIO_B7 PX_GPIO_BTN

#define PX_GPIO_D0 PX_GPIO_UART_RXD
#define PX_GPIO_D1 PX_GPIO_UART_TXD

#ifdef __cplusplus
}
#endif

#endif
