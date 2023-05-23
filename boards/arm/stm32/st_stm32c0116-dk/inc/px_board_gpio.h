#ifndef __PX_BOARD_GPIO_H__
#define __PX_BOARD_GPIO_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2023 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
   
    Title:          board.h : STM32C0116-DK board GPIO pin definitions
    Author(s):      Pieter Conradie
    Creation Date:  2023-05-20

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"
#include "px_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
#define PX_GPIO_UART2_TX     PX_GPIO(A,  2, PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_LO, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_HI, PX_GPIO_AF_1)
#define PX_GPIO_UART2_RX     PX_GPIO(A,  3, PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_UP, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_1)
#define PX_GPIO_ADC_JOYSTICK PX_GPIO(A,  8, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#define PX_GPIO_UART_DBG_TX  PX_GPIO(A,  9, PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_LO, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_HI, PX_GPIO_AF_1)
#define PX_GPIO_UART_DBG_RX  PX_GPIO(A, 10, PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_UP, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_1)
#define PX_GPIO_SWDIO        PX_GPIO(A, 13, PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_HI, PX_GPIO_PULL_UP, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_0)
#define PX_GPIO_SWDCK        PX_GPIO(A, 14, PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_DN, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_0)

#define PX_GPIO_LED          PX_GPIO(B,  6, PX_GPIO_MODE_OUT, PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_LO, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_HI, PX_GPIO_AF_NA)

#define PX_GPIO_A2           PX_GPIO_UART2_TX
#define PX_GPIO_A3           PX_GPIO_UART2_RX
#define PX_GPIO_A8           PX_GPIO_ADC_JOYSTICK
#define PX_GPIO_A9           PX_GPIO_UART_DBG_TX
#define PX_GPIO_A10          PX_GPIO_UART_DBG_RX
#define PX_GPIO_A13          PX_GPIO_SWDIO
#define PX_GPIO_A14          PX_GPIO_SWDCK

#define PX_GPIO_B6           PX_GPIO_LED

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
static const px_gpio_handle_t px_gpio_uart2_tx     = {PX_GPIO_UART2_TX};
static const px_gpio_handle_t px_gpio_uart2_rx     = {PX_GPIO_UART2_RX};
static const px_gpio_handle_t px_gpio_adc_joystick = {PX_GPIO_ADC_JOYSTICK};
static const px_gpio_handle_t px_gpio_uart_dbg_tx  = {PX_GPIO_UART_DBG_TX};
static const px_gpio_handle_t px_gpio_uart_dbg_rx  = {PX_GPIO_UART_DBG_RX};
static const px_gpio_handle_t px_gpio_swdio        = {PX_GPIO_SWDIO};
static const px_gpio_handle_t px_gpio_swdck        = {PX_GPIO_SWDCK};

static const px_gpio_handle_t px_gpio_led          = {PX_GPIO_LED};

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

#endif
