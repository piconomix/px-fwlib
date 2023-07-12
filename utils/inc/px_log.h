#ifndef __PX_LOG_H__
#define __PX_LOG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2007 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          px_log.h : Debug log module
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31

============================================================================= */
/** 
 *  @ingroup UTILS
 *  @defgroup PX_LOG px_log.h : Debug log module
 *  
 *  Output human readable debug info to a serial console.
 *  
 *  File(s):
 *  - utils/inc/px_log.h
 *  - utils/inc/px_log_cfg_template.h
 *  - utils/inc/px_log_color.h
 *  - utils/src/px_log.c
 *  
 *  An old school debugging technique which still works well is to add debug 
 *  print statements in the source code to track program flow and check for
 *  coding mistakes during development. Afterwards the debug print statements
 *  are removed from the final release build to avoid the significant code and 
 *  delay overhead.
 *  
 *  This module allows one to easily add formatted debug print macros to the
 *  source code and enable them by defining PX_LOG=1 or remove them from the
 *  build by defining PX_LOG=0 in the Makefile. There is no need to manually
 *  edit the source code afterwards for the release build.
 *   
 *  ![Debug output on Tera Term](px_log_test.png)
 *
 *  The following macros can be used:
 *  - PX_LOG_E() to report an ERROR message (RED)
 *  - PX_LOG_W() to report a WARNING message (YELLOW)
 *  - PX_LOG_I() to report an INFO message (GREEN)
 *  - PX_LOG_D() to report a DEBUG message (BLUE)
 *  - PX_LOG_V() to report a VERBOSE message (CYAN)
 *  - PX_LOG_ASSERT() to test an expression and halt if it is false, for example
 *    to make sure that a pointer is not NULL before using it.
 *  - PX_LOG_TRACE() to output a formatted string
 *  
 *  #PX_LOG_CFG_LEVEL is used to specify which log messages are compiled into
 *  the source code. For example if #PX_LOG_CFG_LEVEL is set to PX_LOG_LEVEL_INFO
 *  then all ERROR, WARNING and INFO messages will be included in the code but
 *  VERBOSE and DEBUG message will not. It does not affect PX_LOG_TRACE() or
 *  PX_LOG_ASSERT() macros.
 *
 *  Set #PX_LOG_CFG_COLOR to 1 to indicate log message level with color on
 *  an ANSI/VT100 terminal emulator (for example
 *  [Tera Term](http://en.sourceforge.jp/projects/ttssh2)).
 *
 *  Example usage:
 *
 *  @code{.c}
 *      PX_LOG_I("This word is " PX_LOG_FG_GREEN "green" PX_LOG_CR);
 *  @endcode
 *
 *  @tip_s
 *  Observe that the compiler will automatically concatenate adjacent strings.
 *  @tip_e
 *  
 *  The log messages will report the file / module name and line number as
 *  well as the user format string. To save code space, the name must be
 *  declared at the top of the C file using PX_LOG_NAME(). This is the correct
 *  way to declare the name:
 *  
 *  @code{.c}
 *      #include "px_log.h"
 *      PX_LOG_NAME("module / file name"); // Or PX_LOG_NAME(__FILE__);
 *  @endcode
 *  
 *  @warn_s
 *  If PX_LOG_NAME() is not used, then the compiler will output the
 *  following error when log output is enabled (PX_LOG=1):
 *  @warn_e
 *  
 *      error: '_px_log_name' undeclared (first use in this function)
 *   
 *  The log output can be redirected by using the PX_LOG_CFG_PUTCHAR macro,
 *  for example to stderr stream, a different uart or to a log file on an SD
 *  card.
 *  
 *  Example:
 *  
 *  @include utils/test/px_log_test.c
 *  
 *  The project wide #PX_LOG_CFG_LEVEL setting in the Makefile or
 *  "px_log_cfg.h" can be overriden for a specific C file as follows:
 *  
 *  @code{.c}
 *      // PX_LOG_CFG_LEVEL defined in Makefile?
 *      #ifdef PX_LOG_CFG_LEVEL
 *      #undef PX_LOG_CFG_LEVEL
 *      #endif
 *      // Set output level to DEBUG (ERROR, WARN, INFO and DEBUG messages)
 *      #define PX_LOG_CFG_LEVEL PX_LOG_LEVEL_DEBUG
 *      #include "px_log.h"
 *      PX_LOG_NAME("buggy_module");
 *  @endcode
 *  
 *  The #PX_LOG_CFG_LEVEL can easily be tested with the the
 *  #PX_LOG_LEVEL_IS_E(), #PX_LOG_LEVEL_IS_W(), #PX_LOG_LEVEL_IS_I(),
 *  #PX_LOG_LEVEL_IS_V() or #PX_LOG_LEVEL_IS_D() macro to conditionally add
 *  code, for example:
 *  
 *  @code{.c}
 *      void tx_data(void * data, size_t nr_of_bytes)
 *      {
 *          // PX_LOG=1 and PX_LOG_CFG_LEVEL includes VERBOSE messages?
 *          if(PX_LOG_LEVEL_IS_V())
 *          {
 *              // Report content of buffer
 *              PX_LOG_TRACE_HEXDUMP(data, nr_of_bytes);
 *          }
 *          // Rest of function...
 *      }
 *  @endcode
 *  
 *  @tip_s
 *  A naming convention is used to start a function, variable or macro name with
 *  an underscore ('_') to indicate that it is used internally by this C module
 *  and should not be used directly, for example _px_log_log_error() or
 *  _px_log_name[].
 *  @tip_e
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"
#include "px_compiler.h"
#include "px_pgm_P.h"

// Define PX_LOG for the benefit of Doxygen references
#ifdef __DOX__
    /// Set flag to disable (PX_LOG=0) or enable (PX_LOG=1) log output.
    #define PX_LOG 1
#endif

// PX_LOG symbol defined in Makefile?
#if defined(PX_LOG)
    // Yes. Include project specific configuration. See "px_log_cfg_template.h"
    #include "px_log_cfg.h"
#else
    // No: Remove all log code.
    #define PX_LOG              0
    #define PX_LOG_CFG_LEVEL    PX_LOG_LEVEL_NONE
    #define PX_LOG_CFG_FILTER   0
    #define PX_LOG_CFG_BUF_SIZE 64
    #define PX_LOG_CFG_COLOR    0
#endif

// Check that all project specific options have been specified in "px_log_cfg.h"
#if (    !defined(PX_LOG             ) \
      || !defined(PX_LOG_CFG_LEVEL   ) \
      || !defined(PX_LOG_CFG_FILTER  ) \
      || !defined(PX_LOG_CFG_COLOR   ) \
      || !defined(PX_LOG_CFG_BUF_SIZE)  )
#error "One or more options not defined in 'px_log_cfg.h'"
#endif

// Include log color definitions *AFTER* PX_LOG_CFG_COLOR has been defined
#include "px_log_color.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS _________________________________________________________ */
/// Log message level ordered in increasing verbosity
typedef enum
{
    PX_LOG_LEVEL_NONE    = 0,   ///< No log output
    PX_LOG_LEVEL_ERROR   = 1,   ///< Critical errors
    PX_LOG_LEVEL_WARNING = 2,   ///< Warnings
    PX_LOG_LEVEL_INFO    = 3,   ///< Concise info messages to track the normal flow of events
    PX_LOG_LEVEL_DEBUG   = 4,   ///< Extra info for debugging that's not required for normal use
    PX_LOG_LEVEL_VERBOSE = 5,   ///< Extra details for debugging which can flood normal output
} px_log_level_t;

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Run time log filter.
 *
 *  A customised version of this function can be declared to filter log output
 *  based on the level and / or name. Define PX_LOG_CFG_FILTER=1 to enable.
 *
 *  A weak version is declared in px_log.c that always returns `false` meaning
 *  that all log output is allowed (none is filtered).
 *
 *  @param level    Log level
 *  @param name     Module / file name
 *
 *  @retval true    Log must be filtered
 *  @retval false   Log must be outputted
 */
bool px_log_filter(px_log_level_t level, const char * name);

/**
 *  Output error info: module name, line and variable argument user format string.
 *
 *  The line is automatically appended with a new line character ('\\n'), except
 *  if the format string ends with a tab character ('\\t'). The tab character at
 *  the end of the string is discarded.
 *
 *  @param name     Module / file name
 *  @param line     Line number
 *  @param format   User format string
 *  @param ...      Variable number of arguments
 */
void _px_log_log_error(const char * name,
                       uint16_t     line,
                       const char * format, ...) PX_ATTR_FORMAT(printf, 3, 4);

/**
 *  Output warning info: module name, line and variable argument user format string.
 *
 *  The line is automatically appended with a new line character ('\\n'), except
 *  if the format string ends with a tab character ('\\t'). The tab character at
 *  the end of the string is discarded.
 *
 *  @param name     Module / file name
 *  @param line     Line number
 *  @param format   User format string
 *  @param ...      Variable number of arguments
 */
void _px_log_log_warning(const char * name,
                         uint16_t     line,
                         const char * format, ...) PX_ATTR_FORMAT(printf, 3, 4);

/**
 *  Output info info: module name, line and variable argument user format string.
 *
 *  The line is automatically appended with a new line character ('\\n'), except
 *  if the format string ends with a tab character ('\\t'). The tab character at
 *  the end of the string is discarded.
 *
 *
 *  @param name     Module / file name
 *  @param line     Line number
 *  @param format   User format string
 *  @param ...      Variable number of arguments
 */
void _px_log_log_info(const char * name,
                      uint16_t     line,
                      const char * format, ...) PX_ATTR_FORMAT(printf, 3, 4);

/**
 *  Output debug info: module name, line and variable argument user format string.
 *
 *  The line is automatically appended with a new line character ('\\n'), except
 *  if the format string ends with a tab character ('\\t'). The tab character at
 *  the end of the string is discarded.
 *
 *
 *  @param name     Module / file name
 *  @param line     Line number
 *  @param format   User format string
 *  @param ...      Variable number of arguments
 */
void _px_log_log_debug(const char * name,
                       uint16_t     line,
                       const char * format, ...) PX_ATTR_FORMAT(printf, 3, 4);

/**
 *  Output verbose info: module name, line and variable argument user format string.
 *
 *  The line is automatically appended with a new line character ('\\n'), except
 *  if the format string ends with a tab character ('\\t'). The tab character at
 *  the end of the string is discarded.
 *
 *
 *  @param name     Module / file name
 *  @param line     Line number
 *  @param format   User format string
 *  @param ...      Variable number of arguments
 */
void _px_log_log_verbose(const char * name,
                         uint16_t     line,
                         const char * format, ...) PX_ATTR_FORMAT(printf, 3, 4);

/**
 *  Report that an assertion has failed and block indefinitely.
 *
 *  @param name         Module / file name
 *  @param line         Line number
 */
void _px_log_assert(const char * name,
                    uint16_t     line) PX_ATTR_NORETURN;

/**
 *  Output a user format string.
 *  
 *  This function does not automatically append a new line character ('\\n') so
 *  that multiple strings can be sent on the same line.
 *  
 *  @param format   User format string 
 *  @param ...      Variable number of arguments 
 */
void _px_log_trace(const char * format, ...) PX_ATTR_FORMAT(printf, 1, 2);

/**
 *  Output a character.
 *
 *  @param c        Character to output
 */
void _px_log_trace_char(char c);

/**
 *  Output the content of a buffer as an array of HEX values.
 *  
 *  @param data         Pointer to buffer containing data to display
 *  @param nr_of_bytes  Number of bytes in buffer to display
 */
void _px_log_trace_buf_hex(const void * data, size_t nr_of_bytes);

/**
 *  Output the content of a buffer as a spaced table of HEX values and ASCII
 *  characters.
 *  
 *  @param data         Pointer to buffer containing data to display
 *  @param nr_of_bytes  Number of bytes in buffer to display
 */
void _px_log_trace_hexdump(const void * data, size_t nr_of_bytes);

/**
 *  Get access to internal character buffer.
 *
 *  The size of the buffer is #PX_LOG_CFG_BUF_SIZE
 */
char * _px_log_buf_get(void);

/* _____MACROS_______________________________________________________________ */
// PX_LOG enabled?
#if PX_LOG

/// Macro to declare a log name string once for each file to reduce code size.
#define PX_LOG_NAME(name)   PX_ATTR_UNUSED static const char _px_log_name[] PX_ATTR_PGM = name;

// Run time filter enabled?
#if PX_LOG_CFG_FILTER
    /// Error level enabled?
    #define PX_LOG_LEVEL_IS_E() ((PX_LOG_CFG_LEVEL >= PX_LOG_LEVEL_ERROR)   && !px_log_filter(PX_LOG_LEVEL_ERROR,   _px_log_name))
    /// Warning level enabled?
    #define PX_LOG_LEVEL_IS_W() ((PX_LOG_CFG_LEVEL >= PX_LOG_LEVEL_WARNING) && !px_log_filter(PX_LOG_LEVEL_WARNING, _px_log_name))
    /// Info level enabled?
    #define PX_LOG_LEVEL_IS_I() ((PX_LOG_CFG_LEVEL >= PX_LOG_LEVEL_INFO)    && !px_log_filter(PX_LOG_LEVEL_INFO,    _px_log_name))
    /// Debug level enabled?
    #define PX_LOG_LEVEL_IS_D() ((PX_LOG_CFG_LEVEL >= PX_LOG_LEVEL_DEBUG)   && !px_log_filter(PX_LOG_LEVEL_DEBUG,   _px_log_name))
    /// Verbose level enabled?
    #define PX_LOG_LEVEL_IS_V() ((PX_LOG_CFG_LEVEL >= PX_LOG_LEVEL_VERBOSE) && !px_log_filter(PX_LOG_LEVEL_VERBOSE, _px_log_name))
#else
    /// Error level enabled?
    #define PX_LOG_LEVEL_IS_E() (PX_LOG_CFG_LEVEL >= PX_LOG_LEVEL_ERROR)
    /// Warning level enabled?
    #define PX_LOG_LEVEL_IS_W() (PX_LOG_CFG_LEVEL >= PX_LOG_LEVEL_WARNING)
    /// Info level enabled?
    #define PX_LOG_LEVEL_IS_I() (PX_LOG_CFG_LEVEL >= PX_LOG_LEVEL_INFO)
    /// Debug level enabled?
    #define PX_LOG_LEVEL_IS_D() (PX_LOG_CFG_LEVEL >= PX_LOG_LEVEL_DEBUG)
    /// Verbose level enabled?
    #define PX_LOG_LEVEL_IS_V() (PX_LOG_CFG_LEVEL >= PX_LOG_LEVEL_VERBOSE)
#endif

/// Macro to display a formatted ERROR message
#define PX_LOG_E(format, ...) \
    do \
    { \
        if(PX_LOG_CFG_LEVEL >= PX_LOG_LEVEL_ERROR) \
        { \
            _px_log_log_error(_px_log_name, (uint16_t)__LINE__, PX_PGM_STR(format), ## __VA_ARGS__); \
        } \
    } \
    while(0)

/// Macro to display a formatted WARNING message
#define PX_LOG_W(format, ...) \
    do \
    { \
        if(PX_LOG_CFG_LEVEL >= PX_LOG_LEVEL_WARNING) \
        { \
            _px_log_log_warning(_px_log_name, (uint16_t)__LINE__, PX_PGM_STR(format), ## __VA_ARGS__); \
        } \
    } \
    while(0)

/// Macro to display a formatted INFO message
#define PX_LOG_I(format, ...) \
    do \
    { \
        if(PX_LOG_CFG_LEVEL >= PX_LOG_LEVEL_INFO) \
        { \
            _px_log_log_info(_px_log_name, (uint16_t)__LINE__, PX_PGM_STR(format), ## __VA_ARGS__); \
        } \
    } \
    while(0)

/// Macro to display a formatted DEBUG message
#define PX_LOG_D(format, ...) \
    do \
    { \
        if(PX_LOG_CFG_LEVEL >= PX_LOG_LEVEL_DEBUG) \
        { \
            _px_log_log_debug(_px_log_name, (uint16_t)__LINE__, PX_PGM_STR(format), ## __VA_ARGS__); \
        } \
    } \
    while(0)

/// Macro to display a formatted VERBOSE message
#define PX_LOG_V(format, ...) \
    do \
    { \
        if(PX_LOG_CFG_LEVEL >= PX_LOG_LEVEL_VERBOSE) \
        { \
            _px_log_log_verbose(_px_log_name, (uint16_t)__LINE__, PX_PGM_STR(format), ## __VA_ARGS__); \
        } \
    } \
    while(0)

/**
 *  Macro that will test an expression, and block indefinitely if false.
 *
 *  This macro will perform the test and if false, will output the filename and
 *  line number. The macro will then block indefinitely.
 *
 *  @param[in] expression   Expression that evaluates to a boolean value
 *                          (true or false)
 */
#define PX_LOG_ASSERT(expression) \
    do \
    { \
        if(!(expression)) \
        { \
            _px_log_assert(_px_log_name, (uint16_t)__LINE__); \
        } \
    } while(0)

/**
 *  Macro to output a user format string if #PX_LOG=1
 *  
 *  This macro does not append a new line character ('\\n') so that multiple
 *  strings can be sent on the same line.
 *  
 *  @param[in] format   Format string following by a variable list of arguments.
 *  
 */
#define PX_LOG_TRACE(format, ...) _px_log_trace(PX_PGM_STR(format), ## __VA_ARGS__)

/**
 *  Macro to output a char if #PX_LOG=1
 *
 *  @param c            Character to output
 */
#define PX_LOG_TRACE_CHAR(c) _px_log_trace_char(c)

/**
 *  Macro to output the content of a buffer if #PX_LOG=1
 *  
 *  The data is displayed as an array of HEX values.
 *  
 *  @param data         Pointer to buffer containing data to display
 *  @param nr_of_bytes  Size of buffer (in bytes)
 *  
 */
#define PX_LOG_TRACE_BUF_HEX(data, nr_of_bytes) _px_log_trace_buf_hex(data, nr_of_bytes)

/**
 *  Macro to output the content of a buffer if #PX_LOG=1
 *  
 *  The data is displayed as a spaced table of HEX values and ASCII characters.
 *  
 *  @param data         Pointer to buffer containing data to display
 *  @param nr_of_bytes  Size of buffer (in bytes)
 *  
 */
#define PX_LOG_TRACE_HEXDUMP(data, nr_of_bytes) _px_log_trace_hexdump(data, nr_of_bytes)

#else
    // PX_LOG = 0; Remove log code
    #define PX_LOG_NAME(name)
    #define PX_LOG_LEVEL_IS_E()                     0
    #define PX_LOG_LEVEL_IS_W()                     0
    #define PX_LOG_LEVEL_IS_I()                     0
    #define PX_LOG_LEVEL_IS_D()                     0
    #define PX_LOG_LEVEL_IS_V()                     0
    #define PX_LOG_E(format, ...)                   ((void)0)
    #define PX_LOG_W(format, ...)                   ((void)0)
    #define PX_LOG_I(format, ...)                   ((void)0)
    #define PX_LOG_D(format, ...)                   ((void)0)
    #define PX_LOG_V(format, ...)                   ((void)0)
    #define PX_LOG_ASSERT(expression)               ((void)0)
    #define PX_LOG_TRACE(format, ...)               ((void)0)
    #define PX_LOG_TRACE_CHAR(c)                    ((void)0)
    #define PX_LOG_TRACE_BUF_HEX(data, nr_of_bytes) ((void)0)
    #define PX_LOG_TRACE_HEXDUMP(data, nr_of_bytes) ((void)0)
#endif

#ifdef __cplusplus
}
#endif

/// @}
#endif
