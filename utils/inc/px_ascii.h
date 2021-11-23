#ifndef __PX_ASCII_H__
#define __PX_ASCII_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2021 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
 
    Title:          px_ascii.h : ASCII table
    Author(s):      Pieter Conradie
    Creation Date:  2021-06-18

============================================================================= */

/** 
 *  @ingroup UTILS
 *  @defgroup PX_ASCII px_ascii.h : ASCII table
 *  
 *  ASCII table character encoding.
 *  
 *  File(s):
 *  - utils/inc/px_ascii.h
 *  
 *  @see https://en.wikipedia.org/wiki/ASCII
 *  @see https://en.wikipedia.org/wiki/Escape_sequences_in_C
 *
 *  ASCII Table (32 to 127 / 0x20 to 0x7f):
 *
 *  @code{.unparsed}
 *       0   1   2   3   4   5   6   7      8   9   A   B   C   D   E   F
 *     +---+---+---+---+---+---+---+---+  +---+---+---+---+---+---+---+---+
 *  20 |   | ! | " | # | $ | % | & | ' |  | ( | ) | * | + | , | - | . | / |
 *     +---+---+---+---+---+---+---+---+  +---+---+---+---+---+---+---+---+
 *  30 | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |  | 8 | 9 | : | ; | < | = | > | ? |
 *     +---+---+---+---+---+---+---+---+  +---+---+---+---+---+---+---+---+
 *  40 | @ | A | B | C | D | E | F | G |  | H | I | J | K | L | M | N | O |
 *     +---+---+---+---+---+---+---+---+  +---+---+---+---+---+---+---+---+
 *  50 | P | Q | R | S | T | U | V | W |  | X | Y | Z | [ | \ | ] | ^ | _ |
 *     +---+---+---+---+---+---+---+---+  +---+---+---+---+---+---+---+---+
 *  60 | ` | a | b | c | d | e | f | g |  | h | i | j | k | l | m | n | o |
 *     +---+---+---+---+---+---+---+---+  +---+---+---+---+---+---+---+---+
 *  70 | p | q | r | s | t | u | v | w |  | x | y | z | { | | | } | ~ |   |
 *     +---+---+---+---+---+---+---+---+  +---+---+---+---+---+---+---+---+
 * @endcode
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

#ifdef __cplusplus
extern "C"
{
#endif
/* _____DEFINITIONS__________________________________________________________ */
/// @name ASCII name to char map
/// @{
#define PX_ASCII_CHAR_NUL       ((char)0x00)    ///< 0    Null
#define PX_ASCII_CHAR_SOH       ((char)0x01)    ///< 1    Start of Heading
#define PX_ASCII_CHAR_STX       ((char)0x02)    ///< 2    Start of Text
#define PX_ASCII_CHAR_ETX       ((char)0x03)    ///< 3    End of Text
#define PX_ASCII_CHAR_EOT       ((char)0x04)    ///< 4    End of Transmission
#define PX_ASCII_CHAR_ENQ       ((char)0x05)    ///< 5    Enquiry
#define PX_ASCII_CHAR_ACK       ((char)0x06)    ///< 6    Acknowledgment  ("\a")
#define PX_ASCII_CHAR_BEL       ((char)0x07)    ///< 7    Bell
#define PX_ASCII_CHAR_BS        ((char)0x08)    ///< 8    Backspace       ("\b")
#define PX_ASCII_CHAR_HT        ((char)0x09)    ///< 9    Horizontal Tab  ("\t")
#define PX_ASCII_CHAR_LF        ((char)0x0a)    ///< 10   Line Feed       ("\n")
#define PX_ASCII_CHAR_VT        ((char)0x0b)    ///< 11   Vertical Tab    ("\v")
#define PX_ASCII_CHAR_FF        ((char)0x0c)    ///< 12   Form Feed       ("\f")
#define PX_ASCII_CHAR_CR        ((char)0x0d)    ///< 13   Carriage Return ("\r")
#define PX_ASCII_CHAR_SO        ((char)0x0e)    ///< 14   Shift Out
#define PX_ASCII_CHAR_SI        ((char)0x0f)    ///< 15   Shift In
#define PX_ASCII_CHAR_DLE       ((char)0x10)    ///< 16   Data Link Escape
#define PX_ASCII_CHAR_DC1       ((char)0x11)    ///< 17   Device Control 1 (often XON)
#define PX_ASCII_CHAR_XON       ((char)0x11)    ///< 17   XON
#define PX_ASCII_CHAR_DC2       ((char)0x12)    ///< 18   Device Control 2
#define PX_ASCII_CHAR_DC3       ((char)0x13)    ///< 19   Device Control 3 (often XOFF)
#define PX_ASCII_CHAR_XOFF      ((char)0x13)    ///< 19   XOFF
#define PX_ASCII_CHAR_DC4       ((char)0x14)    ///< 20   Device Control 4
#define PX_ASCII_CHAR_NAK       ((char)0x15)    ///< 21   Negative Acknowledgement
#define PX_ASCII_CHAR_SYN       ((char)0x16)    ///< 22   Synchronous Idle
#define PX_ASCII_CHAR_ETB       ((char)0x17)    ///< 23   End of Transmission Block
#define PX_ASCII_CHAR_CAN       ((char)0x18)    ///< 24   Cancel
#define PX_ASCII_CHAR_EM        ((char)0x19)    ///< 25   End of Medium
#define PX_ASCII_CHAR_SUB       ((char)0x1a)    ///< 26   Substitute
#define PX_ASCII_CHAR_ESC       ((char)0x1b)    ///< 27   Escape          ("\e"). Note that this is a non-standard code
#define PX_ASCII_CHAR_FS        ((char)0x1c)    ///< 28   File Separator
#define PX_ASCII_CHAR_GS        ((char)0x1d)    ///< 29   Group Separator
#define PX_ASCII_CHAR_RS        ((char)0x1e)    ///< 30   Record Separator
#define PX_ASCII_CHAR_US        ((char)0x1f)    ///< 31   Unit Separator
#define PX_ASCII_CHAR_DEL       ((char)0x7f)    ///< 128  Delete
/// @}

/// @name ASCII name to string map
/// @{
#define PX_ASCII_STR_NUL        "\x00"          ///< 0    Null
#define PX_ASCII_STR_SOH        "\x01"          ///< 1    Start of Header
#define PX_ASCII_STR_STX        "\x02"          ///< 2    Start of Text
#define PX_ASCII_STR_ETX        "\x03"          ///< 3    End of Text
#define PX_ASCII_STR_EOT        "\x04"          ///< 4    End of Transmission
#define PX_ASCII_STR_ENQ        "\x05"          ///< 5    Enquiry
#define PX_ASCII_STR_ACK        "\x06"          ///< 6    Acknowledgment  ("\a")
#define PX_ASCII_STR_BEL        "\x07"          ///< 7    Bell
#define PX_ASCII_STR_BS         "\x08"          ///< 8    Backspace       ("\b")
#define PX_ASCII_STR_HT         "\x09"          ///< 9    Horizontal Tab  ("\t")
#define PX_ASCII_STR_LF         "\x0a"          ///< 10   Line Feed       ("\n")
#define PX_ASCII_STR_VT         "\x0b"          ///< 11   Vertical Tab    ("\v")
#define PX_ASCII_STR_FF         "\x0c"          ///< 12   Form Feed       ("\f")
#define PX_ASCII_STR_CR         "\x0d"          ///< 13   Carriage Return ("\r")
#define PX_ASCII_STR_SO         "\x0e"          ///< 14   Shift Out
#define PX_ASCII_STR_SI         "\x0f"          ///< 15   Shift In
#define PX_ASCII_STR_DLE        "\x10"          ///< 16   Data Link Escape
#define PX_ASCII_STR_DC1        "\x11"          ///< 17   Device Control 1 (often XON)
#define PX_ASCII_STR_XON        "\x11"          ///< 17   XON
#define PX_ASCII_STR_DC2        "\x12"          ///< 18   Device Control 2
#define PX_ASCII_STR_DC3        "\x13"          ///< 19   Device Control 3 (often XOFF)
#define PX_ASCII_STR_XOFF       "\x13"          ///< 19   XOFF
#define PX_ASCII_STR_DC4        "\x14"          ///< 20   Device Control 4
#define PX_ASCII_STR_NAK        "\x15"          ///< 21   Negative Acknowledgement
#define PX_ASCII_STR_SYN        "\x16"          ///< 22   Synchronous Idle
#define PX_ASCII_STR_ETB        "\x17"          ///< 23   End of Transmission Block
#define PX_ASCII_STR_CAN        "\x18"          ///< 24   Cancel
#define PX_ASCII_STR_EM         "\x19"          ///< 25   End of Medium
#define PX_ASCII_STR_SUB        "\x1a"          ///< 26   Substitute
#define PX_ASCII_STR_ESC        "\x1b"          ///< 27   Escape          ("\e"). Note that this is a non-standard code
#define PX_ASCII_STR_FS         "\x1c"          ///< 28   File Separator
#define PX_ASCII_STR_GS         "\x1d"          ///< 29   Group Separator
#define PX_ASCII_STR_RS         "\x1e"          ///< 30   Record Separator
#define PX_ASCII_STR_US         "\x1f"          ///< 31   Unit Separator
#define PX_ASCII_STR_DEL        "\x7f"          ///< 127  Delete

#define PX_ASCII_STR_CR_LF      "\r\n"          ///< Special CR+LF combo
/// @}

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Return a name string for an ASCII character (0 to 31 or 127)
 *
 *  @param c                ASCII char
 *
 *  @return const char *    Name string
 */
const char * px_ascii_char_to_name_str(char c);

/**
 *  Log trace string and indicate control characters with a <> escape sequence
 *
 *  @param str              String to report
 */
void px_ascii_log_report_str(const char * str);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
