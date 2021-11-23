#ifndef __PX_BOARD_GPIO_H__
#define __PX_BOARD_GPIO_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2020 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
   
    Title:          board.h : Nucleo-L053 board GPIO pin definitions
    Author(s):      Pieter Conradie
    Creation Date:  2020-04-30

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"
#include "px_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
/// ST-LINK TX
#define PX_GPIO_UART2_TX        PX_GPIO(A, 2,  PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_LO, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_HI, PX_GPIO_AF_4)
/// ST-LINK RX
#define PX_GPIO_UART2_RX        PX_GPIO(A, 3,  PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_UP, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_4)
/// LD2
#define PX_GPIO_LED_GRN         PX_GPIO(A, 5,  PX_GPIO_MODE_OUT, PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_LO, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_LO, PX_GPIO_AF_NA)
/// D8 on Arduino CN5 header
#define PX_GPIO_UART1_TX        PX_GPIO(A, 9,  PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_LO, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_HI, PX_GPIO_AF_4)
/// D2 on Arduino CN9 header
#define PX_GPIO_UART1_RX        PX_GPIO(A, 10, PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_UP, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_4)
#define PX_GPIO_SWDIO           PX_GPIO(A, 13, PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_HI, PX_GPIO_PULL_UP, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_0)
#define PX_GPIO_SWDCK           PX_GPIO(A, 14, PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_DN, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_0)
/// User Button
#define PX_GPIO_BTN_BLUE        PX_GPIO(C, 13, PX_GPIO_MODE_IN,  PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_UP, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
static const px_gpio_handle_t px_gpio_uart2_tx       = {PX_GPIO_UART2_TX};
static const px_gpio_handle_t px_gpio_uart2_rx       = {PX_GPIO_UART2_RX};
static const px_gpio_handle_t px_gpio_led_grn        = {PX_GPIO_LED_GRN};
static const px_gpio_handle_t px_gpio_uart1_tx       = {PX_GPIO_UART1_TX};
static const px_gpio_handle_t px_gpio_uart1_rx       = {PX_GPIO_UART1_RX};
static const px_gpio_handle_t px_gpio_swdio          = {PX_GPIO_SWDIO};
static const px_gpio_handle_t px_gpio_swdck          = {PX_GPIO_SWDCK};

static const px_gpio_handle_t px_gpio_btn_blue       = {PX_GPIO_BTN_BLUE};

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */

/* _____MACROS_______________________________________________________________ */
#define PX_GPIO_A2  PX_GPIO_UART2_TX
#define PX_GPIO_A3  PX_GPIO_UART2_RX
#define PX_GPIO_A5  PX_GPIO_LED_GRN
#define PX_GPIO_A9  PX_GPIO_UART1_TX
#define PX_GPIO_A10 PX_GPIO_UART1_RX
#define PX_GPIO_A13 PX_GPIO_SWDIO
#define PX_GPIO_A14 PX_GPIO_SWDCK

#define PX_GPIO_C13 PX_GPIO_BTN_BLUE

#ifdef __cplusplus
}
#endif

#endif
