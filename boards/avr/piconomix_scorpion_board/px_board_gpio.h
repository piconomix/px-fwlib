#ifndef __PX_BOARD_GPIO_H__
#define __PX_BOARD_GPIO_H__
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
   
    Title:          board.h : Piconomix ATmega328P Scorpion Board GPIO pin definitions
    Author(s):      Pieter Conradie
    Creation Date:  2018-05-28

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS _________________________________________________________ */
#define PX_GPIO_LED        PX_GPIO(B, 0, PX_GPIO_DIR_OUT, PX_GPIO_INIT_LO)
#define PX_GPIO_SPI_CS_DF  PX_GPIO(B, 1, PX_GPIO_DIR_OUT, PX_GPIO_INIT_HI)
#define PX_GPIO_SPI_CS     PX_GPIO(B, 2, PX_GPIO_DIR_OUT, PX_GPIO_INIT_HI)
#define PX_GPIO_SPI_MOSI   PX_GPIO(B, 3, PX_GPIO_DIR_OUT, PX_GPIO_INIT_LO)
#define PX_GPIO_SPI_MISO   PX_GPIO(B, 4, PX_GPIO_DIR_IN,  PX_GPIO_INIT_HIZ)
#define PX_GPIO_SPI_SCK    PX_GPIO(B, 5, PX_GPIO_DIR_OUT, PX_GPIO_INIT_LO)

#define PX_GPIO_ADC1       PX_GPIO(C, 0, PX_GPIO_DIR_IN,  PX_GPIO_INIT_HIZ)
#define PX_GPIO_ADC0       PX_GPIO(C, 1, PX_GPIO_DIR_IN,  PX_GPIO_INIT_HIZ)
#define PX_GPIO_4          PX_GPIO(C, 2, PX_GPIO_DIR_IN,  PX_GPIO_INIT_PULLUP)
#define PX_GPIO_3          PX_GPIO(C, 3, PX_GPIO_DIR_IN,  PX_GPIO_INIT_PULLUP)
#define PX_GPIO_I2C_SDA    PX_GPIO(C, 4, PX_GPIO_DIR_IN,  PX_GPIO_INIT_PULLUP)
#define PX_GPIO_I2C_SCL    PX_GPIO(C, 5, PX_GPIO_DIR_IN,  PX_GPIO_INIT_PULLUP)

#define PX_GPIO_SER_RXD    PX_GPIO(D, 0, PX_GPIO_DIR_IN,  PX_GPIO_INIT_PULLUP)
#define PX_GPIO_SER_TXD    PX_GPIO(D, 1, PX_GPIO_DIR_OUT, PX_GPIO_INIT_HI)
#define PX_GPIO_2          PX_GPIO(D, 2, PX_GPIO_DIR_IN,  PX_GPIO_INIT_PULLUP)
#define PX_GPIO_1          PX_GPIO(D, 3, PX_GPIO_DIR_IN,  PX_GPIO_INIT_PULLUP)
#define PX_GPIO_0          PX_GPIO(D, 4, PX_GPIO_DIR_IN,  PX_GPIO_INIT_PULLUP)
#define PX_GPIO_UART_SEL   PX_GPIO(D, 5, PX_GPIO_DIR_OUT, PX_GPIO_INIT_HI)
#define PX_GPIO_PWM_BUZZER PX_GPIO(D, 6, PX_GPIO_DIR_OUT, PX_GPIO_INIT_LO)
#define PX_GPIO_PB         PX_GPIO(D, 7, PX_GPIO_DIR_IN,  PX_GPIO_INIT_PULLUP)

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
static const px_gpio_handle_t px_gpio_led           = {PX_GPIO_LED};
static const px_gpio_handle_t px_gpio_spi_cs_df     = {PX_GPIO_SPI_CS_DF};
static const px_gpio_handle_t px_gpio_spi_cs        = {PX_GPIO_SPI_CS};
static const px_gpio_handle_t px_gpio_spi_mosi      = {PX_GPIO_SPI_MOSI};
static const px_gpio_handle_t px_gpio_spi_miso      = {PX_GPIO_SPI_MISO};
static const px_gpio_handle_t px_gpio_spi_sck       = {PX_GPIO_SPI_SCK};
                                                    
static const px_gpio_handle_t px_gpio_adc1          = {PX_GPIO_ADC1};
static const px_gpio_handle_t px_gpio_adc0          = {PX_GPIO_ADC0};
static const px_gpio_handle_t px_gpio_4             = {PX_GPIO_4};
static const px_gpio_handle_t px_gpio_3             = {PX_GPIO_3};
static const px_gpio_handle_t px_gpio_i2c_sda       = {PX_GPIO_I2C_SDA};
static const px_gpio_handle_t px_gpio_i2c_scl       = {PX_GPIO_I2C_SCL};
                                                    
static const px_gpio_handle_t px_gpio_ser_rxd       = {PX_GPIO_SER_RXD};
static const px_gpio_handle_t px_gpio_ser_txd       = {PX_GPIO_SER_TXD};
static const px_gpio_handle_t px_gpio_2             = {PX_GPIO_2};
static const px_gpio_handle_t px_gpio_1             = {PX_GPIO_1};
static const px_gpio_handle_t px_gpio_0             = {PX_GPIO_0};
static const px_gpio_handle_t px_gpio_uart_sel      = {PX_GPIO_UART_SEL};
static const px_gpio_handle_t px_gpio_pwm_buzzer    = {PX_GPIO_PWM_BUZZER};
static const px_gpio_handle_t px_gpio_pb            = {PX_GPIO_PB};

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */

/* _____MACROS_______________________________________________________________ */
#define GPIO_PORTB_INIT_VAL \
              PX_GPIO_INIT_PORT(PX_GPIO_LED) \
            + PX_GPIO_INIT_PORT(PX_GPIO_SPI_CS_DF) \
            + PX_GPIO_INIT_PORT(PX_GPIO_SPI_CS) \
            + PX_GPIO_INIT_PORT(PX_GPIO_SPI_MOSI) \
            + PX_GPIO_INIT_PORT(PX_GPIO_SPI_MISO) \
            + PX_GPIO_INIT_PORT(PX_GPIO_SPI_SCK)
      
#define GPIO_DDRB_INIT_VAL \
              PX_GPIO_INIT_DDR(PX_GPIO_LED) \
            + PX_GPIO_INIT_DDR(PX_GPIO_SPI_CS_DF) \
            + PX_GPIO_INIT_DDR(PX_GPIO_SPI_CS) \
            + PX_GPIO_INIT_DDR(PX_GPIO_SPI_MOSI) \
            + PX_GPIO_INIT_DDR(PX_GPIO_SPI_MISO) \
            + PX_GPIO_INIT_DDR(PX_GPIO_SPI_SCK)

#define GPIO_PORTC_INIT_VAL \
              PX_GPIO_INIT_PORT(PX_GPIO_ADC1) \
            + PX_GPIO_INIT_PORT(PX_GPIO_ADC0) \
            + PX_GPIO_INIT_PORT(PX_GPIO_4) \
            + PX_GPIO_INIT_PORT(PX_GPIO_3) \
            + PX_GPIO_INIT_PORT(PX_GPIO_I2C_SDA) \
            + PX_GPIO_INIT_PORT(PX_GPIO_I2C_SCL)

#define GPIO_DDRC_INIT_VAL \
              PX_GPIO_INIT_DDR(PX_GPIO_ADC1) \
            + PX_GPIO_INIT_DDR(PX_GPIO_ADC0) \
            + PX_GPIO_INIT_DDR(PX_GPIO_4) \
            + PX_GPIO_INIT_DDR(PX_GPIO_3) \
            + PX_GPIO_INIT_DDR(PX_GPIO_I2C_SDA) \
            + PX_GPIO_INIT_DDR(PX_GPIO_I2C_SCL)

#define GPIO_PORTD_INIT_VAL \
              PX_GPIO_INIT_PORT(PX_GPIO_SER_RXD) \
            + PX_GPIO_INIT_PORT(PX_GPIO_SER_TXD) \
            + PX_GPIO_INIT_PORT(PX_GPIO_2) \
            + PX_GPIO_INIT_PORT(PX_GPIO_1) \
            + PX_GPIO_INIT_PORT(PX_GPIO_0) \
            + PX_GPIO_INIT_PORT(PX_GPIO_UART_SEL) \
            + PX_GPIO_INIT_PORT(PX_GPIO_PWM_BUZZER) \
            + PX_GPIO_INIT_PORT(PX_GPIO_PB)

#define GPIO_DDRD_INIT_VAL \
              PX_GPIO_INIT_DDR(PX_GPIO_SER_RXD) \
            + PX_GPIO_INIT_DDR(PX_GPIO_SER_TXD) \
            + PX_GPIO_INIT_DDR(PX_GPIO_2) \
            + PX_GPIO_INIT_DDR(PX_GPIO_1) \
            + PX_GPIO_INIT_DDR(PX_GPIO_0) \
            + PX_GPIO_INIT_DDR(PX_GPIO_UART_SEL) \
            + PX_GPIO_INIT_DDR(PX_GPIO_PWM_BUZZER) \
            + PX_GPIO_INIT_DDR(PX_GPIO_PB)

#ifdef __cplusplus
}
#endif

#endif // #ifndef __PX_BOARD_GPIO_H__
