/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2012 Pieter Conradie <https://piconomix.com>
 
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
    
    Title:          px_vt100.h : VT100/VT100 Terminal module
    Author(s):      Pieter Conradie
    Creation Date:  2012-09-15

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdio.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_vt100.h"
#include "px_board.h"
#include "px_dbg.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_DBG_DECL_NAME("px_vt100");


/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static uint8_t px_vt100_state;

/* _____LOCAL FUNCTION PROTOTYPES____________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_vt100_init()
{
    // Reset state
    px_vt100_state = 0;

    // Move cursor to upper left corner
    putchar(PX_VT100_CHAR_ESC);
    putchar('[');
    putchar('H');

    // Clear display
    px_vt100_clr_display();

    // Wait until Terminal has reset
    px_board_delay_ms(100);
}

px_vt100_state_t px_vt100_on_rx_char(char data)
{
    switch(px_vt100_state)
    {
    case 0:
        if(data == PX_VT100_CHAR_ESC)
        {
            // Escape sequence detected
            px_vt100_state++;
            // Indicate that received character should be discarded
            return PX_VT100_ESC_SEQ_BUSY;
        }
        if(data >= 0x80)
        {
            // Invalid character received
            return PX_VT100_CHAR_INVALID;            
        }
        // Normal character received
        return PX_VT100_CHAR_NORMAL;        

    case 1:
        if(data == '[')
        {
            // Escape sequence detected
            px_vt100_state++;
            // Indicate that received character should be ignored
            return PX_VT100_ESC_SEQ_BUSY;
        }
        // Incorrect escape sequence
        px_vt100_state = 0;
        // Indicate that received character should be ignored
        return PX_VT100_ESC_SEQ_BUSY;

    case 2:
        // Reset state first
        px_vt100_state = 0;

        // Detect sequence
        switch(data)
        {
        case 'A': return PX_VT100_ESC_SEQ_ARROW_UP;
        case 'B': return PX_VT100_ESC_SEQ_ARROW_DN;
        case 'C': return PX_VT100_ESC_SEQ_ARROW_LEFT;
        case 'D': return PX_VT100_ESC_SEQ_ARROW_RIGHT;
        default:  return PX_VT100_CHAR_INVALID;
        }

    default:
        //Reset state
        px_vt100_state = 0;
        // Indicate that received character should be discarded
        return PX_VT100_CHAR_INVALID;
    }
}

void px_vt100_clr_display(void)
{
    putchar(PX_VT100_CHAR_ESC);
    putchar('[');
    putchar('2');
    putchar('J');
}

void px_vt100_erase_line(void)
{
    putchar(PX_VT100_CHAR_ESC);
    putchar('[');
    putchar('2');
    putchar('K');
}

void px_vt100_del_chars(uint8_t nr_of_chars)
{
    while(nr_of_chars != 0)
    {
        putchar(PX_VT100_CHAR_BS);
        putchar(' ');
        putchar(PX_VT100_CHAR_BS);
        nr_of_chars--;
    }
}
