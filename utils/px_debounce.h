#ifndef __PX_DEBOUNCE_H__
#define __PX_DEBOUNCE_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2015 Pieter Conradie <https://piconomix.com>
 
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
 
    Title:          px_debounce.h : Digital debounce module
    Author(s):      Pieter Conradie
    Creation Date:  2015-08-21

============================================================================= */

/** 
 *  @ingroup UTILS
 *  @defgroup PX_DEBOUNCE px_debounce.h : Digital debounce module
 *  
 *  This module debounces a digital input and remembers a rising or falling edge.
 *  
 *  File(s):
 *  - utils/px_debounce.h
 *  - utils/px_debounce_cfg_template.h
 *  - utils/px_debounce.c
 *  
 *  This module must be configured by supplying a project specific
 *  "px_debounce_cfg.h". "px_debounce_cfg_template.h" can be copied, renamed and
 *  modified to supply compile time options.
 *  
 *  It is ideal for a noisy digital input such as a button. When a key is
 *  pressed, the input will "bounce" between high and low until it finally
 *  settles to a valid state.
 *  
 *  @see https://en.wikipedia.org/wiki/Switch#Contact_bounce
 *  
 *  This module is able to "debounce" a digital input by looking at successive
 *  values and deciding if it is a valid low or high state. It is also able to
 *  register and remember a rising and falling edge event, e.g. "key pressed
 *  event" or "key released event".    
 *  
 *  A counter is incremented each time the current digital input state is HI.
 *  If the counter reaches the high threshold, the debounced state is considered
 *  HI. Conversely, the counter is decremented each time the current digital
 *  input state is LO. If the counter reaches the low threshold, the debounced
 *  state is considered LO. The minimum counter value is 0 and the maximum value
 *  is PX_DEBOUNCE_CFG_COUNT_MAX. This scheme provides sufficient hysteresis to
 *  debounce a noisy digital input.
 *  
 *  If PX_DEBOUNCE_CFG_FIXED is 1, then the parameters are fixed at compile time
 *  for all debounce state tracking structure and set with
 *  PX_DEBOUNCE_CFG_COUNT_MAX, PX_DEBOUNCE_CFG_THRESHOLD_LO and
 *  PX_DEBOUNCE_CFG_THRESHOLD_HI.
 *  
 *  If PX_DEBOUNCE_CFG_FIXED is 0, then the parameters can be customised during
 *  run time for each debounce tracking structure.
 *  
 *  If no hysteresis is required, then PX_DEBOUNCE_CFG_THRESHOLD_LO can be set to 0
 *  and PX_DEBOUNCE_CFG_THRESHOLD_HI can be set to PX_DEBOUNCE_CFG_COUNT_MAX.
 *  
 *  Example:
 *  
 *  @include "px_debounce_test.c"
 *  
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
// Include project specific config. See "px_debounce_cfg_template.h"
#include "px_debounce_cfg.h"

// Check that all project specific options have been specified in "px_debounce_cfg.h"
#ifndef PX_DEBOUNCE_CFG_FIXED
#error "PX_DEBOUNCE_CFG_FIXED not specified"
#endif

// Compile-time configured parameters?
#if !PX_DEBOUNCE_CFG_FIXED
#ifndef PX_DEBOUNCE_CFG_COUNT_MAX
#error "PX_DEBOUNCE_CFG_COUNT_MAX not specified"
#endif
#ifndef PX_DEBOUNCE_CFG_THRESHOLD_LO
#error "PX_DEBOUNCE_CFG_THRESHOLD_LO not specified"
#endif
#ifndef PX_DEBOUNCE_CFG_THRESHOLD_HI
#error "PX_DEBOUNCE_CFG_THRESHOLD_HI not specified"
#endif
#endif


#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS _________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Size definition of counter
typedef uint8_t px_debounce_count_t;

/// Debounce state definition
typedef enum
{
    PX_DEBOUNCE_STATE_LO = 0,               ///< Debounced LO state
    PX_DEBOUNCE_STATE_HI,                   ///< Debounced HI state
} px_debounce_state_t;

/// Debounce state tracking structure
typedef struct
{
    px_debounce_state_t state;              ///< debounced state: LO or HI
    px_debounce_count_t count;              ///< LO/HI counter
#if !PX_DEBOUNCE_CFG_FIXED
    px_debounce_count_t count_max;          ///< Max HI counter value
    px_debounce_count_t count_threshold_lo; ///< Low watermark threshold for valid LO
    px_debounce_count_t count_threshold_hi; ///< High watermark threshold for valid HI
#endif
    bool                rising_edge_flag;   ///< Flag is set when a rising edge is detected (HI threshold is reached)
    bool                falling_edge_flag;  ///< Flag is set when a falling edge is detected (LO threshold is reached)
} px_debounce_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise debounce state tracking structure.
 *  
 *  If PX_DEBOUNCE_CFG_FIXED is 0 then count_max, count_threshold_lo and
 *  count_threshold_hi can be configured for each debounce state tracking
 *  structure.
 *  
 *  @param debounce             Debounce state tracking structure to intialise
 *  @param initial_state_is_hi  Initial state is hi(true) or LO(false)
 *  @param count_max            Max HI counter value
 *  @param count_threshold_lo   Low watermark threshold for valid LO
 *  @param count_threshold_hi   High watermark threshold for valid HI
 */
void px_debounce_init(px_debounce_t *     debounce,
                      bool                initial_state_is_hi
#if !PX_DEBOUNCE_CFG_FIXED
                     ,px_debounce_count_t count_max,
                      px_debounce_count_t count_threshold_lo,
                      px_debounce_count_t count_threshold_hi
#endif
                   );

/**
 *  Update the debounce state with the current state of the digital input
 *  
 *  @param debounce     Debounce state tracking structure to update
 *  @param input_is_hi  true of digital input is high, false if low
 */
void px_debounce_update(px_debounce_t * debounce, 
                        bool            input_is_hi);

/** 
 *  Return current debounced state of digital input.
 */
px_debounce_state_t px_debounce_state(const px_debounce_t * debounce);

/**
 *  Indicates if a rising edge event was detected.
 *  
 *  The rising edge flag is cleared after this function returns true.
 *  
 *  @param debounce Debounce state tracking structure
 *  
 *  @retval true    Rising edge event detected
 *  @retval false   Rising edge event not detected
 */
bool px_debounce_rising_edge_detected(px_debounce_t * debounce);

/**
 *  Indicates if a falling edge event was detected.
 *  
 *  The falling edge flag is cleared after this function returns true.
 *  
 *  @param debounce Debounce state tracking structure
 *  
 *  @retval true    Falling edge event detected
 *  @retval false   Falling edge event not detected
 */
bool px_debounce_falling_edge_detected(px_debounce_t * debounce);

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif // #ifndef __PX_DEBOUNCE_H__
