#ifndef __PX_VT100_H__
#define __PX_VT100_H__
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
    
    Title:          px_vt100.h : VT100/ANSI Terminal module
    Author(s):      Pieter Conradie
    Creation Date:  2012-09-15

============================================================================= */

/** 
 *  @ingroup COMMS
 *  @defgroup PX_VT100 px_vt100.h : VT100/ANSI Terminal module
 *  
 *  Parses VT100 / ANSI escape sequences to interface with a terminal emulator.
 *  
 *  File(s):
 *  - comms/px_vt100.h
 *  - comms/px_vt100.c
 *  
 *  @see
 *  - http://en.wikipedia.org/wiki/ANSI_escape_code
 *  - http://www.termsys.demon.co.uk/vtansi.htm
 *  
 *  Example:
 *  
 *  @include comms/test/px_cli_test.c
  */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
/// @name Special ASCII values
//@{
#define PX_VT100_CHAR_BEL      0x07
#define PX_VT100_CHAR_BS       0x08
#define PX_VT100_CHAR_TAB      0x09
#define PX_VT100_CHAR_CR       0x0D
#define PX_VT100_CHAR_LF       0x0A
#define PX_VT100_CHAR_ESC      0x1B
#define PX_VT100_CHAR_DEL      0x7F
//@}

/// @name VT100 escape sequences
//@{
#define PX_VT100_CURSOR_HOME                "\e[H"
#define PX_VT100_CURSOR_UP                  "\e[1A"
#define PX_VT100_CURSOR_DOWN                "\e[1B"
#define PX_VT100_CURSOR_FORWARD             "\e[1C"
#define PX_VT100_CURSOR_BACKWARD            "\e[1D"
#define PX_VT100_SAVE_CURSOR                "\e[s"
#define PX_VT100_UNSAVE_CURSOR              "\e[u"
#define PX_VT100_SAVE_CURSOR_AND_ATTRS      "\e7"
#define PX_VT100_RESTORE_CURSOR_AND_ATTRS   "\e8"

#define PX_VT100_SCROLL_SCREEN              "\e[r"
#define PX_VT100_SCROLL_DOWN                "\eD"
#define PX_VT100_SCROLL_UP                  "\eM"

#define PX_VT100_SET_TAB                    "\eH"
#define PX_VT100_CLEAR_TAB                  "\e[g"
#define PX_VT100_CLEAR_ALL_TABS             "\e[3g"

#define PX_VT100_ERASE_END_OF_LINE          "\e[K"
#define PX_VT100_ERASE_START_OF_LINE        "\e[1K"
#define PX_VT100_ERASE_LINE                 "\e[2K"
#define PX_VT100_ERASE_DOWN                 "\e[J"
#define PX_VT100_ERASE_UP                   "\e[1J"
#define PX_VT100_ERASE_SCREEN               "\e[2J"

#define PX_VT100_RESET_ALL_ATTRS            "\e[0m"
#define PX_VT100_SET_ATTR_BRIGHT            "\e[1m"
#define PX_VT100_SET_ATTR_DIM               "\e[2m"
#define PX_VT100_SET_ATTR_UNDERSCORE        "\e[4m"
#define PX_VT100_SET_ATTR_BLINK             "\e[5m"
#define PX_VT100_SET_ATTR_REVERSE           "\e[7m"
#define PX_VT100_SET_ATTR_HIDDEN            "\e[8m"

#define PX_VT100_SET_FOREGROUND_BLACK       "\e[30m"
#define PX_VT100_SET_FOREGROUND_RED         "\e[31m"
#define PX_VT100_SET_FOREGROUND_GREEN       "\e[32m"
#define PX_VT100_SET_FOREGROUND_YELLOW      "\e[33m"
#define PX_VT100_SET_FOREGROUND_BLUE        "\e[34m"
#define PX_VT100_SET_FOREGROUND_MAGENTA     "\e[35m"
#define PX_VT100_SET_FOREGROUND_CYAN        "\e[36m"
#define PX_VT100_SET_FOREGROUND_WHITE       "\e[37m"

#define PX_VT100_SET_BACKGROUND_BLACK       "\e[40m"
#define PX_VT100_SET_BACKGROUND_RED         "\e[41m"
#define PX_VT100_SET_BACKGROUND_GREEN       "\e[42m"
#define PX_VT100_SET_BACKGROUND_YELLOW      "\e[43m"
#define PX_VT100_SET_BACKGROUND_BLUE        "\e[44m"
#define PX_VT100_SET_BACKGROUND_MAGENTA     "\e[45m"
#define PX_VT100_SET_BACKGROUND_CYAN        "\e[46m"
#define PX_VT100_SET_BACKGROUND_WHITE       "\e[47m"
//@}

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// VT100 Terminal receive state
typedef enum
{
    PX_VT100_CHAR_NORMAL,          ///< A normal key has been pressed and must be used
    PX_VT100_CHAR_INVALID,         ///< An invalid key code has been sent and must be discarded
    PX_VT100_ESC_SEQ_BUSY,         ///< Busy with escape sequence; data must be discarded
    PX_VT100_ESC_SEQ_ARROW_UP,     ///< Up Arrow has been pressed
    PX_VT100_ESC_SEQ_ARROW_DN,     ///< Down Arrow has been pressed
    PX_VT100_ESC_SEQ_ARROW_LEFT,   ///< Left Arrow has been pressed
    PX_VT100_ESC_SEQ_ARROW_RIGHT,  ///< Right Arrow has been pressed
} px_vt100_state_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/** 
 *  Initialise module.
 */
extern void px_vt100_init(void);

/**
 *  Process a received character byte. 
 *  
 *  If an ANSI escape sequence is detected, PX_VT100_ESC_SEQ_BUSY will be returned
 *  to indicate that received character should be ignored, otherwise
 *  PX_VT100_CHAR_NORMAL is returned to indicate that the character must be used. 
 *  
 *  If an ANSI escape sequence is decoded it is indicated with a
 *  PX_VT100_ESC_SEQ... state.
 *  
 *  @param data             Received character to be process for ANSI Escape Sequences
 *  
 *  @return px_vt100_state_t   PX_VT100_CHAR_NORMAL if the character must be used as
 *                          normal; PX_VT100_CHAR_INVALID or PX_VT100_ESC_SEQ_BUSY if
 *                          character should be ignored; otherwise the decoded
 *                          ANSI Escape Sequence.
 *  
 */
extern px_vt100_state_t px_vt100_on_rx_char(char data);

/** 
 *  Send 'clear screen' command to terminal.
 */
extern void px_vt100_clr_display(void);

/** 
 *  Send 'erase line' command to terminal.
 */
extern void px_vt100_erase_line(void);

/** 
 *  Delete specified number of characters
 *  
 *  @param nr_of_chars  Number of characters to delete
 */
extern void px_vt100_del_chars(uint8_t nr_of_chars);

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif // #ifndef __PX_VT100_H__
