#ifndef __PX_BOARD_GPIO_H__
#define __PX_BOARD_GPIO_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
   
    Title:          board.h : Arduino UNO R3 Board GPIO pin definitions
    Author(s):      Pieter Conradie
    Creation Date:  2018-06-03

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
#define PX_GPIO_D8          PX_GPIO(B, 0, PX_GPIO_DIR_IN,  PX_GPIO_INIT_PULLUP)
#define PX_GPIO_D9          PX_GPIO(B, 1, PX_GPIO_DIR_IN,  PX_GPIO_INIT_PULLUP)                          
#define PX_GPIO_SPI_SS      PX_GPIO(B, 2, PX_GPIO_DIR_OUT, PX_GPIO_INIT_HI)
#define PX_GPIO_SPI_MOSI    PX_GPIO(B, 3, PX_GPIO_DIR_OUT, PX_GPIO_INIT_LO)
#define PX_GPIO_SPI_MISO    PX_GPIO(B, 4, PX_GPIO_DIR_IN,  PX_GPIO_INIT_HIZ)
#define PX_GPIO_SPI_SCK     PX_GPIO(B, 5, PX_GPIO_DIR_OUT, PX_GPIO_INIT_LO)
#define PX_GPIO_LED         PX_GPIO(B, 5, PX_GPIO_DIR_OUT, PX_GPIO_INIT_LO)
                            
#define PX_GPIO_A0          PX_GPIO(C, 0, PX_GPIO_DIR_IN, PX_GPIO_INIT_HIZ)
#define PX_GPIO_A1          PX_GPIO(C, 1, PX_GPIO_DIR_IN, PX_GPIO_INIT_HIZ)
#define PX_GPIO_A2          PX_GPIO(C, 2, PX_GPIO_DIR_IN, PX_GPIO_INIT_HIZ)
#define PX_GPIO_A3          PX_GPIO(C, 3, PX_GPIO_DIR_IN, PX_GPIO_INIT_HIZ)
#define PX_GPIO_I2C_SDA     PX_GPIO(C, 4, PX_GPIO_DIR_IN, PX_GPIO_INIT_PULLUP)
#define PX_GPIO_I2C_SCL     PX_GPIO(C, 5, PX_GPIO_DIR_IN, PX_GPIO_INIT_PULLUP)

#define PX_GPIO_UART_RXD    PX_GPIO(D, 0, PX_GPIO_DIR_IN, PX_GPIO_INIT_PULLUP)
#define PX_GPIO_UART_TXD    PX_GPIO(D, 1, PX_GPIO_DIR_OUT, PX_GPIO_INIT_HI)
#define PX_GPIO_D2          PX_GPIO(D, 2, PX_GPIO_DIR_IN,  PX_GPIO_INIT_PULLUP)
#define PX_GPIO_D3          PX_GPIO(D, 3, PX_GPIO_DIR_IN,  PX_GPIO_INIT_PULLUP)
#define PX_GPIO_D4          PX_GPIO(D, 4, PX_GPIO_DIR_IN,  PX_GPIO_INIT_PULLUP)
#define PX_GPIO_D5          PX_GPIO(D, 5, PX_GPIO_DIR_IN,  PX_GPIO_INIT_PULLUP)
#define PX_GPIO_D6          PX_GPIO(D, 6, PX_GPIO_DIR_IN,  PX_GPIO_INIT_PULLUP)
#define PX_GPIO_D7          PX_GPIO(D, 7, PX_GPIO_DIR_IN,  PX_GPIO_INIT_PULLUP)

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
static const px_gpio_handle_t px_gpio_d8 =       {PX_GPIO_D8};
static const px_gpio_handle_t px_gpio_d9 =       {PX_GPIO_D9};
static const px_gpio_handle_t px_gpio_spi_ss =   {PX_GPIO_SPI_SS};
static const px_gpio_handle_t px_gpio_spi_mosi = {PX_GPIO_SPI_MOSI};
static const px_gpio_handle_t px_gpio_spi_miso = {PX_GPIO_SPI_MISO};
static const px_gpio_handle_t px_gpio_spi_sck =  {PX_GPIO_SPI_SCK};
static const px_gpio_handle_t px_gpio_led =      {PX_GPIO_LED};
                            
static const px_gpio_handle_t px_gpio_a0 =       {PX_GPIO_A0};
static const px_gpio_handle_t px_gpio_a1 =       {PX_GPIO_A1};
static const px_gpio_handle_t px_gpio_a2 =       {PX_GPIO_A2};
static const px_gpio_handle_t px_gpio_a3 =       {PX_GPIO_A3};
static const px_gpio_handle_t px_gpio_i2c_sda =  {PX_GPIO_I2C_SDA};
static const px_gpio_handle_t px_gpio_i2c_scl =  {PX_GPIO_I2C_SCL};

static const px_gpio_handle_t px_gpio_uart_rxd = {PX_GPIO_UART_RXD};
static const px_gpio_handle_t px_gpio_uart_txd = {PX_GPIO_UART_TXD};
static const px_gpio_handle_t px_gpio_d2 =       {PX_GPIO_D2};
static const px_gpio_handle_t px_gpio_d3 =       {PX_GPIO_D3};
static const px_gpio_handle_t px_gpio_d4 =       {PX_GPIO_D4};
static const px_gpio_handle_t px_gpio_d5 =       {PX_GPIO_D5};
static const px_gpio_handle_t px_gpio_d6 =       {PX_GPIO_D6};
static const px_gpio_handle_t px_gpio_d7 =       {PX_GPIO_D7};

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */

/* _____MACROS_______________________________________________________________ */
#define PX_GPIO_B0 PX_GPIO_D8
#define PX_GPIO_B1 PX_GPIO_D9
#define PX_GPIO_B2 PX_GPIO_SPI_SS
#define PX_GPIO_B3 PX_GPIO_SPI_MOSI
#define PX_GPIO_B4 PX_GPIO_SPI_MISO
#define PX_GPIO_B5 PX_GPIO_SPI_SCK
#define PX_GPIO_B6 PX_GPIO_LED

#define PX_GPIO_C0 PX_GPIO_A0
#define PX_GPIO_C1 PX_GPIO_A1
#define PX_GPIO_C2 PX_GPIO_A2
#define PX_GPIO_C3 PX_GPIO_A3
#define PX_GPIO_C4 PX_GPIO_I2C_SDA
#define PX_GPIO_C5 PX_GPIO_I2C_SCL

#define PX_GPIO_D0 PX_GPIO_UART_RXD
#define PX_GPIO_D1 PX_GPIO_UART_TXD
//#define PX_GPIO_D2 PX_GPIO_D2
//#define PX_GPIO_D3 PX_GPIO_D3
//#define PX_GPIO_D4 PX_GPIO_D4
//#define PX_GPIO_D5 PX_GPIO_D5
//#define PX_GPIO_D6 PX_GPIO_D6
//#define PX_GPIO_D7 PX_GPIO_D7

#ifdef __cplusplus
}
#endif

#endif
