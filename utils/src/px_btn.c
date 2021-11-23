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


/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"
#include "px_btn.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("btn");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_btn_init(px_btn_t * btn, bool btn_is_pressed)
{
    if(btn_is_pressed)
    {
        btn->flag.state = 1;
        btn->counter    = PX_BTN_CFG_COUNT_MAX;
    }
    else
    {
        btn->flag.state = 0;
        btn->counter    = 0;
    }
    btn->flag.event_press        = 0;
    btn->flag.event_release      = 0;
    btn->flag.event_long_press   = 0;
    btn->flag.event_long_release = 0;
    btn->flag.event_click        = 0;
    btn->state_counter           = 0;
    btn->nr_of_clicks            = 0;
}

void px_btn_update(px_btn_t * btn, bool btn_is_pressed)
{
    if(btn_is_pressed)
    {
        // Maximum reached?
        if(btn->counter < PX_BTN_CFG_COUNT_MAX)
        {
            // High threshold reached?
            if(++btn->counter == PX_BTN_CFG_THRESHOLD_HI)
            {
                // Must state be changed?
                if(btn->flag.state == 0)
                {
                    btn->flag.state       = 1;      // Yes. Change to HI state
                    btn->flag.event_press = 1;      // Set button pressed flag
                    btn->state_counter    = 0;      // Reset state counter
                }
            }
        }
    }
    else
    {
        // Minimum reached?
        if(btn->counter > 0)
        {
            // Low threshold reached?
            if(--btn->counter == PX_BTN_CFG_THRESHOLD_LO)
            {
                // Must state be changed?
                if(btn->flag.state == 1)
                {
                    // Longer than minimum press?
                    if(btn->state_counter > PX_BTN_CFG_CLICK_PRESS_MIN)
                    {
                        if(btn->nr_of_clicks < PX_BTN_CFG_CLICKS_MAX)
                        {
                            btn->nr_of_clicks++;    // Yes. Increment button click count
                        }
                    }
                    btn->flag.state         = 0;    // Change to LO state
                    btn->flag.event_release = 1;    // Set button release flag
                    btn->state_counter      = 0;    // Reset state counter
                }
            }
        }
    }
    // Maximum reached?
    if(btn->state_counter < PX_U16_MAX)
    {
        // Long counter expired?
        if(++btn->state_counter == PX_BTN_CFG_LONG_COUNT)
        {
            // Is the current debounced state LO?
            if(btn->flag.state == 0)
            {
                btn->flag.event_long_release = 1;   // Yes. Set long release flag
            }
            else
            {
                btn->flag.event_long_press = 1;     // No. Set long press flag
            }
        }
        // Button click(s) detected?
        if((btn->nr_of_clicks != 0) && (btn->flag.state == 0))
        {
            // Outside button click release window?
            if(btn->state_counter == PX_BTN_CFG_CLICK_RELEASE_MAX)
            {
                btn->flag.event_click = 1;  // Yes. Set button click flag
            }
        }
    }
}

bool px_btn_is_pressed(const px_btn_t * btn)
{
    if(btn->flag.state == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool px_btn_is_released(const px_btn_t * btn)
{
    if(btn->flag.state == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool px_btn_event_press(px_btn_t * btn)
{
    // Flag set?
    if(btn->flag.event_press)
    {
        // Clear flag
        btn->flag.event_press = 0;
        // Flag was set
        return true;
    }
    else
    {
        // Flag is not set
        return false;
    }
}

bool px_btn_event_release(px_btn_t * btn)
{
    // Flag set?
    if(btn->flag.event_release)
    {
        // Clear flag
        btn->flag.event_release = 0;
        // Flag was set
        return true;
    }
    else
    {
        // Flag is not set
        return false;
    }
}

bool px_btn_event_long_press(px_btn_t * btn)
{
    // Flag set?
    if(btn->flag.event_long_press)
    {
        // Clear flag
        btn->flag.event_long_press = 0;
        // Flag was set
        return true;
    }
    else
    {
        // Flag is not set
        return false;
    }
}

bool px_btn_event_long_release(px_btn_t * btn)
{
    // Flag set?
    if(btn->flag.event_long_release)
    {
        // Clear flag
        btn->flag.event_long_release = 0;
        // Flag was set
        return true;
    }
    else
    {
        // Flag is not set
        return false;
    }
}

uint8_t px_btn_event_click(px_btn_t * btn)
{
    uint8_t nr_of_clicks;

    // Flag set?
    if(btn->flag.event_click)
    {
        // Clear flag
        btn->flag.event_click = 0;
        // Save number of clicks
        nr_of_clicks = btn->nr_of_clicks;
        // Reset number of clicks
        btn->nr_of_clicks = 0;
        return nr_of_clicks;
    }
    else
    {
        return 0;
    }
}
