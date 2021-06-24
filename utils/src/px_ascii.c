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

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_ascii.h"

#include "px_log.h"
PX_LOG_NAME("px_ascii")

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static const char px_ascii_strings[][4] =
{
    {"NUL"},    // 0    Null
    {"SOH"},    // 1    Start of Header
    {"STX"},    // 2    Start of Text
    {"ETX"},    // 3    End of Text
    {"EOT"},    // 4    End of Transmission
    {"ENQ"},    // 5    Enquiry
    {"ACK"},    // 6    Acknowledgment  ("\a")
    {"BEL"},    // 7    Bell
    {"BS"},     // 8    Backspace       ("\b")
    {"HT"},     // 9    Horizontal Tab  ("\t")
    {"LF"},     // 10   Line Feed       ("\n")
    {"VT"},     // 11   Vertical Tab    ("\v")
    {"FF"},     // 12   Form Feed       ("\f")
    {"CR"},     // 13   Carriage Return ("\r")
    {"SO"},     // 14   Shift Out
    {"SI"},     // 15   Shift In
    {"DLE"},    // 16   Data Link Escape
    {"XON"},    // 17   XON
    {"DC2"},    // 18   Device Control 2
    {"XOF"},    // 19   XOFF
    {"DC4"},    // 20   Device Control 4
    {"NAK"},    // 21   Negative Acknowledgement
    {"SYN"},    // 22   Synchronous Idle
    {"ETB"},    // 23   End of Transmission Block
    {"CAN"},    // 24   Cancel
    {"EM"},     // 25   End of Medium
    {"SUB"},    // 26   Substitute
    {"ESC"},    // 27   Escape          ("\e"). Note that this is a non-standard code
    {"FS"},     // 28   File Separator
    {"GS"},     // 29   Group Separator
    {"RS"},     // 30   Record Separator
    {"US"},     // 31   Unit Separator
};

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
const char * px_ascii_char_to_name_str(char c)
{
    if(c == PX_ASCII_CHAR_DEL)
    {
        return "DEL";
    }
    else if(c > 31)
    {
        return "???";
    }
    else
    {
        return &px_ascii_strings[(uint8_t)c][0];
    }
}

void px_ascii_log_report_str(const char * str)
{
    while(*str != 0)
    {
        if((*str <= 31) || (*str == PX_ASCII_CHAR_DEL))
        {
            PX_LOG_TRACE("<%s>", px_ascii_char_to_name_str(*str));
        }
        else if(*str > 127)
        {
            PX_LOG_TRACE("<%02X>", *str);
        }
        else
        {
            PX_LOG_TRACE("%c", *str);
        }
        str++;
    }
}

