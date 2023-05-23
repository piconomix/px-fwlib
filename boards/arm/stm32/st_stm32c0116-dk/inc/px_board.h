#ifndef __PX_BOARD_H__
#define __PX_BOARD_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2023 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          board.h : STM32C0116-DK board
    Author(s):      Pieter Conradie
    Creation Date:  2023-05-20

============================================================================= */

/**
 *  @ingroup BOARDS_STM32
 *  @defgroup BOARDS_STM32_STM32C0116_DK px_board.h : STM32C0116-DK board
 *
 *  File(s):
 *  - boards/arm/stm32/st_stm32c0116_dk/inc/px_board.h
 *  - boards/arm/stm32/st_stm32c0116_dk/inc/px_board_gpio.h
 *  - boards/arm/stm32/st_stm32c0116_dk/src/px_board.c
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
#define PX_BOARD_STM32C0116_DK

/// System clock frequency in Hz
#define PX_BOARD_SYS_CLK_HZ     48000000ul

/// Peripheral clock frequency in Hz
#define PX_BOARD_PER_CLK_HZ     PX_BOARD_SYS_CLK_HZ

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/// Initialise the board hardware
void px_board_init(void);

/**
 *  Initialise Timer 3 with 1 us clock ticks for delay use
 *  
 */
void px_board_delay_init(void);

/**
 *  Disable Timer 3
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
 *  Enable DBG module.
 *  
 */
void px_board_dbg_enable(void);

/**
 *  Disable DBG module.
 */
void px_board_dbg_disable(void);

/* _____MACROS_______________________________________________________________ */
/// LED
#define PX_USR_LED_ON()         px_gpio_out_set_lo(&px_gpio_led)
#define PX_USR_LED_OFF()        px_gpio_out_set_hi(&px_gpio_led)
#define PX_USR_LED_TOGGLE()     px_gpio_out_toggle(&px_gpio_led)

#ifdef __cplusplus
}
#endif

/// @}
#endif
