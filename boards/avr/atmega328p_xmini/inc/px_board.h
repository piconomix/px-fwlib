#ifndef __PX_BOARD_H__
#define __PX_BOARD_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2021 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
   
    Title:          px_board.h : ATmega328P-XMINI Board
    Author(s):      Pieter Conradie
    Creation Date:  2021-09-26

============================================================================= */
/** 
 *  @ingroup BOARDS_AVR
 *  @defgroup BOARDS_AVR_ATMEGA328P_XMINI px_board.h : ATmega328P-XMINI Board
 *  
 *  ![ATmega328P-XMINI](atmega328p_xmini.jpg)
 *   
 *  Image Copyright (c) [Microchip Technology Inc](https://www.microchip.com).
 *  
 *  File(s):
 *  - boards/avr/atmega328p_xmini/inc/px_board.h
 *  - boards/avr/atmega328p_xmini/inc/px_board_gpio.h
 *  - boards/avr/atmega328p_xmini/src/px_board.c
 *
 *  https://www.microchip.com/en-us/development-tool/ATMEGA328P-XMINI
 */   
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_board_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
/// Board name
#define BOARD_AVR_ATMEGA328P_XMINI

/**
 *  Processor frequency in Hz.
 *
 *  @note:
 *  Microcontroller is supplied with +5V by default and mEDBG will supply an
 *  external clock of 16 MHz. If the board is modified by removing R301,
 *  populating J301 and placing jumper in +3V3 position then mEDBG will supply
 *  an external clock of 8 MHz.
 */
#ifndef F_CPU
#define F_CPU           16000000ul
#endif

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/** 
 *  Initialise the board hardware.
 */
extern void px_board_init(void);

/**
 * Blocking delay for specified number of microseconds.
 *  
 * @param delay_us  Number of microseconds to block
 */
extern void px_board_delay_us(uint16_t delay_us);

/**
 *  Blocking delay for specified number of milliseconds.
 *  
 *  @param delay_ms  Number of milliseconds to block
 */
void px_board_delay_ms(uint16_t delay_ms);

/* _____MACROS_______________________________________________________________ */
/// LED (shared with SPI clock)
#define PX_LED_ON()             PX_GPIO_PIN_SET_HI(PX_GPIO_LED)
#define PX_LED_OFF()            PX_GPIO_PIN_SET_LO(PX_GPIO_LED)
#define PX_LED_TOGGLE()         PX_GPIO_PIN_TOGGLE(PX_GPIO_LED)

/// User button
#define PX_BTN_IS_PRESSED()     PX_GPIO_PIN_IS_LO(PX_GPIO_BTN)

/// @}
#ifdef __cplusplus
}
#endif

#endif
