#ifndef __PX_VT100_H__
#define __PX_VT100_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2012 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
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
 *  - comms/inc/px_vt100.h
 *  - comms/src/px_vt100.c
 *  
 *  @see
 *  - http://en.wikipedia.org/wiki/ANSI_escape_code
 *  - http://www.termsys.demon.co.uk/vtansi.htm
 *  
 *  Example:
 *  
 *  @include comms/test/px_cli_test.c
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
/// @name Special ASCII values
/// @{
#define PX_VT100_CHAR_BEL                   0x07
#define PX_VT100_CHAR_BS                    0x08
#define PX_VT100_CHAR_TAB                   0x09
#define PX_VT100_CHAR_CR                    0x0D    // '\r'
#define PX_VT100_CHAR_LF                    0x0A    // '\n'
#define PX_VT100_CHAR_ESC                   0x1B
#define PX_VT100_CHAR_DEL                   0x7F
/// @}

/// @name VT100 escape sequences
/// @{
#define PX_VT100_CURSOR_HOME                "\x1B[H"
#define PX_VT100_CURSOR_UP                  "\x1B[1A"
#define PX_VT100_CURSOR_DOWN                "\x1B[1B"
#define PX_VT100_CURSOR_FORWARD             "\x1B[1C"
#define PX_VT100_CURSOR_BACKWARD            "\x1B[1D"
#define PX_VT100_SAVE_CURSOR                "\x1B[s"
#define PX_VT100_UNSAVE_CURSOR              "\x1B[u"
#define PX_VT100_SAVE_CURSOR_AND_ATTRS      "\x1B7"
#define PX_VT100_RESTORE_CURSOR_AND_ATTRS   "\x1B8"

#define PX_VT100_SCROLL_SCREEN              "\x1B[r"
#define PX_VT100_SCROLL_DOWN                "\x1BD"
#define PX_VT100_SCROLL_UP                  "\x1BM"

#define PX_VT100_SET_TAB                    "\x1BH"
#define PX_VT100_CLEAR_TAB                  "\x1B[g"
#define PX_VT100_CLEAR_ALL_TABS             "\x1B[3g"

#define PX_VT100_ERASE_END_OF_LINE          "\x1B[K"
#define PX_VT100_ERASE_START_OF_LINE        "\x1B[1K"
#define PX_VT100_ERASE_LINE                 "\x1B[2K"
#define PX_VT100_ERASE_DOWN                 "\x1B[J"
#define PX_VT100_ERASE_UP                   "\x1B[1J"
#define PX_VT100_ERASE_SCREEN               "\x1B[2J"

#define PX_VT100_RST_ALL_ATTRS              "\x1B[0m"
#define PX_VT100_SET_ATTR_BRIGHT            "\x1B[1m"
#define PX_VT100_SET_ATTR_DIM               "\x1B[2m"
#define PX_VT100_SET_ATTR_UNDERSCORE        "\x1B[4m"
#define PX_VT100_SET_ATTR_BLINK             "\x1B[5m"
#define PX_VT100_SET_ATTR_REVERSE           "\x1B[7m"
#define PX_VT100_SET_ATTR_HIDDEN            "\x1B[8m"

#define PX_VT100_SET_FG_BLACK               "\x1B[30m"
#define PX_VT100_SET_FG_RED                 "\x1B[31m"
#define PX_VT100_SET_FG_GREEN               "\x1B[32m"
#define PX_VT100_SET_FG_YELLOW              "\x1B[33m"
#define PX_VT100_SET_FG_BLUE                "\x1B[34m"
#define PX_VT100_SET_FG_MAGENTA             "\x1B[35m"
#define PX_VT100_SET_FG_CYAN                "\x1B[36m"
#define PX_VT100_SET_FG_WHITE               "\x1B[37m"

#define PX_VT100_SET_BG_BLACK               "\x1B[40m"
#define PX_VT100_SET_BG_RED                 "\x1B[41m"
#define PX_VT100_SET_BG_GREEN               "\x1B[42m"
#define PX_VT100_SET_BG_YELLOW              "\x1B[43m"
#define PX_VT100_SET_BG_BLUE                "\x1B[44m"
#define PX_VT100_SET_BG_MAGENTA             "\x1B[45m"
#define PX_VT100_SET_BG_CYAN                "\x1B[46m"
#define PX_VT100_SET_BG_WHITE               "\x1B[47m"
/// @}

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

#ifdef __cplusplus
}
#endif

/// @}
#endif
