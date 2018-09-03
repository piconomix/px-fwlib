#ifndef __PX_CLI_H__
#define __PX_CLI_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2008 Pieter Conradie <https://piconomix.com>
 
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
    
    Title:          px_cli.h : Command Line Interpreter and command dispatcher
    Author(s):      Pieter Conradie
    Creation Date:  2008-08-01

============================================================================= */

/** 
 *  @ingroup COMMS
 *  @defgroup PX_CLI px_cli.h : Command Line Interpreter
 *  
 *  Implements a command line interpreter interfacing with an ANSI/VT100
 *  terminal emulator.
 *  
 *  File(s):
 *  - comms/px_cli.h
 *  - comms/px_cli_cfg_template.h
 *  - comms/px_cli.c (general version)
 *  - comms/px_cli_P.c (minimise RAM usage by using Program Memory. See @ref PX_PGM_P)
 *  
 *  References:
 *  - http://en.wikipedia.org/wiki/PX_VT100_escape_code
 *  - http://www.termsys.demon.co.uk/vtansi.htm
 *  
 *  This CLI supports autocomplete using TAB and recalling old commands
 *  using UP/DOWN (command history). Commands starting with a hash (#) is 
 *  regarded as comments and ignored.
 *  
 *  ![CLI using Tera Term terminal emulator](arduino_uno_board/arduino_uno_cli.png)
 *  
 *  Example:
 *  
 *  @include comms/test/px_cli_test.c
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_pgm_P.h"

// Include project specific config. See "px_cli_cfg_template.h"
#include "px_cli_cfg.h"

// Check that all project specific options have been correctly specified
#if (   !defined(PX_CLI_CFG_ARGV_MAX          ) \
     || !defined(PX_CLI_CFG_LINE_LENGTH_MAX   ) \
     || !defined(PX_CLI_CFG_TREE_DEPTH_MAX    ) \
     || !defined(PX_CLI_CFG_HISTORY_SIZE      ) \
     || !defined(PX_CLI_CFG_DISP_HELP_STR     ) \
     || !defined(PX_CLI_CFG_NAME_STR_MAX_SIZE ) \
     || !defined(PX_CLI_CFG_PARAM_STR_MAX_SIZE)  )
      )
#error "One or more options not defined in 'px_cli_cfg.h'"
#endif
#if (PX_CLI_CFG_LINE_LENGTH_MAX > 255)
#error "PX_CLI_CFG_LINE_LENGTH_MAX must be less than 256"
#endif
#if (PX_CLI_CFG_TREE_DEPTH_MAX < 1) || (PX_CLI_CFG_TREE_DEPTH_MAX > PX_CLI_CFG_ARGV_MAX)
#error "PX_CLI_CFG_LINE_LENGTH_MAX must be at least 1 and less than or equal to PX_CLI_CFG_ARGV_MAX"
#endif
#if (PX_CLI_CFG_HISTORY_SIZE != 0) && (PX_CLI_CFG_HISTORY_SIZE < PX_CLI_CFG_LINE_LENGTH_MAX)
#error "PX_CLI_CFG_HISTORY_SIZE must be equal or greater than PX_CLI_CFG_LINE_LENGTH_MAX"
#endif
#if (PX_CLI_CFG_HISTORY_SIZE > 65536)
#error "PX_CLI_CFG_HISTORY_SIZE must be equal or less than 65536"
#endif
#if (PX_CLI_CFG_NAME_STR_MAX_SIZE != 0) && (PX_CLI_CFG_PARAM_STR_MAX_SIZE == 0)
#error "PX_CLI_CFG_PARAM_STR_MAX_SIZE must also be specifed to reduce code size"
#endif
#if (PX_CLI_CFG_NAME_STR_MAX_SIZE == 0) && (PX_CLI_CFG_PARAM_STR_MAX_SIZE != 0)
#error "PX_CLI_CFG_NAME_STR_MAX_SIZE must also be specifed to reduce code size"
#endif

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS _________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */
/** 
 *  Pointer to a function that will be called to handle a command
 *   
 *  @param argc          Number of argument strings
 *  @param argv          Array of pointers to zero terminated argument strings
 *  
 *  @return const char * Response string to display; return NULL to display nothing
 */
typedef const char * (*px_cli_handler_t)(uint8_t argc, char * argv[]);

/// Command structure
typedef struct
{
    const char *  name;        ///< Command name
    uint8_t       argc_min;    ///< Minimum number of valid arguments
    uint8_t       argc_max;    ///< Maximum number of valid arguments
    const char *  param;       ///< Parameter string to be displayed when 'help' is executed
#if PX_CLI_CFG_DISP_HELP_STR
    const char *  help;        ///< Help string to be displayed when 'help' is executed
#endif
} px_cli_cmd_t;

/// Command group structure
typedef struct
{
    const char *                          name;     ///< Command name
    const struct px_cli_cmd_list_item_s * list;     ///< Pointer to command list array
} px_cli_group_t;

/// Command list item declaration
typedef struct px_cli_cmd_list_item_s
{
    px_cli_handler_t   handler;         ///< Function to be called when command is executed
    union
    {
        const px_cli_cmd_t *   cmd;     ///< pointer to command (if handler != NULL)
        const px_cli_group_t * group;   ///< pointer to group   (if handler == NULL)
    };
} px_cli_cmd_list_item_t;

/// Union of ARGV converted value types
typedef union
{
    uint8_t  u8;
    uint16_t u16;
    uint32_t u32;
    int8_t   s8;
    int16_t  s16;
    int32_t  s32;
    float    f;
    double   d;
} px_cli_argv_val_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */
/** 
 *  Converted argument value using px_cli_util_argv_to_...() conversion function.
 * 
 *  For example, if `px_cli_util_argv_to_u8()` is used, the answer is stored as
 *  `px_cli_argv_val.u8`
 *  
 *  Example:
 * 
 *      @code{.c}
 *      uint8_t i2c_sla_adr;
 *
 *      // <i2c_slave_adr>
 *      if(!px_cli_util_argv_to_u8(0, 0, 127))
 *      {
 *          return "Error. <i2c_slave_adr> must be 0 to 127";
 *      }
 *      i2c_sla_adr = px_cli_argv_val.u8;
 *      @endcode
 */
extern px_cli_argv_val_t    px_cli_argv_val;

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise command line module.
 *  
 *  @param startup_str   Start up string to display.
 */
extern void px_cli_init(const char * startup_str);

/** 
 *  Function called to handle a received character.
 *  
 *  This function drives the command line interpreter. All actions are taken in 
 *  response to a received character.
 *  
 *  @param data      The received character.
 */
extern void px_cli_on_rx_char(char data);

/**
 *  Handler function to call when "help" command is invoked.
 *  
 *  @param argc          Number of argument strings
 *  @param argv          Array of pointers to zero terminated argument strings
 *  
 *  @return const char * Response string to display; return NULL to display nothing
 */
extern const char* px_cli_cmd_help_fn(uint8_t argc, char * argv[]);

/**
 *  Utility function to convert an ARGV string to an option.
 *  
 *  The ARGV string is compared to a series of zero terminated strings with the
 *  end indicated by an empty string ('\0'). The index of the string that it
 *  matches is returned. 0xff is returned if no match is found.
 *  
 *  Example:
 *  
 *      @code{.c}
 *      switch(px_cli_util_argv_to_option(1, "on\0off\0")))
 *      {
 *      case 0:
 *          LED_ON();
 *          break;
 *      case 1:
 *          LED_OFF();
 *          break;
 *      default:
 *          return "Error: incorrect parameters!";
 *      }
 *      @endcode    
 *  
 *  @param argv_index Index of ARGV to convert into an option
 *  @param options    Series of zero terminated strings. End is indicated by an
 *                    empty string.
 *  
 *  @return uint8_t   Index of the string that matches ARGV string.
 *  @retval 0xff      No match was found.
 */
uint8_t px_cli_util_argv_to_option(uint8_t argv_index, const char * options);

/** 
 *  Utility function to convert an ARGV string to a number.
 *  
 *  This function is useful to parse ARGV parameters. The string is converted 
 *  into a number and it is tested if the number is within the specified 
 *  bounds (min <= val <= max).
 *   
 *  @param argv_index Index of ARGV to convert
 *  @param min        Minimum value accepted for number
 *  @param max        Maximum value accepted for number
 *   
 *  @retval true      String has been succesfully converted into a number and 
 *                    lies between the specified bounds.
 */
extern bool px_cli_util_argv_to_u8(uint8_t argv_index, uint8_t min, uint8_t max);

/** 
 *  Utility function to convert an ARGV string to a number.
 *   
 *  This function is useful to parse ARGV parameters. The string is converted 
 *  into a number and it is tested if the number is within the specified 
 *  bounds (min <= val <= max).
 *   
 *  @param argv_index Index of ARGV to convert
 *  @param min        Minimum value accepted for number
 *  @param max        Maximum value accepted for number
 *   
 *  @retval true      String has been succesfully converted into a number and 
 *                    lies between the specified bounds.
 */
extern bool px_cli_util_argv_to_u16(uint8_t argv_index, uint16_t min, uint16_t max);

/** 
 *  Utility function to convert an ARGV string to a number.
 *   
 *  This function is useful to parse ARGV parameters. The string is converted 
 *  into a number and it is tested if the number is within the specified 
 *  bounds (min <= val <= max).
 *   
 *  @param argv_index Index of ARGV to convert
 *  @param min        Minimum value accepted for number
 *  @param max        Maximum value accepted for number
 *   
 *  @retval true      String has been succesfully converted into a number and 
 *                    lies between the specified bounds.
 */
extern bool px_cli_util_argv_to_u32(uint8_t argv_index, uint32_t min, uint32_t max);

/** 
 *  Utility function to convert an ARGV string to a number.
 *   
 *  This function is useful to parse ARGV parameters. The string is converted 
 *  into a number and it is tested if the number is within the specified 
 *  bounds (min <= val <= max).
 *   
 *  @param argv_index Index of ARGV to convert
 *  @param min        Minimum value accepted for number
 *  @param max        Maximum value accepted for number
 *   
 *  @retval true      String has been succesfully converted into a number and 
 *                    lies between the specified bounds.
 */
extern bool px_cli_util_argv_to_s8(uint8_t argv_index, int8_t min, int8_t max);

/** 
 *  Utility function to convert an ARGV string to a number.
 *   
 *  This function is useful to parse ARGV parameters. The string is converted 
 *  into a number and it is tested if the number is within the specified 
 *  bounds (min <= val <= max).
 *   
 *  @param argv_index Index of ARGV to convert
 *  @param min        Minimum value accepted for number
 *  @param max        Maximum value accepted for number
 *   
 *  @retval true      String has been succesfully converted into a number and 
 *                    lies between the specified bounds.
 */
extern bool px_cli_util_argv_to_s16(uint8_t argv_index, int16_t min, int16_t max);

/** 
 *  Utility function to convert an ARGV string to a number.
 *   
 *  This function is useful to parse ARGV parameters. The string is converted 
 *  into a number and it is tested if the number is within the specified 
 *  bounds (min <= val <= max).
 *   
 *  @param argv_index Index of ARGV to convert
 *  @param min        Minimum value accepted for number
 *  @param max        Maximum value accepted for number
 *   
 *  @retval true      String has been succesfully converted into a number and 
 *                    lies between the specified bounds.
 */
extern bool px_cli_util_argv_to_s32(uint8_t argv_index, int32_t min, int32_t max);

/** 
 *  Utility function to convert an ARGV string to a number.
 *   
 *  This function is useful to parse ARGV parameters.
 *   
 *  @param argv_index Index of ARGV to convert
 *   
 *  @retval true      String has been succesfully converted into a number and 
 *                    lies between the specified bounds.
 */
extern bool px_cli_util_argv_to_float(uint8_t argv_index);

/** 
 *  Utility function to convert an ARGV string to a number.
 *   
 *  This function is useful to parse ARGV parameters.
 *   
 *  @param argv_index Index of ARGV to convert
 *   
 *  @retval true      String has been succesfully converted into a number and 
 *                    lies between the specified bounds.
 */
extern bool px_cli_util_argv_to_double(uint8_t argv_index);

/**
 *  Utility function to display the content of a buffer.
 *   
 *  This function displays the content of a buffer in HEX and ASCII. The output 
 *  is formatted in columns of 16 bytes of HEX data and adjacent 16 characters 
 *  of ASCII data. If the ASCII data is not displayable (code < 32 or code > 127) 
 *  then a dot ('.') is displayed instead.
 *   
 *  @param data          Pointer to buffer containing data to display
 *  @param nr_of_bytes   Number of bytes in buffer to display.
 */
void px_cli_util_disp_buf(const uint8_t * data, size_t nr_of_bytes);

/* _____MACROS_______________________________________________________________ */
/**
 *  Macro to create a new CLI command structure.
 *  
 *  Example:
 *  
 *      @code{.c}
 *      PX_CLI_CMD_CREATE(px_cli_cmd_help, "help", "Display list of commands", 0, 0)
 *      @endcode
 *  
 *  @param cli_cmd          Prefix used to name command structures
 *  @param name_str         String name of command
 *  @param nr_arg_min       Minimum number of valid arguments
 *  @param nr_arg_max       Maximum number of valid arguments
 *  @param param_str        Parameter string displayed when 'help' command is
 *                          executed
 *  @param help_str         Help string displayed when 'help' command is
 *                          executed
 */
#if PX_CLI_CFG_DISP_HELP_STR
#define PX_CLI_CMD_CREATE(cli_cmd, name_str, nr_arg_min, nr_arg_max, param_str, help_str) \
    static const char cli_cmd ## _name[]  PX_ATTR_PGM = name_str; \
    static const char cli_cmd ## _param[] PX_ATTR_PGM = param_str; \
    static const char cli_cmd ## _help[]  PX_ATTR_PGM = help_str; \
    const px_cli_cmd_t cli_cmd PX_ATTR_PGM = \
    { \
        .name     = cli_cmd ## _name, \
        .argc_min = nr_arg_min, \
        .argc_max = nr_arg_max, \
        .param    = cli_cmd ## _param, \
        .help     = cli_cmd ## _help, \
    };
#else
#define PX_CLI_CMD_CREATE(cli_cmd, name_str, nr_arg_min, nr_arg_max, param_str, help_str) \
    static const char cli_cmd ## _name[]  PX_ATTR_PGM = name_str; \
    static const char cli_cmd ## _param[] PX_ATTR_PGM = param_str; \
    const px_cli_cmd_t cli_cmd PX_ATTR_PGM = \
    { \
        .name     = cli_cmd ## _name, \
        .argc_min = nr_arg_min, \
        .argc_max = nr_arg_max, \
        .param    = cli_cmd ## _param, \
    };
#endif

/**
 *  Macro to create a new CLI command group structure.
 *  
 *  Example:
 *  
 *      @code{.c}
 *      PX_CLI_GROUP_CREATE(px_cli_group_i2c, "i2c")
 *      @endcode
 *  
 *  @param cli_group    Prefix name of command group structures
 *  @param name_str     String name of command
 */
#define PX_CLI_GROUP_CREATE(cli_group, name_str) \
    static const char                   cli_group ## _str[] PX_ATTR_PGM = name_str; \
    static const px_cli_cmd_list_item_t cli_group ## _list[] PX_ATTR_PGM; \
    const px_cli_group_t cli_group PX_ATTR_PGM = \
    { \
        .name = cli_group ## _str, \
        .list = cli_group ## _list, \
    }; \
    static const px_cli_cmd_list_item_t cli_group ## _list[] PX_ATTR_PGM = \
    {

/// Macro to end a command list declaration
#define PX_CLI_GROUP_END() \
        { \
            .handler = NULL, \
            .cmd     = NULL, \
        }, \
    };

/// Macro to start a command list declaration.
#define PX_CLI_CMD_LIST_CREATE() \
    const px_cli_cmd_list_item_t px_cli_cmd_list[] PX_ATTR_PGM = \
    {

/**
 *  Macro to add a created command structure to the list
 *  
 *  @param cli_cmd          Name of command structure
 *  @param handler_fn       Function (of type px_cli_handler_t) to be called
 *                          when command is executed
 *  
 */
#define PX_CLI_CMD_ADD(cli_cmd, handler_fn) \
        { \
            .handler = handler_fn, \
            .cmd     = &cli_cmd, \
        },

/**
 *  Macro to add a created command group structure to the list
 *  
 *  @param cli_group    Name of command group structure
 */
#define PX_CLI_GROUP_ADD(cli_group) \
        { \
            .handler = NULL, \
            .group   = &cli_group, \
        },

/// Macro to end a command list declaration
#define PX_CLI_CMD_LIST_END() \
        { \
            .handler = NULL, \
            .cmd     = NULL, \
        }, \
    };

/// @}
#ifdef __cplusplus
}
#endif

#endif // #ifndef __PX_CLI_H__
