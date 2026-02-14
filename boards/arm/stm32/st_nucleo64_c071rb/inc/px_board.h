#ifndef __PX_BOARD_H__
#define __PX_BOARD_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2026 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          px_board.h : ST Nucleo64-C071RB board
    Author(s):      Pieter Conradie
    Creation Date:  2026-02-14

============================================================================= */
/**
 *  @ingroup BOARDS_STM32
 *  @defgroup BOARDS_STM32_NUCLEO64_C071RB px_board.h : ST Nucleo64-C071RB board
 *
 *  File(s):
 *  - boards/arm/stm32/st_nucleo64_c071rb/inc/px_board.h
 *  - boards/arm/stm32/st_nucleo64_c071rb/inc/px_board_gpio.h
 *  - boards/arm/stm32/st_nucleo64_c071rb/src/px_board.c
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
#define PX_BOARD_NUCLEO64_C071RB

/// System clock frequency in Hz
#define PX_BOARD_SYS_CLK_HZ     48000000ul

/// Peripheral clock frequency in Hz
#define PX_BOARD_PER_CLK_HZ     PX_BOARD_SYS_CLK_HZ

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise the board hardware
 */
void px_board_init(void);

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
 *  Enable DBG module during SLEEP and STOP mode.
 *  
 */
void px_board_dbg_enable(void);

/**
 *  Disable DBG module.
 */
void px_board_dbg_disable(void);

/* _____MACROS_______________________________________________________________ */
/// User button (B1 on PC13)
#define PX_USR_BTN_IS_PRESSED()     px_gpio_in_is_lo(&px_gpio_btn_blue)

/// User LED (LD1 on PA5)
#define PX_USR_LED_ON()             px_gpio_out_set_hi(&px_gpio_led_grn)
#define PX_USR_LED_OFF()            px_gpio_out_set_lo(&px_gpio_led_grn)
#define PX_USR_LED_TOGGLE()         px_gpio_out_toggle(&px_gpio_led_grn)

/// Green LED (LD1 on PA5)
#define PX_USR_LED_GRN_ON()         px_gpio_out_set_hi(&px_gpio_led_grn)
#define PX_USR_LED_GRN_OFF()        px_gpio_out_set_lo(&px_gpio_led_grn)
#define PX_USR_LED_GRN_TOGGLE()     px_gpio_out_toggle(&px_gpio_led_grn)

/// Blue LED (LD2 on PC9)
#define PX_USR_LED_BLUE_ON()        px_gpio_out_set_hi(&px_gpio_led_blue)
#define PX_USR_LED_BLUE_OFF()       px_gpio_out_set_lo(&px_gpio_led_blue)
#define PX_USR_LED_BLUE_TOGGLE()    px_gpio_out_toggle(&px_gpio_led_blue)

#ifdef __cplusplus
}
#endif

/// @}
#endif
