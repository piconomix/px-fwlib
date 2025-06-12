#ifndef __PX_BOARD_GPIO_H__
#define __PX_BOARD_GPIO_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2025 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
   
    Title:          board.h : PX-ZER0 STM32C0 Board GPIO pin definitions
    Author(s):      Pieter Conradie
    Creation Date:  2025-05-24

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"
#include "px_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
#define PX_GPIO_UART1_TX        PX_GPIO(A,  0, PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_LO, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_HI, PX_GPIO_AF_4)
#define PX_GPIO_UART1_RX        PX_GPIO(A,  1, PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_UP, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_4)
#define PX_GPIO_DBG_UART2_TX    PX_GPIO(A,  2, PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_LO, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_HI, PX_GPIO_AF_1)
#define PX_GPIO_DBG_UART2_RX    PX_GPIO(A,  3, PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_UP, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_1)
#define PX_GPIO_SPI1_CS         PX_GPIO(A,  4, PX_GPIO_MODE_OUT, PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_LO, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_HI, PX_GPIO_AF_NA)
#define PX_GPIO_SPI1_SCK        PX_GPIO(A,  5, PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_HI, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_LO, PX_GPIO_AF_0)
#define PX_GPIO_SPI1_MISO       PX_GPIO(A,  6, PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_DN, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_0)
#define PX_GPIO_SPI1_MOSI       PX_GPIO(A,  7, PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_HI, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_LO, PX_GPIO_AF_0)
#define PX_GPIO_ADC0            PX_GPIO(A,  8, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#define PX_GPIO_SPI1_CS_SF      PX_GPIO(A,  9, PX_GPIO_MODE_OUT, PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_LO, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_HI, PX_GPIO_AF_NA)
#define PX_GPIO_USR_BTN         PX_GPIO(A, 10, PX_GPIO_MODE_IN,  PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_UP, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#define PX_GPIO_USBD_N          PX_GPIO(A, 11, PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_VH, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_10)
#define PX_GPIO_USBD_P          PX_GPIO(A, 12, PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_VH, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_10)
#define PX_GPIO_SWDIO           PX_GPIO(A, 13, PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_HI, PX_GPIO_PULL_UP, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_0)
#define PX_GPIO_SWDCK           PX_GPIO(A, 14, PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_DN, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_0)
#define PX_GPIO_USR_LED         PX_GPIO(A, 15, PX_GPIO_MODE_OUT, PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_LO, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_LO, PX_GPIO_AF_NA)

#define PX_GPIO_ADC3            PX_GPIO(B,  0, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#define PX_GPIO_ADC2            PX_GPIO(B,  1, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#define PX_GPIO_ADC1            PX_GPIO(B,  2, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#define PX_GPIO_3               PX_GPIO(B,  3, PX_GPIO_MODE_IN,  PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_LO, PX_GPIO_PULL_DN, PX_GPIO_OUT_INIT_LO, PX_GPIO_AF_NA)
#define PX_GPIO_PWM             PX_GPIO(B,  4, PX_GPIO_MODE_OUT, PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_LO, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_LO, PX_GPIO_AF_1)
#define PX_GPIO_2               PX_GPIO(B,  5, PX_GPIO_MODE_IN,  PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_LO, PX_GPIO_PULL_DN, PX_GPIO_OUT_INIT_LO, PX_GPIO_AF_NA)
#define PX_GPIO_1               PX_GPIO(B,  6, PX_GPIO_MODE_IN,  PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_LO, PX_GPIO_PULL_DN, PX_GPIO_OUT_INIT_LO, PX_GPIO_AF_NA)
#define PX_GPIO_I2C1_SDA        PX_GPIO(B,  7, PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_OD, PX_GPIO_OSPEED_LO, PX_GPIO_PULL_UP, PX_GPIO_OUT_INIT_HI, PX_GPIO_AF_6)
#define PX_GPIO_I2C1_SCL        PX_GPIO(B,  8, PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_OD, PX_GPIO_OSPEED_LO, PX_GPIO_PULL_UP, PX_GPIO_OUT_INIT_HI, PX_GPIO_AF_6)
#define PX_GPIO_0               PX_GPIO(B,  9, PX_GPIO_MODE_IN,  PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_LO, PX_GPIO_PULL_DN, PX_GPIO_OUT_INIT_LO, PX_GPIO_AF_NA)

#define PX_GPIO_PWM_BUZZER      PX_GPIO(C,  6, PX_GPIO_MODE_OUT, PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_LO, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_LO, PX_GPIO_AF_3)
#define PX_GPIO_OSC32_IN        PX_GPIO(C, 14, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#define PX_GPIO_OSC32_OUT       PX_GPIO(C, 15, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)

#define PX_GPIO_A0              PX_GPIO_UART1_TX
#define PX_GPIO_A1              PX_GPIO_UART1_RX
#define PX_GPIO_A2              PX_GPIO_DBG_UART2_TX
#define PX_GPIO_A3              PX_GPIO_DBG_UART2_RX
#define PX_GPIO_A4              PX_GPIO_SPI1_CS
#define PX_GPIO_A5              PX_GPIO_SPI1_SCK
#define PX_GPIO_A6              PX_GPIO_SPI1_MISO
#define PX_GPIO_A7              PX_GPIO_SPI1_MOSI
#define PX_GPIO_A8              PX_GPIO_ADC0
#define PX_GPIO_A9              PX_GPIO_SPI1_CS_SF
#define PX_GPIO_A10             PX_GPIO_USR_BTN
#define PX_GPIO_A11             PX_GPIO_USBD_N
#define PX_GPIO_A12             PX_GPIO_USBD_P
#define PX_GPIO_A13             PX_GPIO_SWDIO
#define PX_GPIO_A14             PX_GPIO_SWDCK
#define PX_GPIO_A15             PX_GPIO_USR_LED

#define PX_GPIO_B0              PX_GPIO_ADC3
#define PX_GPIO_B1              PX_GPIO_ADC2
#define PX_GPIO_B2              PX_GPIO_ADC1
#define PX_GPIO_B3              PX_GPIO_3
#define PX_GPIO_B4              PX_GPIO_PWM
#define PX_GPIO_B5              PX_GPIO_2
#define PX_GPIO_B6              PX_GPIO_1
#define PX_GPIO_B7              PX_GPIO_I2C1_SDA
#define PX_GPIO_B8              PX_GPIO_I2C1_SCL
#define PX_GPIO_B9              PX_GPIO_0

#define PX_GPIO_C6              PX_GPIO_PWM_BUZZER
#define PX_GPIO_C14             PX_GPIO_OSC32_IN
#define PX_GPIO_C15             PX_GPIO_OSC32_OUT

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
static const px_gpio_handle_t px_gpio_uart1_tx     = {PX_GPIO_UART1_TX};
static const px_gpio_handle_t px_gpio_uart1_rx     = {PX_GPIO_UART1_RX};
static const px_gpio_handle_t px_gpio_dbg_uart2_tx = {PX_GPIO_DBG_UART2_TX};
static const px_gpio_handle_t px_gpio_dbg_uart2_rx = {PX_GPIO_DBG_UART2_RX};
static const px_gpio_handle_t px_gpio_spi1_cs      = {PX_GPIO_SPI1_CS};
static const px_gpio_handle_t px_gpio_spi1_sck     = {PX_GPIO_SPI1_SCK};
static const px_gpio_handle_t px_gpio_spi1_miso    = {PX_GPIO_SPI1_MISO};
static const px_gpio_handle_t px_gpio_spi1_mosi    = {PX_GPIO_SPI1_MOSI};
static const px_gpio_handle_t px_gpio_adc0         = {PX_GPIO_ADC0};
static const px_gpio_handle_t px_gpio_spi1_cs_sf   = {PX_GPIO_SPI1_CS_SF};
static const px_gpio_handle_t px_gpio_usr_btn      = {PX_GPIO_USR_BTN};
static const px_gpio_handle_t px_gpio_usbd_n       = {PX_GPIO_USBD_N};
static const px_gpio_handle_t px_gpio_usbd_p       = {PX_GPIO_USBD_P};
static const px_gpio_handle_t px_gpio_swdio        = {PX_GPIO_SWDIO};
static const px_gpio_handle_t px_gpio_swdck        = {PX_GPIO_SWDCK};
static const px_gpio_handle_t px_gpio_usr_led      = {PX_GPIO_USR_LED};

static const px_gpio_handle_t px_gpio_adc3         = {PX_GPIO_ADC3};
static const px_gpio_handle_t px_gpio_adc2         = {PX_GPIO_ADC2};
static const px_gpio_handle_t px_gpio_adc1         = {PX_GPIO_ADC1};
static const px_gpio_handle_t px_gpio_3            = {PX_GPIO_3};
static const px_gpio_handle_t px_gpio_pwm          = {PX_GPIO_PWM};
static const px_gpio_handle_t px_gpio_2            = {PX_GPIO_2};
static const px_gpio_handle_t px_gpio_1            = {PX_GPIO_1};
static const px_gpio_handle_t px_gpio_i2c1_sda     = {PX_GPIO_I2C1_SDA};
static const px_gpio_handle_t px_gpio_i2c1_scl     = {PX_GPIO_I2C1_SCL};
static const px_gpio_handle_t px_gpio_0            = {PX_GPIO_0};

static const px_gpio_handle_t px_gpio_pwm_buzzer   = {PX_GPIO_PWM_BUZZER};
static const px_gpio_handle_t px_gpio_osc32_in     = {PX_GPIO_OSC32_IN};
static const px_gpio_handle_t px_gpio_osc32_out    = {PX_GPIO_OSC32_OUT};

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

#endif
