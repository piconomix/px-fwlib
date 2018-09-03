#ifndef __PX_KBD_MATRIX_H__
#define __PX_KBD_MATRIX_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2008 Pieter Conradie <https://piconomix.com>
 
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
    
    Title:          px_kbd_matrix.h : Matrix Keyboard module
    Author(s):      Pieter Conradie
    Creation Date:  2008-11-21

============================================================================= */

/** 
 *  @ingroup DEVICES_GENERAL
 *  @defgroup PX_KBD_MATRIX px_kbd_matrix.h : Matrix Keyboard module
 *  
 *  Full featured matrix (row/column) keyboard scanning module
 *  
 *  File(s):
 *  - devices/general/px_kbd_matrix.h
 *  - devices/general/px_kbd_matrix.c
 *  
 *  This module implements or makes provision for:
 *  - Optimised matrix row/column key scanning
 *  - Debouncing of key press and release
 *  - n-key rollover
 *  - Detection and elimination of "phantom" keys
 *  - Key repetition (typematic)
 *  
 *  @see
 *  - http://en.wikipedia.org/wiki/Computer_keyboard
 *  - http://en.wikipedia.org/wiki/Rollover_(key)
 *  - http://www.dribin.org/dave/keyboard/one_html/
 *  
 *  @tip_s
 *  This module is based on "kbmatrix.h" and "kbmatrix.c" found in the
 *  [Atmel AT91 Software package](http://www.atmel.com/dyn/products/tools_card.asp?tool_id=4343)
 *  @tip_e
 *  
 *  License:
 *  
 *      @code
 *      ----------------------------------------------------------------------------
 *             ATMEL Microcontroller Software Support 
 *      ----------------------------------------------------------------------------
 *      Copyright (c) 2008, Atmel Corporation
 *  
 *      All rights reserved.
 *  
 *      Redistribution and use in source and binary forms, with or without
 *      modification, are permitted provided that the following conditions are met:
 *  
 *      - Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the disclaimer below.
 *  
 *      Atmel's name may not be used to endorse or promote products derived from
 *      this software without specific prior written permission.
 *  
 *      DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 *      IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *      MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 *      DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 *      INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *      LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 *      OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *      LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *      NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *      EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *      ----------------------------------------------------------------------------
 *      @endcode
 * @{ 
 */ 

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_list.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS _________________________________________________________ */
#ifndef PX_KBD_MATRIX_MAX_NR_KEYS
/// Maximum number of keys that can be handled by the module.
#define PX_KBD_MATRIX_MAX_NR_KEYS  56
#if ((PX_KBD_MATRIX_MAX_NR_KEYS % 8) != 0)
#error "PX_KBD_MATRIX_MAX_NR_KEYS must be a multiple of 8"
#endif
#endif

#ifndef PX_KBD_MATRIX_NR_SAMPLES
/// Number of samples that are necessary to debounce a keypress.
#define PX_KBD_MATRIX_NR_SAMPLES  4
#endif

#ifndef PX_KBD_MATRIX_MAX_NR_KEYS_PRESSED
/// Maximum number of keys that may be pressed simultaneously
#define PX_KBD_MATRIX_MAX_NR_KEYS_PRESSED 4
#endif

#ifndef KBD_DELAY_BEFORE_REPEATING_KEY
/// Delay before a pressed key will be repeated
#define KBD_DELAY_BEFORE_REPEATING_KEY 200
#endif

#ifndef KBD_KEY_REPEAT_DELAY
/// Delay between pressed key repeats
#define KBD_KEY_REPEAT_DELAY 20
#endif

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Key event types
typedef enum
{
    PX_KBD_MATRIX_EVENT_RELEASED = 0,  ///< Key has been released
    PX_KBD_MATRIX_EVENT_PRESSED  = 1   ///< Key has been pressed
} px_kbd_matrix_event_t;

/**
 * Function that will be called when a key event has taken place
 *  
 * @param[in] row       Row address of key event 
 * @param[in] col       Column address of key event 
 * @param[in] event     Key event type: PRESSED or RELEASED
 *  
 */
typedef void (*px_kbd_matrix_on_key_event_t)(uint8_t row, uint8_t col, px_kbd_matrix_event_t event);

/**
 * Function that will be called to set a row output 
 *  
 * @param[in] row       Specifies which row that should be set
 * @param[in] active    Specifies that row should be activated 
 *                      (normally active low)
 */
typedef void (*px_kbd_matrix_set_row_t)(uint8_t row, bool active);

/**
 * Function that will be called to read a column input
 *  
 * @retval true     Key is being pressed
 * @retval false    Key is not pressed (released)
 */
typedef bool (*px_kbd_matrix_get_col_t)(uint8_t col);

/**
 * Function that will be called for row column scanning delay. 
 *  
 * If a row is set to active, there is a certain delay before the columns should 
 * be sampled. 
 *  
 * If a row is set to inactive, there is a certain delay before the columns 
 * return to their inactive state. 
 *  
 * @param[in] active    Specifies that row should be activated 
 *                      (normally active low)
 */
typedef void (*px_kbd_matrix_delay_t)(bool active);

/// Key state bit mask (1 = key pressed; 0 = key released)
typedef struct
{
    uint8_t bit_mask[PX_KBD_MATRIX_MAX_NR_KEYS/8];
} px_kbd_matrix_key_state_t;

/// Linked list structure that describes which key has been pressed
typedef struct
{
    px_list_item_t key_list_item;
    uint8_t        row;
    uint8_t        col;
} px_kbd_matrix_key_t;

/// Keyboard matrix definition and state
typedef struct
{
    /// Number of rows in matrix
    uint8_t                      nr_of_rows;
    /// Number of columns in matrix
    uint8_t                      nr_of_columns;

    /// Bit mask that stores the current state of the keys
    px_kbd_matrix_key_state_t    key_state;

    /// Array of bit masks that stores the key state history
    px_kbd_matrix_key_state_t    key_history[PX_KBD_MATRIX_NR_SAMPLES];

    /// Wrapping index (used as a ring buffer)
    uint8_t                      key_history_index;

    /// Linked list of pressed keys
    px_list_t                    key_press_list;

    /// Storage for linked list to remember which keys are pressed
    px_kbd_matrix_key_t          key_press_array[PX_KBD_MATRIX_MAX_NR_KEYS_PRESSED];

    // Counter to determine if a pressed key must repeated (typematic)
    uint16_t                     key_repeat_counter;

    /// Function handler to write a row output
    px_kbd_matrix_set_row_t      set_row;

    /// Function handler to read a column input
    px_kbd_matrix_get_col_t      get_col;

    /// Function handler to read a column input
    px_kbd_matrix_delay_t        delay;

    /// Function that will be called if a key is pressed or released
    px_kbd_matrix_on_key_event_t on_key_event_handler;
} px_kbd_matrix_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
void px_kbd_matrix_init(px_kbd_matrix_t *            matrix,
                        uint8_t                      nr_of_rows,
                        uint8_t                      nr_of_columns,
                        px_kbd_matrix_set_row_t      set_row,
                        px_kbd_matrix_get_col_t      get_col,
                        px_kbd_matrix_delay_t        delay,
                        px_kbd_matrix_on_key_event_t on_key_event_handler);

void px_kbd_matrix_scan(px_kbd_matrix_t * matrix);

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif // #ifndef __PX_KBD_MATRIX_H__
