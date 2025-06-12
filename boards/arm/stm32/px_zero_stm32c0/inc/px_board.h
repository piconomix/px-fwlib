#ifndef __PX_BOARD_H__
#define __PX_BOARD_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2025 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          board.h : PX-ZER0 STM32C0 Board
    Author(s):      Pieter Conradie
    Creation Date:  2025-05-24

============================================================================= */
/** 
 *  @ingroup BOARDS_STM32
 *  @defgroup BOARDS_STM32_PX_ZERO_STM32C0 px_board.h : PX-ZER0 STM32C0 Board
 *  
 *  File(s):
 *  - boards/arm/stm32/px_zero_stm32c0/inc/px_board.h
 *  - boards/arm/stm32/px_zero_stm32c0/inc/px_board_gpio.h
 *  - boards/arm/stm32/px_zero_stm32c0/src/px_board.c
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"
#include "px_board_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
/// Board name
#define PX_BOARD_PX_ZERO_STM32C0

/// System clock frequency in Hz
#define PX_BOARD_SYS_CLK_HZ     48000000ul

/// Peripheral clock frequency in Hz
#define PX_BOARD_PER_CLK_HZ     PX_BOARD_SYS_CLK_HZ

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Identify which GPIO pins are used for CS in px_board_spi_cs_lo() and px_board_spi_cs_hi()
typedef enum
{
    PX_BOARD_SPI_CS_NONE,      ///< Do not perform a chip select
    PX_BOARD_SPI1_CS,          ///< Peripheral CS
    PX_BOARD_SPI1_CS_SF,       ///< Adesto AT25S Serial FLASH
} px_board_spi_cs_t;

/// @name Mapping of board's ADC pins to ADC channels
/// @{
#define PX_BOARD_ADC0       PX_ADC_CH8
#define PX_BOARD_ADC1       PX_ADC_CH19
#define PX_BOARD_ADC2       PX_ADC_CH18
#define PX_BOARD_ADC3       PX_ADC_CH17
/// @}

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/// Initialise the board hardware
void px_board_init(void);

/**
 *  Set specified SPI Chip Select low
 *  
 *  @param cs_id    ID which GPIO pin must be used for Chip Select
 */
void px_board_spi_cs_lo(uint8_t cs_id);

/**
 *  Set specified SPI Chip Select high
 *  
 *  @param cs_id    ID which GPIO pin must be used for Chip Select
 */
void px_board_spi_cs_hi(uint8_t cs_id);

/**
 *  Initialise Timer 6 with 1 us clock ticks for delay use
 *  
 */
void px_board_delay_init(void);

/**
 *  Disable Timer 6
 *  
 */
void px_board_delay_deinit(void);

/**
 *  Blocking delay for specified number of microseconds.
 *  
 *  @param delay_us  Number of microseconds to block
 */
void px_board_delay_us(uint16_t delay_us);

/**
 *  Blocking delay for specified number of milliseconds.
 *  
 *  @param delay_ms  Number of milliseconds to block
 */
void px_board_delay_ms(uint16_t delay_ms);

/**
 *  Enable PWM output to piezo buzzer at specified frequency.
 *  
 *  @param freq_hz PWM frequency in Hz
 */
void px_board_buzzer_on(uint16_t freq_hz);

/** 
 *  Disable PWM output to piezo buzzer.
 */
void px_board_buzzer_off(void);

/**
 *  Enable DBG module during SLEEP and STOP mode.
 *  
 */
void px_board_dbg_enable(void);

/**
 *  Disable DBG module.
 */
void px_board_dbg_disable(void);

/* _____MACROS_______________________________________________________________ */
/// User button
#define PX_USR_BTN_IS_PRESSED()     px_gpio_in_is_lo(&px_gpio_usr_btn)

/// User LED
#define PX_USR_LED_ON()             px_gpio_out_set_hi(&px_gpio_usr_led)
#define PX_USR_LED_OFF()            px_gpio_out_set_lo(&px_gpio_usr_led)
#define PX_USR_LED_TOGGLE()         px_gpio_out_toggle(&px_gpio_usr_led)

#ifdef __cplusplus
}
#endif

/// @}
#endif
