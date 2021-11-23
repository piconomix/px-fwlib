#ifndef __PX_BTN_H__
#define __PX_BTN_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2021 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
 
    Title:          px_btn.h : Digital button input
    Author(s):      Pieter Conradie
    Creation Date:  2021-11-22

============================================================================= */

/** 
 *  @ingroup UTILS
 *  @defgroup PX_BTN px_btn.h : Digital button input
 *  
 *  This module debounces a digital button input and reports button events.
 *  
 *  File(s):
 *  - utils/inc/px_btn.h
 *  - utils/inc/px_btn_cfg_template.h
 *  - utils/src/px_btn.c
 *  
 *  This module must be configured by supplying a project specific
 *  "px_btn_cfg.h". "px_btn_cfg_template.h" can be copied, renamed and
 *  modified to supply compile time options.
 *  
 *  It is ideal for a noisy digital input such as a button. When a switch is
 *  pressed, the input will bounce between high and low until it finally
 *  settles to a valid state.
 *  
 *  @see https://en.wikipedia.org/wiki/Switch#Contact_bounce
 *  
 *  This module is able to debounce a digital input by looking at successive
 *  values and deciding if it is a valid low or high state. It is also able to
 *  detect and remember a button pressed event (debounced rising edge) or button
 *  released event (debounced falling edge). Finally, if PX_BTN_CFG_LONG_COUNT
 *  is defined with a large non-zero value, a long press or long release is also
 *  detected.
 *
 *  Single, double or triple clicks can be detected and reported by
 *  setting PX_BTN_CFG_CLICKS_MAX to the maximum number of clicks. A click is
 *  defined as a button press exceeding PX_BTN_CFG_CLICK_PRESS_MIN followed by a
 *  button release. The maximum inter-click time is set with
 *  PX_BTN_CFG_CLICK_RELEASE_MAX.
 *  
 *  Debouncing is implemented as follows: a counter is incremented each time the
 *  current digital input state is HI. If the counter reaches the high watermark
 *  threshold, the debounced state is considered HI. Conversely, the counter is
 *  decremented each time the current digital input state is LO. If the counter
 *  reaches the low watermark threshold, the debounced state is considered LO.
 *  The minimum counter value is 0 and the maximum value is PX_BTN_CFG_COUNT_MAX.
 *  This scheme provides sufficient hysteresis to debounce a noisy digital input.
 *
 *  @code{.unparsed}
 *  [9] <-----MAX----------------1---------------------------
 *  [8]                         1 1                         1
 *  [7] <--HI threshold--------1---1-------------1---------1-
 *  [6]                       0     1           0 1       0
 *  [5]                      0       1         0   1     0
 *  [4]                     0         1       0     1   0
 *  [3]                    0           1     0       1 0
 *  [2] <--LO threshold---0-------------0---0---------0------
 *  [1]                  0               0 0
 *  [0] <-----MIN-------0-----------------0------------------
 *  @endcode
 *  
 *  If no hysteresis is required, then PX_BTN_CFG_THRESHOLD_LO can be set
 *  to 0 and PX_BTN_CFG_THRESHOLD_HI can be set to PX_BTN_CFG_COUNT_MAX.
 *  
 *  Example:
 *  
 *  @include "px_btn_test.c"
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

// Include project specific configuration. See "px_btn_cfg_template.h"
#include "px_btn_cfg.h"

// Check that all project specific options have been specified in "px_btn_cfg.h"
#if (   !defined(PX_BTN_CFG_COUNT_MAX        ) \
     || !defined(PX_BTN_CFG_THRESHOLD_LO     ) \
     || !defined(PX_BTN_CFG_THRESHOLD_HI     ) \
     || !defined(PX_BTN_CFG_LONG_COUNT       ) \
     || !defined(PX_BTN_CFG_CLICKS_MAX       ) \
     || !defined(PX_BTN_CFG_CLICK_PRESS_MIN  ) \
     || !defined(PX_BTN_CFG_CLICK_RELEASE_MAX)  )
#error "One or more options not defined in 'px_btn_cfg.h'"
#endif
#if (PX_BTN_CFG_COUNT_MAX > 0xff)
#error "counter size is uint8_t"
#endif
#if (PX_BTN_CFG_LONG_COUNT > 0xffff)
#error "long_counter size is uint16_t"
#endif
#if(PX_BTN_CFG_CLICKS_MAX < 1)
#error "Minimum value of PX_BTN_CFG_CLICKS_MAX is 1"
#endif

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Structure to track state of button
typedef struct
{
    struct
    {
        uint8_t state : 1;              ///< Debounce state (0 = button is released; 1 = button is being pressed)
        uint8_t event_press : 1;        ///< Set when a rising edge is detected (HI threshold is reached)
        uint8_t event_release : 1;      ///< Set when a falling edge is detected (LO threshold is reached)
        uint8_t event_long_press : 1;   ///< Set when a long HI is detected (after rising edge)
        uint8_t event_long_release : 1; ///< Set when a long LO is detected (after falling edge)
        uint8_t event_click : 1;        ///< Set when a button click (or clicks) have been detected
    } flag;

    uint8_t  counter;                   ///< LO/HI counter
    uint16_t state_counter;             ///< debounced state counter
    uint8_t  nr_of_clicks;              ///< Number of clicks detected
} px_btn_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise button state.
 *
 *  @param btn                  Pointer to a button object
 *  @param btn_is_pressed       Button is being pressed (true) or released (false)
 */
void px_btn_init(px_btn_t * btn, bool btn_is_pressed);

/**
 *  Update debounced button state and if event is detected set flag.
 *
 *  @param btn                  Pointer to a button object
 *  @param btn_is_pressed       Button is being pressed (true) or released (false)
 */
void px_btn_update(px_btn_t * btn, bool btn_is_pressed);

/**
 *  Is button being pressed (debounced state).
 *
 *  @param btn                  Pointer to a button object
 *
 *  @retval true                Button is being pressed
 *  @retval false               Button is released
 */
bool px_btn_is_pressed(const px_btn_t * btn);

/**
 *  Is button released (debounced state).
 *
 *  @param btn                  Pointer to a button object
 *
 *  @retval true                Button is released
 *  @retval false               Button is being pressed
 *
 */
bool px_btn_is_released(const px_btn_t * btn);

/**
 *  Button press event detected?
 *
 *  The event flag is also cleared after this function is called.
 *
 *  @param btn                  Pointer to a button object
 *
 *  @retval true                Button has been pressed
 *  @retval false               Button has not been pressed
 */
bool px_btn_event_press(px_btn_t * btn);

/**
 *  Button release event detected?
 *
 *  The event flag is also cleared after this function is called.
 *
 *  @param btn                  Pointer to a button object
 *
 *  @retval true                Button has been released
 *  @retval false               Button has not been released
 */
bool px_btn_event_release(px_btn_t * btn);

/**
 *  Button long press event detected?
 *
 *  The event flag is also cleared after this function is called.
 *
 *  @param btn                  Pointer to a button object
 *
 *  @retval true                Button is being pressed for a long time
 *  @retval false               Button has not been pressed for a long time
 */
bool px_btn_event_long_press(px_btn_t * btn);

/**
 *  Button long release event detected?
 *
 *  The event flag is also cleared after this function is called.
 *
 *  @param btn                  Pointer to a button object
 *
 *  @retval true                Button has been released for a long time
 *  @retval false               Button has not been released for a long time
 */
bool px_btn_event_long_release(px_btn_t * btn);

/**
 *  Button click (or clicks) event detected?
 *
 *  @param btn                  Pointer to a button object
 *  @param nr_of_clicks         Pointer to location where number of clicks detected must be stored
 *
 *  @return uint8_t             Number of clicks detected. 0 = no clicks
 */
uint8_t px_btn_event_click(px_btn_t * btn);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
