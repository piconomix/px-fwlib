/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2006 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md

    Title:          px_cli.h : Command Line Interpreter and command dispatcher
    Author(s):      Pieter Conradie
    Creation Date:  2008-08-01

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <string.h>
#include <stdio.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_cli.h"
#include "px_vt100.h"
#include "px_pgm_P.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("px_cli_P");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
/// Converted argument value using px_cli_util_argv_to_...() conversion function
px_cli_argv_val_t px_cli_argv_val;

/* _____LOCAL VARIABLES______________________________________________________ */
/// Pointer to root array of commands
static const px_cli_cmd_list_item_t * px_cli_cmd_list;

/// Character buffer for command line
static char    px_cli_line_buf[PX_CLI_CFG_LINE_LENGTH_MAX];
/// Index of next free position in character buffer
static uint8_t px_cli_line_buf_index;

/// Command line broken up into an array of pointers to strings
static char * px_cli_argv[PX_CLI_CFG_ARGV_MAX];

/// Autocomplete index that is used to mark start of word used for match
static uint8_t px_cli_autocomplete_start_index;
/// Autocomplete index that is used to mark end of word used for match
static uint8_t px_cli_autocomplete_end_index;

#if PX_CLI_CFG_HISTORY_SIZE
/**
 *  Circular buffer to store history of cmd line strings entered by user.
 *
 *  Cmd line strings are saved in this circular buffer as a series of zero
 *  terminated strings. If the newest string partially overwrites the oldest
 *  string, the remaining characters of the oldest string is also zeroed.
 *
 *  Example:
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *                                            px_cli_hist_index_last
 *                                                     |
 *  |  0  | 'O' | 'N' | 'E' |  0  | 'T' | 'W' | 'O' |  0  |  0  |  0  |
 *                                   |
 *                           px_cli_hist_index_now
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  @see px_cli_hist_save_cmd()
 */
static char px_cli_hist_circ_buf[PX_CLI_CFG_HISTORY_SIZE];

#if (PX_CLI_CFG_HISTORY_SIZE <= 256)
typedef uint8_t  px_cli_hist_size_t;
#else
typedef uint16_t px_cli_hist_size_t;
#endif
/// Zero terminated *END* of last cmd line string stored in circular buffer
static px_cli_hist_size_t   px_cli_hist_index_last;
/// Zero terminated *START* of current cmd line string displayed
static px_cli_hist_size_t   px_cli_hist_index_now;
#endif

// Copy of current command list item being processed
static px_cli_cmd_list_item_t px_cli_cmd_list_item;

// Copy of current command list item data being processed
static union
{
    px_cli_cmd_t   cmd;        ///< command data
    px_cli_group_t group;      ///< group data
} px_cli_cmd_list_item_data;

/**
 *  Tree path of current command list item.
 *  @see https://en.wikipedia.org/wiki/Tree_(data_structure)
 *
 *  This array stores a pointer to the current command list item (node) being
 *  referenced, as well as it's parent command (node), it's grand-parent
 *  command (node), etc.
 */
static const px_cli_cmd_list_item_t * px_cli_tree_path[PX_CLI_CFG_TREE_DEPTH_MAX];

// Tree path depth of current command list item
static uint8_t px_cli_tree_path_depth;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */
static bool    px_cli_cmd_get_item       (const px_cli_cmd_list_item_t * item);
static bool    px_cli_cmd_item_get_root  (void);
static bool    px_cli_cmd_item_get_parent(void);
static bool    px_cli_cmd_item_get_child (void);
static bool    px_cli_cmd_item_get_first (void);
static bool    px_cli_cmd_item_get_next  (void);

static uint8_t px_cli_cmd_line_to_args   (void);

#if PX_CLI_CFG_HISTORY_SIZE
static px_cli_hist_size_t px_cli_hist_circ_buf_index_prev(px_cli_hist_size_t index) PX_ATTR_CONST;
static px_cli_hist_size_t px_cli_hist_circ_buf_index_next(px_cli_hist_size_t index) PX_ATTR_CONST;
static void    px_cli_hist_copy          (void);
static void    px_cli_hist_save_cmd      (void);
static void    px_cli_hist_load_older_cmd(void);
static void    px_cli_hist_load_newer_cmd(void);
#endif

static void    px_cli_autocomplete_reset (void);
static bool    px_cli_autocomplete       (void);
static void    px_cli_cmd_exe            (void);

/* _____LOCAL FUNCTIONS______________________________________________________ */
static bool px_cli_cmd_get_item(const px_cli_cmd_list_item_t * item)
{
    // Copy list item structure to RAM
    memcpy_P(&px_cli_cmd_list_item, item, sizeof(px_cli_cmd_list_item));

    // End of list?
    if(px_cli_cmd_list_item.cmd == NULL)
    {
        return false;
    }

    // Is this a command?
    if(px_cli_cmd_list_item.handler != NULL)
    {
        // Copy command structure to RAM
        memcpy_P(&px_cli_cmd_list_item_data.cmd,
                 px_cli_cmd_list_item.cmd,
                 sizeof(px_cli_cmd_list_item_data.cmd));
    }
    else
    {
        // Copy group structure to RAM
        memcpy_P(&px_cli_cmd_list_item_data.group,
                 px_cli_cmd_list_item.group,
                 sizeof(px_cli_cmd_list_item_data.group));
    }

    return true;
}

static bool px_cli_cmd_item_get_root(void)
{
    // Reset to first item in root list
    px_cli_tree_path_depth = 0;
    px_cli_tree_path[0]    = &px_cli_cmd_list[0];

    // Read first item in root list
    return px_cli_cmd_get_item(px_cli_tree_path[px_cli_tree_path_depth]);
}

static bool px_cli_cmd_item_get_parent(void)
{
    // Already in root list?
    if(px_cli_tree_path_depth == 0)
    {
        PX_LOG_E("Already in root");
        return false;
    }

    // Go back to parent
    px_cli_tree_path_depth--;

    // Read parent
    return px_cli_cmd_get_item(px_cli_tree_path[px_cli_tree_path_depth]);
}

static bool px_cli_cmd_item_get_child(void)
{
    // End of list or command item?
    if(  (px_cli_cmd_list_item.cmd     == NULL)
       ||(px_cli_cmd_list_item.handler != NULL)  )
    {
        PX_LOG_E("Not a group item");
        return false;
    }

    // Maximum depth reached?
    if(px_cli_tree_path_depth >= (PX_CLI_CFG_TREE_DEPTH_MAX-1))
    {
        PX_LOG_E("Maximum command depth exceeded");
        return false;
    }

    // Go to first item in child list
    px_cli_tree_path_depth++;
    px_cli_tree_path[px_cli_tree_path_depth] = px_cli_cmd_list_item_data.group.list;

    // Read first item in child list
    return px_cli_cmd_get_item(px_cli_tree_path[px_cli_tree_path_depth]);
}

static bool px_cli_cmd_item_get_first(void)
{
    // Root list?
    if(px_cli_tree_path_depth == 0)
    {
        // Reset to start of root list
        px_cli_tree_path[0] = px_cli_cmd_list;
    }
    else
    {
        // Get parent item
        px_cli_cmd_get_item(px_cli_tree_path[px_cli_tree_path_depth-1]);
        // Go to first item in child list
        px_cli_tree_path[px_cli_tree_path_depth] = px_cli_cmd_list_item_data.group.list;
    }

    // Read first item (sibling)
    return px_cli_cmd_get_item(px_cli_tree_path[px_cli_tree_path_depth]);
}

static bool px_cli_cmd_item_get_next(void)
{
    // End of list reached?
    if(px_cli_cmd_list_item.cmd == NULL)
    {
        PX_LOG_E("End of list already reached");
        return false;
    }

    // Advance to next item in list
    px_cli_tree_path[px_cli_tree_path_depth]++;

    // Read next item in list (sibling)
    return px_cli_cmd_get_item(px_cli_tree_path[px_cli_tree_path_depth]);
}

static uint8_t px_cli_cmd_line_to_args(void)
{
    uint8_t argc;
    char *  str;
    bool    quote_flag;

    // Zero terminate command line
    px_cli_line_buf[px_cli_line_buf_index] = '\0';

    // Break command line string up into separate words:
    // Array of pointers to zero terminated strings
    argc       = 0;
    str        = px_cli_line_buf;
    quote_flag = false;
    while(true)
    {
        // Replace spaces with zero termination
        while(*str == ' ')
        {
            *str++ = '\0';
        }
        // End of line?
        if(*str == '\0')
        {
            break;
        }
        // Maximum number of arguments exceeded?
        if(argc >= PX_CLI_CFG_ARGV_MAX)
        {
            break;
        }
        // Starts with a quote?
        if(*str == '"')
        {
            // Discard quote character
            str++;
            // Set flag to indicate that parameter consists of a quoted string
            quote_flag = true;
        }
        // Save start of word and increment argument count
        px_cli_argv[argc++] = str;

        // Quoted string?
        if(quote_flag)
        {
            // Move to end of quote
            while(  (*str != '"') && (*str != '\0')  )
            {
                str++;
            }
            // End quote found?
            if(*str == '"')
            {
                // Replace with zero termination
                *str = '\0';
                // Next character
                str++;
            }
        }

        // Move to end of word
        while(  (*str != ' ') && (*str != '\0')  )
        {
            str++;
        }
    }

    return argc;
}

#if PX_CLI_CFG_HISTORY_SIZE
static px_cli_hist_size_t px_cli_hist_circ_buf_index_next(px_cli_hist_size_t index)
{
    // Wrap?
    if(index >= (PX_CLI_CFG_HISTORY_SIZE-1))
    {
        // Yes
        return 0;
    }
    else
    {
        // No
        return (index+1);
    }
}

static px_cli_hist_size_t px_cli_hist_circ_buf_index_prev(px_cli_hist_size_t index)
{
    // Wrap?
    if(index == 0)
    {
        // Yes
        return (PX_CLI_CFG_HISTORY_SIZE-1);
    }
    else
    {
        // No
        return (index-1);
    }
}

static void px_cli_hist_copy(void)
{
    uint8_t            data;
    px_cli_hist_size_t i;
    px_cli_hist_size_t j;

    // Delete old command from terminal
    px_vt100_del_chars(px_cli_line_buf_index);

    // Copy characters from history to command line
    i = 0;
    j = px_cli_hist_index_now;
    while(true)
    {
        // Fetch character from history
        data = px_cli_hist_circ_buf[j];
        // End reached?
        if(data == '\0')
        {
            break;
        }
        // Send character to terminal
        putchar(data);
        // Copy character to cmd line buffer
        px_cli_line_buf[i++] = data;
        // Next index
        j = px_cli_hist_circ_buf_index_next(j);
    }
    px_cli_line_buf_index = i;
}

static void px_cli_hist_save_cmd(void)
{
    px_cli_hist_size_t i;
    px_cli_hist_size_t j;

    // Empty command?
    if(px_cli_line_buf_index == 0)
    {
        // Reset up/down history to end of latest saved command
        px_cli_hist_index_now = px_cli_hist_index_last;
        return;
    }

    // Duplicate command?
    i = px_cli_line_buf_index;
    j = px_cli_hist_index_last;
    while(true)
    {
        // Previous index
        i--;
        j = px_cli_hist_circ_buf_index_prev(j);

        // No match?
        if(px_cli_line_buf[i] != px_cli_hist_circ_buf[j])
        {
            // New command
            break;
        }
        // Complete match?
        if(i == 0)
        {
            // Duplicate command... reset up/down history
            px_cli_hist_index_now = px_cli_hist_index_last;
            return;
        }
    }

    // Append command line string (except terminating zero) in history circular
    // buffer
    i = 0;
    j = px_cli_hist_index_last;
    do
    {
        // Next index
        j = px_cli_hist_circ_buf_index_next(j);
        // Append character from line buffer
        px_cli_hist_circ_buf[j] = px_cli_line_buf[i++];
    }
    while(i < px_cli_line_buf_index);

    // Remember end of last saved string
    j = px_cli_hist_circ_buf_index_next(j);
    px_cli_hist_index_last = j;
    // Reset up/down history to end of latest saved command
    px_cli_hist_index_now = j;

    // Zero terminate and eat remaining characters of oldest command line in
    // history that may have been partially overwritten.
    while(true)
    {
        // Terminating zero reached?
        if(px_cli_hist_circ_buf[j] == '\0')
        {
            // Stop
            break;
        }
        else
        {
            // Reset to zero
            px_cli_hist_circ_buf[j] = '\0';
        }
        // Next index
        j = px_cli_hist_circ_buf_index_next(j);
    }
}

static void px_cli_hist_load_older_cmd(void)
{
    px_cli_hist_size_t i;

    // Oldest in history already displayed?
    i = px_cli_hist_circ_buf_index_prev(px_cli_hist_index_now);
    if(i == px_cli_hist_index_last)
    {
        return;
    }
    i = px_cli_hist_circ_buf_index_prev(i);
    if(px_cli_hist_circ_buf[i] == '\0')
    {
        return;
    }

    // Find start of older cmd saved in history
    while(px_cli_hist_circ_buf[i] != '\0')
    {
        i = px_cli_hist_circ_buf_index_prev(i);
    }
    px_cli_hist_index_now = px_cli_hist_circ_buf_index_next(i);

    // Replace current command line with one stored in history
    px_cli_hist_copy();
}

static void px_cli_hist_load_newer_cmd(void)
{
    px_cli_hist_size_t i;

    // Find start of newer cmd saved in history
    i = px_cli_hist_index_now;
    while(px_cli_hist_circ_buf[i] != '\0')
    {
        i = px_cli_hist_circ_buf_index_next(i);
    }

    // Newest command already displayed?
    if(i != px_cli_hist_index_last)
    {
        // Move index to start of string
        i = px_cli_hist_circ_buf_index_next(i);
    }
    px_cli_hist_index_now = i;

    // Replace current command line with one stored in history
    px_cli_hist_copy();
}
#endif

static void px_cli_autocomplete_reset(void)
{
    // Reset autocomplete to last typed character
    px_cli_autocomplete_start_index = 0;
    px_cli_autocomplete_end_index   = px_cli_line_buf_index;

    // Start at first item in root list
    px_cli_cmd_item_get_root();
}

static bool px_cli_autocomplete(void)
{
    uint8_t                        i;
    const char *                   name;
    const px_cli_cmd_list_item_t * cmd_start = px_cli_tree_path[px_cli_tree_path_depth];

    i = px_cli_autocomplete_start_index;
    while(true)
    {
        name = px_cli_cmd_list_item_data.cmd.name;

        // Does name match line?
        while(i < px_cli_autocomplete_end_index)
        {
            // Fetch line character
            char line_char = px_cli_line_buf[i++];
            // Fetch name character
            char name_char = px_pgm_rd_char(name++);

            // End of name *and* end of word reached?
            if(  (name_char == '\0') && (line_char == ' ')  )
            {
                // Name match... is this a group item?
                if(px_cli_cmd_list_item.handler == NULL)
                {
                    // Proceed to child name
                    px_cli_cmd_item_get_child();
                    name = px_cli_cmd_list_item_data.cmd.name;
                    // Set start index to start of child command
                    px_cli_autocomplete_start_index = i;
                    break;
                }
                else
                {
                    // This is a command item... no match
                    return false;
                }
            }
            // String and name character mismatch?
            else if(line_char != name_char)
            {
                // No match.. reset to start of word
                i = px_cli_autocomplete_start_index;
                // Proceed to next item
                if(!px_cli_cmd_item_get_next())
                {
                    // Go back to start of list
                    px_cli_cmd_item_get_first();
                }
                // Reset to start of word
                i = px_cli_autocomplete_start_index;
                break;
            }
        }

        // Autocomplete match reached?
        if(i >= px_cli_autocomplete_end_index)
        {
            // (Partial) match
            break;
        }

        // Cycled through list?
        if(px_cli_tree_path[px_cli_tree_path_depth] == cmd_start)
        {
            // No match
            return false;
        }
    }

    // Autocomplete rest of name
    px_vt100_del_chars(px_cli_line_buf_index - px_cli_autocomplete_end_index);
    while(true)
    {
        char name_char = px_pgm_rd_char(name++);
        if(name_char == '\0')
        {
            break;
        }
        if(i >= (PX_CLI_CFG_LINE_LENGTH_MAX-1))
        {
            break;
        }
        px_cli_line_buf[i++] = name_char;
        putchar(name_char);
    }
    px_cli_line_buf_index = i;

    // Next item in list for next autocomplete
    if(!px_cli_cmd_item_get_next())
    {
        // Go back to start of list
        px_cli_cmd_item_get_first();
    }

    return true;
}

static void px_cli_cmd_exe(void)
{
    uint8_t      argc;
    char **      argv;
    const char * report_str;

    /*
       Break command line string up into separate words:
       Array of pointers to zero terminated strings
     */
    argc = px_cli_cmd_line_to_args();

    // Ignore empty command
    if(argc == 0)
    {
        return;
    }

    // Ignore command starting with a hash (#) as it is regarded as a comment
    if(px_cli_argv[0][0] == '#')
    {
        return;
    }

    // Find command in command list
    px_cli_cmd_item_get_root();
    while(true)
    {
        // End of list or not enough arguments?
        if(  (px_cli_cmd_list_item.cmd == NULL)
           ||(px_cli_tree_path_depth   >= argc)  )
        {
            // Command not found in list
            PX_PRINTF_P("Error! Command not found\n");
            return;
        }

        // Does the argument match the command string?
        if(strcmp_P(px_cli_argv[px_cli_tree_path_depth],
                    px_cli_cmd_list_item_data.cmd.name) == 0)
        {
            // Is this a command item?
            if(px_cli_cmd_list_item.handler != NULL)
            {
                // Command match
                break;
            }
            else
            {
                // Group item match... proceed to child list
                px_cli_cmd_item_get_child();

            }
        }
        else
        {
            // Next item in list
            px_cli_cmd_item_get_next();
        }
    }

    // Remove command argument(s)
    argc -= (px_cli_tree_path_depth+1);
    argv  = &px_cli_argv[px_cli_tree_path_depth+1];

    // Does number of parameters exceed bounds?
    if(  (argc < px_cli_cmd_list_item_data.cmd.argc_min)
       ||(argc > px_cli_cmd_list_item_data.cmd.argc_max)  )
    {
        PX_PRINTF_P("Error! Number of parameters incorrect\n");
        return;
    }

#if PX_CLI_CFG_COLOR
    // Set font color to green
    printf(PX_VT100_SET_FG_GREEN);
#endif

    // Execute command with parameters
    report_str = (*(px_cli_cmd_list_item.handler))(argc, argv);

    // Did handler report a string to display?
    if(report_str != NULL)
    {
        // Display string
        printf_P(report_str);
        // Append newline character
        putchar('\n');
    }
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_cli_init(const px_cli_cmd_list_item_t * cli_cmd_list, const char* startup_str)
{
#if PX_CLI_CFG_HISTORY_SIZE
    px_cli_hist_size_t i;

    // Clear history buffer
    for(i=0; i<PX_CLI_CFG_HISTORY_SIZE; i++)
    {
        px_cli_hist_circ_buf[i] = '\0';
    }
    px_cli_hist_index_last = 0;
    px_cli_hist_index_now  = 0;
#endif

    // Save pointer to root of command list
    PX_LOG_ASSERT(cli_cmd_list != NULL);
    px_cli_cmd_list = cli_cmd_list;

    // Reset
    px_cli_line_buf_index = 0;
    px_cli_autocomplete_reset();

    // Reset Terminal
    px_vt100_init();

    // Display startup string
    if(startup_str != NULL)
    {
        printf_P(startup_str);
    }

#if PX_CLI_CFG_COLOR
    // Set font color to green
    printf(PX_VT100_SET_FG_GREEN);
#endif

    // Display start up help advice
#if PX_CLI_CFG_DISP_HELP_STR
    PX_PRINTF_P("Type 'help' to get list of cmds with help descriptions\n\n"
#else
    PX_PRINTF_P("Type 'help' to get list of cmds\n\n"
#endif
                "TAB   to cycle/autocomplete cmd(s)\n"
#if PX_CLI_CFG_HISTORY_SIZE
                "UP/DN to recall old cmds in history\n"
#endif
                "ENTER to execute cmd\n"
                "<...> are required parameters\n"
                "[...] are optional parameters\n\n");

#if PX_CLI_CFG_COLOR
    // Restore font color
    printf(PX_VT100_RST_ALL_ATTRS);
#endif

    // Display prompt
    putchar('>');
}

void px_cli_on_rx_char(char data)
{
    // Process received character to detect ANSI Escape Sequences
    switch(px_vt100_on_rx_char(data))
    {
    case PX_VT100_CHAR_NORMAL:
        switch(data)
        {
        // ENTER has been pressed
        case PX_CLI_CFG_CHAR_ENTER:
            // Terminate line
            putchar('\n');
#if PX_CLI_CFG_HISTORY_SIZE
            // Save command
            px_cli_hist_save_cmd();
#endif
            // Execute command
            px_cli_cmd_exe();
            // Reset command buffer
            px_cli_line_buf_index = 0;
            // Reset autocomplete
            px_cli_autocomplete_reset();
#if PX_CLI_CFG_COLOR
            // Restore font color
            printf(PX_VT100_RST_ALL_ATTRS);
#endif
            // Display prompt
            putchar('>');
            return;

        // BACK SPACE has been pressed
        case PX_VT100_CHAR_BS:
            // Buffer not empty?
            if(px_cli_line_buf_index > 0)
            {
                // Remove last character from buffer
                px_cli_line_buf_index--;
                // Remove last character from terminal screen
                px_vt100_del_chars(1);
                // Reset autocomplete to last character
                px_cli_autocomplete_reset();
            }
            else
            {
                // No characters to delete
                putchar(PX_VT100_CHAR_BEL);
            }
            return;

        // TAB has been pressed
        case PX_VT100_CHAR_TAB:
            if(!px_cli_autocomplete())
            {
                // Autocomplete failed
                putchar(PX_VT100_CHAR_BEL);
            }
            return;

        default:
            break;
        }

        // Ignore invalid values
        if(data < 0x20)
        {
            return;
        }
        // Buffer not full?
        if(px_cli_line_buf_index < (PX_CLI_CFG_LINE_LENGTH_MAX-1))
        {
            // Add character to line buffer
            px_cli_line_buf[px_cli_line_buf_index++] = data;
            // Reset autocomplete to last character
            px_cli_autocomplete_reset();
#if PX_CLI_CFG_ECHO_CHARS
            // Echo character
            putchar(data);
#endif
        }
        else
        {
            // Buffer full
            putchar(PX_VT100_CHAR_BEL);
        }
        return;

    case PX_VT100_CHAR_INVALID:
        // Discard/ignore character
        return;

    case PX_VT100_ESC_SEQ_BUSY:
        // Discard/ignore character
        return;

#if PX_CLI_CFG_HISTORY_SIZE
    case PX_VT100_ESC_SEQ_ARROW_UP:
        px_cli_hist_load_older_cmd();
        return;

    case PX_VT100_ESC_SEQ_ARROW_DN:
        px_cli_hist_load_newer_cmd();
        return;
#endif

    default:
        return;
    }
}

const char* px_cli_cmd_help_fn(uint8_t argc, char* argv[])
{
    uint8_t i;
    uint8_t len;
    uint8_t name_char_cnt;
    uint8_t param_char_cnt;
    bool    line_break;

    // Find longest command and param length
    name_char_cnt  = PX_CLI_CFG_NAME_STR_MAX_SIZE;
    param_char_cnt = PX_CLI_CFG_PARAM_STR_MAX_SIZE;

#if ((PX_CLI_CFG_NAME_STR_MAX_SIZE == 0) || (PX_CLI_CFG_PARAM_STR_MAX_SIZE == 0))
    px_cli_cmd_item_get_root();
    while(true)
    {
        // End of list?
        if(px_cli_cmd_list_item.cmd == NULL)
        {
            // Root list?
            if(px_cli_tree_path_depth == 0)
            {
                // The end has been reached
                break;
            }
            else
            {
                // Return to parent list
                px_cli_cmd_item_get_parent();
                // Next item
                px_cli_cmd_item_get_next();
                continue;
            }
        }

        // Is this a command item?
        if(px_cli_cmd_list_item.handler != NULL)
        {
            // Longest command string?
            len = 0;
            for(i=0; i<=px_cli_tree_path_depth; i++)
            {
                px_cli_cmd_get_item(px_cli_tree_path[i]);
                len += strlen_P(px_cli_cmd_list_item_data.cmd.name) + 1;
            }
            if(name_char_cnt < len)
            {
                // Remember longest command string
                name_char_cnt = len;
            }
            // Longest param string?
            len = strlen_P(px_cli_cmd_list_item_data.cmd.param);
            if(param_char_cnt < len)
            {
                // Remember longest param string
                param_char_cnt = len;
            }

            // Next item in list
            px_cli_cmd_item_get_next();
        }
        else
        {
            // Group item... proceed to child list
            px_cli_cmd_item_get_child();
        }
    }
    PX_LOG_D("Max command chars = %d", name_char_cnt);
    PX_LOG_D("Max param chars = %d", param_char_cnt);
#endif

    // Display help for each command in list
    px_cli_cmd_item_get_root();
    line_break = false;
    while(true)
    {
        // End of list?
        if(px_cli_cmd_list_item.cmd == NULL)
        {
            // Root list?
            if(px_cli_tree_path_depth == 0)
            {
                // The end has been reached
                break;
            }
            else
            {
                // Return to parent list
                px_cli_cmd_item_get_parent();
                // Next item
                px_cli_cmd_item_get_next();
                // Insert line break
                line_break = true;
                continue;
            }
        }

        // Is this a command item?
        if(px_cli_cmd_list_item.handler != NULL)
        {
            px_cli_cmd_get_item(px_cli_tree_path[0]);
            if(  (argc == 0)
               ||(strncmp_P(argv[0],
                            px_cli_cmd_list_item_data.cmd.name,
                            strlen(argv[0])                 ) == 0)  )
            {
                // Insert line break?
                if(  (argc == 0) && (line_break)  )
                {
                    line_break = false;
                    putchar('\n');
                }

                // Display all command strings
                len = 0;
                for(i=0; i<=px_cli_tree_path_depth; i++)
                {
                    // Display name
                    px_cli_cmd_get_item(px_cli_tree_path[i]);
                    printf_P(px_cli_cmd_list_item_data.cmd.name);
                    putchar(' ');
                    len += strlen_P(px_cli_cmd_list_item_data.cmd.name) + 1;
                }

                // Adjust column
                for(i = len; i < name_char_cnt; i++)
                {
                    putchar(' ');
                }

                // Display param
                printf_P(px_cli_cmd_list_item_data.cmd.param);
#if PX_CLI_CFG_DISP_HELP_STR
                // Adjust column
                len = strlen_P(px_cli_cmd_list_item_data.cmd.param);
                for(i = len; i < param_char_cnt; i++)
                {
                    putchar(' ');
                }

                PX_PRINTF_P(" : ");
                // Display help string
                printf_P(px_cli_cmd_list_item_data.cmd.help);
#endif
                putchar('\n');
            }

            // Next item in list
            px_cli_cmd_item_get_next();
        }
        else
        {
            // Group item... proceed to child list
            px_cli_cmd_item_get_child();
            // Insert line break;
            line_break = true;
        }
    }

    return NULL;
}

uint8_t px_cli_util_argv_to_option(uint8_t argv_index, const char* options)
{
    uint8_t index = 0;

    // Adjust index
    argv_index += px_cli_tree_path_depth+1;
    PX_LOG_ASSERT(argv_index < PX_CLI_CFG_ARGV_MAX);

    while(strlen_P(options) != 0)
    {
        if(strcmp_P(px_cli_argv[argv_index], options) == 0)
        {
            return index;
        }
        options += strlen_P(options) + 1;
        index++;
    }

    return 0xff;
}

bool px_cli_util_argv_to_u8(uint8_t argv_index, uint8_t min, uint8_t max)
{
    unsigned long i;
    char *        end;

    // Adjust index
    argv_index += px_cli_tree_path_depth+1;
    PX_LOG_ASSERT(argv_index < PX_CLI_CFG_ARGV_MAX);

    i = strtoul(px_cli_argv[argv_index], &end, 0);

    if(  (end == px_cli_argv[argv_index]) || (*end != '\0')  )
    {
        return false;
    }
    if((i >= min) && (i <= max) )
    {
        px_cli_argv_val.u8 = (uint8_t)i;
        return true;
    }
    return false;
}

bool px_cli_util_argv_to_u16(uint8_t argv_index, uint16_t min, uint16_t max)
{
    unsigned long i;
    char *        end;

    // Adjust index
    argv_index += px_cli_tree_path_depth+1;
    PX_LOG_ASSERT(argv_index < PX_CLI_CFG_ARGV_MAX);

    i = strtoul(px_cli_argv[argv_index], &end, 0);

    if(  (end == px_cli_argv[argv_index]) || (*end != '\0')  )
    {
        return false;
    }
    if((i >= min) && (i <= max) )
    {
        px_cli_argv_val.u16 = (uint16_t)i;
        return true;
    }
    return false;
}

bool px_cli_util_argv_to_u32(uint8_t argv_index, uint32_t min, uint32_t max)
{
    unsigned long i;
    char *        end;

    // Adjust index
    argv_index += px_cli_tree_path_depth+1;
    PX_LOG_ASSERT(argv_index < PX_CLI_CFG_ARGV_MAX);

    i = strtoul(px_cli_argv[argv_index], &end, 0);

    if(  (end == px_cli_argv[argv_index]) || (*end != '\0')  )
    {
        return false;
    }
    if((i >= min) && (i <= max) )
    {
        px_cli_argv_val.u32 = (uint32_t)i;
        return true;
    }
    return false;
}

bool px_cli_util_argv_to_s8(uint8_t argv_index, int8_t min, int8_t max)
{
    long   i;
    char * end;

    // Adjust index
    argv_index += px_cli_tree_path_depth+1;
    PX_LOG_ASSERT(argv_index < PX_CLI_CFG_ARGV_MAX);

    i = strtol(px_cli_argv[argv_index], &end, 0);

    if(  (end == px_cli_argv[argv_index]) || (*end != '\0')  )
    {
        return false;
    }
    if((i >= min) && (i <= max) )
    {
        px_cli_argv_val.s8 = (int8_t)i;
        return true;
    }
    return false;
}

bool px_cli_util_argv_to_s16(uint8_t argv_index, int16_t min, int16_t max)
{
    long   i;
    char * end;

    // Adjust index
    argv_index += px_cli_tree_path_depth+1;
    PX_LOG_ASSERT(argv_index < PX_CLI_CFG_ARGV_MAX);

    i = strtol(px_cli_argv[argv_index], &end, 0);

    if(  (end == px_cli_argv[argv_index]) || (*end != '\0')  )
    {
        return false;
    }
    if((i >= min) && (i <= max) )
    {
        px_cli_argv_val.s16 = (int16_t)i;
        return true;
    }
    return false;
}

bool px_cli_util_argv_to_s32(uint8_t argv_index, int32_t min, int32_t max)
{
    long   i;
    char * end;

    // Adjust index
    argv_index += px_cli_tree_path_depth+1;
    PX_LOG_ASSERT(argv_index < PX_CLI_CFG_ARGV_MAX);

    i = strtol(px_cli_argv[argv_index], &end, 0);

    if(  (end == px_cli_argv[argv_index]) || (*end != '\0')  )
    {
        return false;
    }
    if((i >= min) && (i <= max) )
    {
        px_cli_argv_val.s32 = (int32_t)i;
        return true;
    }
    return false;
}

bool px_cli_util_argv_to_float(uint8_t argv_index)
{
    double i;
    char * end;

    // Adjust index
    argv_index += px_cli_tree_path_depth+1;
    PX_LOG_ASSERT(argv_index < PX_CLI_CFG_ARGV_MAX);

    i = strtod(px_cli_argv[argv_index], &end);

    if(  (end == px_cli_argv[argv_index]) || (*end != '\0')  )
    {
        return false;
    }
    px_cli_argv_val.f = (float)i;
    return true;
}

bool px_cli_util_argv_to_double(uint8_t argv_index)
{
    double i;
    char * end;

    // Adjust index
    argv_index += px_cli_tree_path_depth+1;
    PX_LOG_ASSERT(argv_index < PX_CLI_CFG_ARGV_MAX);

    i = strtod(px_cli_argv[argv_index], &end);

    if(  (end == px_cli_argv[argv_index]) || (*end != '\0')  )
    {
        return false;
    }
    px_cli_argv_val.d = i;
    return true;
}

void px_cli_util_disp_buf(const uint8_t * data, size_t nr_of_bytes)
{
    size_t i, j;

    for(i=0; i<nr_of_bytes; i+= 16)
    {
        for(j=i; j<(i+16); j++)
        {
            if(j<nr_of_bytes)
            {
                PX_PRINTF_P("%02hX ", data[j]);
            }
            else
            {
                PX_PRINTF_P("   ");
            }
        }
        for(j=i; j<(i+16); j++)
        {
            if(j<nr_of_bytes)
            {
                if( (data[j] >= 32) && (data[j] <= 127) )
                {
                    PX_PRINTF_P("%c", data[j]);
                }
                else
                {
                    putchar('.');
                }
            }
            else
            {
                putchar(' ');
            }
        }
        putchar('\n');
    }
}

void px_cli_util_disp_data(const uint8_t * data, size_t nr_of_bytes)
{
    size_t i;

    for(i = 0; i < nr_of_bytes; i++)
    {
        PX_PRINTF_P("%02hX ", data[i]);
    }
}
