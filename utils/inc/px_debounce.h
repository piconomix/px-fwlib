#ifndef __PX_DEBOUNCE_H__
#define __PX_DEBOUNCE_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2015 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
 
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
 *  pressed, the input will bounce between high and low until it finally
 *  settles to a valid state.
 *  
 *  @see https://en.wikipedia.org/wiki/Switch#Contact_bounce
 *  
 *  This module is able to debounce a digital input by looking at successive
 *  values and deciding if it is a valid low or high state. It is also able to
 *  register and remember a rising and falling edge event, e.g. "key pressed
 *  event" or "key released event". Finally, if PX_DEBOUNCE_CFG_LONG_COUNT is 
 *  defined with a non-zero value, a long HI or long LO is also detected.
 *  
 *  A counter is incremented each time the current digital input state is HI.
 *  If the counter reaches the high threshold, the debounced state is considered
 *  HI. Conversely, the counter is decremented each time the current digital
 *  input state is LO. If the counter reaches the low threshold, the debounced
 *  state is considered LO. The minimum counter value is 0 and the maximum value
 *  is PX_DEBOUNCE_CFG_COUNT_MAX. This scheme provides sufficient hysteresis to
 *  debounce a noisy digital input.
 *  
 *  If no hysteresis is required, then PX_DEBOUNCE_CFG_THRESHOLD_LO can be set 
 *  to 0 and PX_DEBOUNCE_CFG_THRESHOLD_HI can be set to PX_DEBOUNCE_CFG_COUNT_MAX. 
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
#if (   !defined(PX_DEBOUNCE_CFG_COUNT_MAX   ) \
     || !defined(PX_DEBOUNCE_CFG_THRESHOLD_LO) \
     || !defined(PX_DEBOUNCE_CFG_THRESHOLD_HI) \
     || !defined(PX_DEBOUNCE_CFG_LONG_COUNT  )  )
#error "One or more options not defined in 'px_debounce_cfg.h'"
#endif

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */

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
    px_debounce_count_t counter;            ///< LO/HI counter
#if (PX_DEBOUNCE_CFG_LONG_COUNT != 0)
    px_debounce_count_t counter_long;       ///< long LO/HI detection counter
#endif

    bool                rising_edge_flag;   ///< Flag is set when a rising edge is detected (HI threshold is reached)
    bool                falling_edge_flag;  ///< Flag is set when a falling edge is detected (LO threshold is reached)
#if (PX_DEBOUNCE_CFG_LONG_COUNT != 0)
    bool                long_hi_flag;       ///< Flag is set when a long HI is detected (after rising edge)
    bool                long_lo_flag;       ///< Flag is set when a long LO is detected (after falling edge)
#endif
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
 */
void px_debounce_init(px_debounce_t * debounce,
                      bool            initial_state_is_hi);

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
 *  The flag is cleared after this function returns true.
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
 *  The flag is cleared after this function returns true.
 *  
 *  @param debounce Debounce state tracking structure
 *  
 *  @retval true    Falling edge event detected
 *  @retval false   Falling edge event not detected
 */
bool px_debounce_falling_edge_detected(px_debounce_t * debounce);

#if (PX_DEBOUNCE_CFG_LONG_COUNT != 0)
/**
 *  Indicates if a long HI event was detected.
 *  
 *  The flag is cleared after this function returns true.
 *  
 *  @param debounce Debounce state tracking structure
 *  
 *  @retval true    Long HI event detected
 *  @retval false   Long HI event not detected
 */
bool px_debounce_long_hi_detected(px_debounce_t * debounce);

/**
 *  Indicates if a long LO event was detected.
 *  
 *  The flag is cleared after this function returns true.
 *  
 *  @param debounce Debounce state tracking structure
 *  
 *  @retval true    Long LO event detected
 *  @retval false   Long LO event not detected
 */
bool px_debounce_long_lo_detected(px_debounce_t * debounce);
#endif

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif // #ifndef __PX_DEBOUNCE_H__
