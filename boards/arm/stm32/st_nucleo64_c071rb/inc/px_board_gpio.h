#ifndef __PX_BOARD_GPIO_H__
#define __PX_BOARD_GPIO_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2026 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          px_board_gpio.h : ST Nucleo64-C071RB GPIO pin definitions
    Author(s):      Pieter Conradie
    Creation Date:  2026-02-14

============================================================================= */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"
#include "px_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
#define PX_GPIO_DBG_UART2_TX    PX_GPIO(A,  2, PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_LO, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_HI, PX_GPIO_AF_4)
#define PX_GPIO_DBG_UART2_RX    PX_GPIO(A,  3, PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_UP, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_4)
#define PX_GPIO_LED_GRN         PX_GPIO(A,  5, PX_GPIO_MODE_OUT, PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_LO, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_LO, PX_GPIO_AF_NA)
#define PX_GPIO_USBD_N          PX_GPIO(A, 11, PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_VH, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_10)
#define PX_GPIO_USBD_P          PX_GPIO(A, 12, PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_VH, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_10)
#define PX_GPIO_SWDIO           PX_GPIO(A, 13, PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_HI, PX_GPIO_PULL_UP, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_0)
#define PX_GPIO_SWDCK           PX_GPIO(A, 14, PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_DN, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_0)

#define PX_GPIO_USB_SENSE       PX_GPIO(C,  0, PX_GPIO_MODE_IN,  PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#define PX_GPIO_LED_BLUE        PX_GPIO(C,  9, PX_GPIO_MODE_OUT, PX_GPIO_OTYPE_OD, PX_GPIO_OSPEED_LO, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_HI, PX_GPIO_AF_NA)
#define PX_GPIO_BTN_BLUE        PX_GPIO(C, 13, PX_GPIO_MODE_IN,  PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_UP, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#define PX_GPIO_OSC32_IN        PX_GPIO(C, 14, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#define PX_GPIO_OSC32_OUT       PX_GPIO(C, 15, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)

#define PX_GPIO_A2              PX_GPIO_DBG_UART2_TX
#define PX_GPIO_A3              PX_GPIO_DBG_UART2_RX
#define PX_GPIO_A5              PX_GPIO_LED_GRN
#define PX_GPIO_A11             PX_GPIO_USBD_N
#define PX_GPIO_A12             PX_GPIO_USBD_P
#define PX_GPIO_A13             PX_GPIO_SWDIO
#define PX_GPIO_A14             PX_GPIO_SWDCK

#define PX_GPIO_C0              PX_GPIO_USB_SENSE
#define PX_GPIO_C9              PX_GPIO_LED_BLUE
#define PX_GPIO_C13             PX_GPIO_BTN_BLUE
#define PX_GPIO_C14             PX_GPIO_OSC32_IN
#define PX_GPIO_C15             PX_GPIO_OSC32_OUT

/* _____GLOBAL VARIABLES_____________________________________________________ */
static const px_gpio_handle_t px_gpio_dbg_uart2_tx = {PX_GPIO_DBG_UART2_TX};
static const px_gpio_handle_t px_gpio_dbg_uart2_rx = {PX_GPIO_DBG_UART2_RX};
static const px_gpio_handle_t px_gpio_led_grn      = {PX_GPIO_LED_GRN};
static const px_gpio_handle_t px_gpio_usbd_n       = {PX_GPIO_USBD_N};
static const px_gpio_handle_t px_gpio_usbd_p       = {PX_GPIO_USBD_P};
static const px_gpio_handle_t px_gpio_swdio        = {PX_GPIO_SWDIO};
static const px_gpio_handle_t px_gpio_swdck        = {PX_GPIO_SWDCK};

static const px_gpio_handle_t px_gpio_usb_sense    = {PX_GPIO_USB_SENSE};
static const px_gpio_handle_t px_gpio_led_blue     = {PX_GPIO_LED_BLUE};
static const px_gpio_handle_t px_gpio_btn_blue     = {PX_GPIO_BTN_BLUE};
static const px_gpio_handle_t px_gpio_osc32_in     = {PX_GPIO_OSC32_IN};
static const px_gpio_handle_t px_gpio_osc32_out    = {PX_GPIO_OSC32_OUT};

#ifdef __cplusplus
}
#endif

#endif
