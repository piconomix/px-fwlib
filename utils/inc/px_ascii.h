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
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
 
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
 *  - utils/src/px_ascii.h
 *  
 *  @see https://en.wikipedia.org/wiki/ASCII
 *  @see https://en.wikipedia.org/wiki/Escape_sequences_in_C
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

#ifdef __cplusplus
extern "C"
{
#endif
/* _____DEFINITIONS__________________________________________________________ */
#define PX_ASCII_NUL         ((char)0x00)   // 0    Null
#define PX_ASCII_SOH         ((char)0x01)   // 1    Start of Heading
#define PX_ASCII_STX         ((char)0x02)   // 2    Start of Text
#define PX_ASCII_ETX         ((char)0x03)   // 3    End of Text
#define PX_ASCII_EOT         ((char)0x04)   // 4    End of Transmission
#define PX_ASCII_ENQ         ((char)0x05)   // 5    Enquiry
#define PX_ASCII_ACK         ((char)0x06)   // 6    Acknowledgment  ("\a")
#define PX_ASCII_BEL         ((char)0x07)   // 7    Bell
#define PX_ASCII_BS          ((char)0x08)   // 8    Backspace       ("\b")
#define PX_ASCII_HT          ((char)0x09)   // 9    Horizontal Tab  ("\t")
#define PX_ASCII_LF          ((char)0x0a)   // 10   Line Feed       ("\n")
#define PX_ASCII_VT          ((char)0x0b)   // 11   Vertical Tab    ("\v")
#define PX_ASCII_FF          ((char)0x0c)   // 12   Form Feed       ("\f")
#define PX_ASCII_CR          ((char)0x0d)   // 13   Carriage Return ("\r")
#define PX_ASCII_SO          ((char)0x0e)   // 14   Shift Out
#define PX_ASCII_SI          ((char)0x0f)   // 15   Shift In
#define PX_ASCII_DLE         ((char)0x10)   // 16   Data Link Escape
#define PX_ASCII_DC1         ((char)0x11)   // 17   Device Control 1 (often XON)
#define PX_ASCII_XON         ((char)0x11)   // 17   XON
#define PX_ASCII_DC2         ((char)0x12)   // 18   Device Control 2
#define PX_ASCII_DC3         ((char)0x13)   // 19   Device Control 3 (often XOFF)
#define PX_ASCII_XOFF        ((char)0x13)   // 19   XOFF
#define PX_ASCII_DC4         ((char)0x14)   // 20   Device Control 4
#define PX_ASCII_NAK         ((char)0x15)   // 21   Negative Acknowledgement
#define PX_ASCII_SYN         ((char)0x16)   // 22   Synchronous Idle
#define PX_ASCII_ETB         ((char)0x17)   // 23   End of Transmission Block
#define PX_ASCII_CAN         ((char)0x18)   // 24   Cancel
#define PX_ASCII_EM          ((char)0x19)   // 25   End of Medium
#define PX_ASCII_SUB         ((char)0x1a)   // 26   Substitute
#define PX_ASCII_ESC         ((char)0x1b)   // 27   Escape          ("\e"). Note that this is a non-standard code
#define PX_ASCII_FS          ((char)0x1c)   // 28   File Separator
#define PX_ASCII_GS          ((char)0x1d)   // 29   Group Separator
#define PX_ASCII_RS          ((char)0x1e)   // 30   Record Separator
#define PX_ASCII_US          ((char)0x1f)   // 31   Unit Separator
#define PX_ASCII_DEL         ((char)0x7f)   // 128  Delete

#define PX_ASCII_NUL_STR     "\x00"         // 0    Null
#define PX_ASCII_SOH_STR     "\x01"         // 1    Start of Header
#define PX_ASCII_STX_STR     "\x02"         // 2    Start of Text
#define PX_ASCII_ETX_STR     "\x03"         // 3    End of Text
#define PX_ASCII_EOT_STR     "\x04"         // 4    End of Transmission
#define PX_ASCII_ENQ_STR     "\x05"         // 5    Enquiry
#define PX_ASCII_ACK_STR     "\x06"         // 6    Acknowledgment  ("\a")
#define PX_ASCII_BEL_STR     "\x07"         // 7    Bell
#define PX_ASCII_BS_STR      "\x08"         // 8    Backspace       ("\b")
#define PX_ASCII_HT_STR      "\x09"         // 9    Horizontal Tab  ("\t")
#define PX_ASCII_LF_STR      "\x0a"         // 10   Line Feed       ("\n")
#define PX_ASCII_VT_STR      "\x0b"         // 11   Vertical Tab    ("\v")
#define PX_ASCII_FF_STR      "\x0c"         // 12   Form Feed       ("\f")
#define PX_ASCII_CR_STR      "\x0d"         // 13   Carriage Return ("\r")
#define PX_ASCII_SO_STR      "\x0e"         // 14   Shift Out
#define PX_ASCII_SI_STR      "\x0f"         // 15   Shift In
#define PX_ASCII_DLE_STR     "\x10"         // 16   Data Link Escape
#define PX_ASCII_DC1_STR     "\x11"         // 17   Device Control 1 (often XON)
#define PX_ASCII_XON_STR     "\x11"         // 17   XON
#define PX_ASCII_DC2_STR     "\x12"         // 18   Device Control 2
#define PX_ASCII_DC3_STR     "\x13"         // 19   Device Control 3 (often XOFF)
#define PX_ASCII_XOFF_STR    "\x13"         // 19   XOFF
#define PX_ASCII_DC4_STR     "\x14"         // 20   Device Control 4
#define PX_ASCII_NAK_STR     "\x15"         // 21   Negative Acknowledgement
#define PX_ASCII_SYN_STR     "\x16"         // 22   Synchronous Idle
#define PX_ASCII_ETB_STR     "\x17"         // 23   End of Transmission Block
#define PX_ASCII_CAN_STR     "\x18"         // 24   Cancel
#define PX_ASCII_EM_STR      "\x19"         // 25   End of Medium
#define PX_ASCII_SUB_STR     "\x1a"         // 26   Substitute
#define PX_ASCII_ESC_STR     "\x1b"         // 27   Escape          ("\e"). Note that this is a non-standard code
#define PX_ASCII_FS_STR      "\x1c"         // 28   File Separator
#define PX_ASCII_GS_STR      "\x1d"         // 29   Group Separator
#define PX_ASCII_RS_STR      "\x1e"         // 30   Record Separator
#define PX_ASCII_US_STR      "\x1f"         // 31   Unit Separator
#define PX_ASCII_DEL_STR     "\x7f"         // 127  Delete

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

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif
