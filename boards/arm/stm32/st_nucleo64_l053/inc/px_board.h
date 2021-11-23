#ifndef __PX_BOARD_H__
#define __PX_BOARD_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2020 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
   
    Title:          board.h : Nucleo-L053 board
    Author(s):      Pieter Conradie
    Creation Date:  2020-04-30

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"
#include "px_board_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
/// Board name
#define PX_BOARD_NUCLEO_L053

/// System clock frequency in Hz
#define PX_BOARD_SYS_CLK_HZ     32000000ul

/// Peripheral clock frequency in Hz
#define PX_BOARD_PER_CLK_HZ     PX_BOARD_SYS_CLK_HZ

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/// Initialise the board hardware
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
 *  Enable DBG module.
 *  
 */
void px_board_dbg_enable(void);

/**
 *  Disable DBG module.
 */
void px_board_dbg_disable(void);

/**
 *  Enter low power STOP mode and wait for interrupt to wake up core
 *  
 *  NB! Debug module must be disabled during STOP mode, otherwise SysTick
 *  interrupt will wake up core.
 */
void px_board_stop_mode(void);


/* _____MACROS_______________________________________________________________ */
/// LED
#define PX_USR_LED_ON()         px_gpio_out_set_hi(&px_gpio_led_grn)
#define PX_USR_LED_OFF()        px_gpio_out_set_lo(&px_gpio_led_grn)
#define PX_USR_LED_TOGGLE()     px_gpio_out_toggle(&px_gpio_led_grn)

/// User button
#define PX_USR_PB_IS_PRESSED()  px_gpio_in_is_lo(&px_gpio_btn_blue)

#ifdef __cplusplus
}
#endif

#endif
