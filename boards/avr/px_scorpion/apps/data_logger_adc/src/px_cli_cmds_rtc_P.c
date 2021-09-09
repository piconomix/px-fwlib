/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2014 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_cli_cmds_rtc.h : CLI commands for RTC
    Author(s):      Pieter Conradie
    Creation Date:  2014-05-31

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_cli.h"
#include "px_pgm_P.h"
#include "px_rtc_util.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("cli_cmds_rtc");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static const char * px_cli_cmd_fn_rtc_rd(uint8_t argc, char* argv[])
{
    px_rtc_date_time_t date_time;

    px_rtc_util_date_time_rd(&date_time);
    px_rtc_util_report_date_time(&date_time);
    putchar('\n');

    return NULL;
}

static const char * px_cli_cmd_fn_rtc_wr(uint8_t argc, char* argv[])
{
    px_rtc_date_time_t date_time;

    uint8_t year, month, day;
    uint8_t hour, min, sec;

    sscanf(argv[0],
           "%02hhu-%02hhu-%02hhu",
           &year, 
           &month, 
           &day);

    sscanf(argv[1],
           "%02hhu:%02hhu:%02hhu",
           &hour,
           &min,
           &sec);

    date_time.year  = year;
    date_time.month = month;
    date_time.day   = day;
    date_time.hour  = hour;
    date_time.min   = min;
    date_time.sec   = sec;

    if(!px_rtc_util_date_time_fields_valid(&date_time))
    {
        return "Error! One or more date-time fields are invalid";
    }

    px_rtc_util_date_time_wr(&date_time);
    return NULL;
}

// Create CLI command structures
PX_CLI_CMD_CREATE(px_cli_cmd_rtc_rd,  "r",    0, 0,   "",                         "Read RTC time")
PX_CLI_CMD_CREATE(px_cli_cmd_rtc_wr,  "w",    2, 2,   "<yy-mm-dd> <hh:mm:ss>",    "Write RTC time")

PX_CLI_GROUP_CREATE(px_cli_group_rtc, "rtc")
    PX_CLI_CMD_ADD(px_cli_cmd_rtc_rd,  px_cli_cmd_fn_rtc_rd)
    PX_CLI_CMD_ADD(px_cli_cmd_rtc_wr,  px_cli_cmd_fn_rtc_wr)
PX_CLI_GROUP_END()
