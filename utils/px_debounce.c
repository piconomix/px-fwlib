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


/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_debounce.h"

#include "px_dbg.h"
PX_DBG_DECL_NAME("debounce")

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_debounce_init(px_debounce_t *     debounce,
                      bool                initial_state_is_hi
#if !PX_DEBOUNCE_CFG_FIXED
                     ,px_debounce_count_t count_max,
                      px_debounce_count_t count_threshold_lo,
                      px_debounce_count_t count_threshold_hi
#endif
                          )
{
#if PX_DEBOUNCE_CFG_FIXED
    if(initial_state_is_hi)
    {
        debounce->state = PX_DEBOUNCE_STATE_HI;
        debounce->count = PX_DEBOUNCE_CFG_COUNT_MAX;
    }
    else
    {
        debounce->state = PX_DEBOUNCE_STATE_LO;
        debounce->count = 0;
    }
#else
    if(initial_state_is_hi)
    {
        debounce->state = PX_DEBOUNCE_STATE_HI;
        debounce->count = count_max;
    }
    else
    {
        debounce->state = PX_DEBOUNCE_STATE_LO;
        debounce->count = 0;
    }
    debounce->count_max          = count_max;
    debounce->count_threshold_lo = count_threshold_lo;
    debounce->count_threshold_hi = count_threshold_hi;    
#endif

    debounce->rising_edge_flag  = false;
    debounce->falling_edge_flag = false;
}

void px_debounce_update(px_debounce_t * debounce, 
                        bool            input_is_hi)
{
#if PX_DEBOUNCE_CFG_FIXED
    if(input_is_hi)
    {
        // Maximum reached?
        if(debounce->count < PX_DEBOUNCE_CFG_COUNT_MAX)
        {
            // Increment count
            debounce->count++;
        }
        // High watermark threshold reached?
        if(debounce->count == PX_DEBOUNCE_CFG_THRESHOLD_HI)
        {
            // Must state be changed?
            if(debounce->state == PX_DEBOUNCE_STATE_LO)
            {
                // Change to HI state
                debounce->state = PX_DEBOUNCE_STATE_HI;
                // Set rising edge flag
                debounce->rising_edge_flag = true;
            }
        }
    }
    else
    {
        // Minimum reached?
        if(debounce->count > 0)
        {
            // Decrement count
            debounce->count--;
        }
        // Low watermark threshold reached?
        if(debounce->count == PX_DEBOUNCE_CFG_THRESHOLD_LO)
        {
            // Must state be changed?
            if(debounce->state == PX_DEBOUNCE_STATE_HI)
            {
                // Change to LO state
                debounce->state = PX_DEBOUNCE_STATE_LO;
                // Set falling edge flag
                debounce->falling_edge_flag = true;
            }           
        }
    }
#else
    if(input_is_hi)
    {
        // Maximum reached?
        if(debounce->count < debounce->count_max)
        {
            // Increment count
            debounce->count++;
        }
        // High watermark threshold reached?
        if(debounce->count == debounce->count_threshold_hi)
        {
            // Must state be changed?
            if(debounce->state == PX_DEBOUNCE_STATE_LO)
            {
                // Change to HI state
                debounce->state = PX_DEBOUNCE_STATE_HI;
                // Set rising edge flag
                debounce->rising_edge_flag = true;
            }
        }
    }
    else
    {
        // Minimum reached?
        if(debounce->count > 0)
        {
            // Decrement count
            debounce->count--;
        }
        // Low watermark threshold reached?
        if(debounce->count == debounce->count_threshold_lo)
        {
            // Must state be changed?
            if(debounce->state == PX_DEBOUNCE_STATE_HI)
            {
                // Change to LO state
                debounce->state = PX_DEBOUNCE_STATE_LO;
                // Set falling edge flag
                debounce->falling_edge_flag = true;
            }
        }
    }
#endif
}

px_debounce_state_t px_debounce_state(const px_debounce_t * debounce)
{
    return debounce->state;
}

bool px_debounce_rising_edge_detected(px_debounce_t * debounce)
{
    // Rising edge flag set?
    if(debounce->rising_edge_flag)
    {
        // Clear flag
        debounce->rising_edge_flag = false;
        // Flag was set
        return true;
    }
    else
    {
        // Flag is not set
        return false;
    }
}

bool px_debounce_falling_edge_detected(px_debounce_t * debounce)
{
    // Falling edge flag set?
    if(debounce->falling_edge_flag)
    {
        // Clear flag
        debounce->falling_edge_flag = false;
        // Flag was set
        return true;
    }
    else
    {
        // Flag is not set
        return false;
    }
}
