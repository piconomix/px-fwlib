#ifndef __PX_DBG_H__
#define __PX_DBG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2007 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md

    Title:          px_dbg.h : Debug module
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31

============================================================================= */
/** 
 *  @ingroup UTILS
 *  @defgroup PX_DBG px_dbg.h : Debug module
 *  
 *  Outputs debug information to a serial console
 *  
 *  File(s):
 *  - utils/inc/px_dbg.h
 *  - utils/inc/px_dbg_cfg_template.h
 *  - utils/inc/px_dbg_color.h
 *  - utils/src/px_dbg.c
 *  
 *  An old school debugging technique which still works well is to add debug 
 *  print statements in the source code to track program flow and check for 
 *  coding mistakes during development. Afterwards the debug print statements 
 *  are removed from the final release build to avoid the significant code and 
 *  delay overhead.
 *  
 *  This module allows one to easily add debug print macros to the source code 
 *  and enable them by defining PX_DBG=1 or remove them from the build by 
 *  defining PX_DBG=0 in the Makefile. There is no need to manually edit the 
 *  source code afterwards for the release build. 
 *   
 *  ![Debug output on Tera Term](px_dbg_test.png) 
 *   
 *  Set #PX_DBG_CFG_COLOR to 1 to mark debug message level with color on 
 *  an ANSI/VT100 terminal emulator for example 
 *  [Tera Term](http://en.sourceforge.jp/projects/ttssh2).
 *
 *  The following macros can be used:
 *  - PX_DBG_ERR() to report an error message (RED)
 *  - PX_DBG_WARN() to report a warning message (YELLOW)
 *  - PX_DBG_INFO() to report an info message (GREEN)
 *  - PX_DBG_VERB() to report an verbose message (BLUE)
 *  - PX_DBG_ASSERT() to test an expression and halt if it is false, for example
 *    to make sure that a pointer is not NULL before using it.
 *  - PX_DBG_TRACE() to output debug information
 *  
 *  #PX_DBG_CFG_MSG_LEVEL is a bitmask that is used to select which debug 
 *  messages are compiled into the source code: PX_DBG_ERR(), PX_DBG_WARN(), 
 *  PX_DBG_INFO(), PX_DBG_VERB() or other combinations including NONE or ALL.
 *  It does not affect PX_DBG_TRACE() or PX_DBG_ASSERT() macros.
 *  
 *  The debug macros will report the file name / module name and line number as
 *  well as the user format string. To save code space, the file name / module 
 *  name must be declared at the top of the C file using PX_DBG_DECL_NAME(). 
 *  This is the correct way to declare the file name / module name:
 *  
 *      @code{.c}
 *      #include "px_dbg.h"
 *      PX_DBG_DECL_NAME("module name / file name"); // Or PX_DBG_DECL_NAME(__FILE__);
 *      @endcode
 *  
 *  @warn_s
 *  If PX_DBG_DECL_NAME() is not used, then the compiler will output the 
 *  following error when debug output is enabled (PX_DBG=1):
 *  @warn_e
 *  
 *      error: '_px_dbg_name' undeclared (first use in this function)
 *   
 *  The debug output can be redirected by using the PX_DBG_CFG_PUT_CHAR macro, 
 *  for example to stderr, a different uart or to a log file on an SD card.
 *  
 *  Example:
 *  
 *  @include utils/test/px_dbg_test.c
 *  
 *  The project wide #PX_DBG_CFG_MSG_LEVEL setting in the Makefile or 
 *  "px_dbg_cfg.h" can be overriden for a specific C file as follows:
 *  
 *      @code{.c}
 *      // PX_DBG_CFG_MSG_LEVEL defined in Makefile?
 *      #ifdef PX_DBG_CFG_MSG_LEVEL
 *      #undef PX_DBG_CFG_MSG_LEVEL
 *      #endif
 *      // Set output level to ALL (ERROR, WARN and INFO messages)
 *      #define PX_DBG_CFG_MSG_LEVEL PX_DBG_CFG_MSG_LEVEL_ALL
 *      #include "px_dbg.h"
 *      PX_DBG_DECL_NAME("buggy_module");
 *      @endcode
 *  
 *  The #PX_DBG_CFG_MSG_LEVEL can easily be tested with the the 
 *  #PX_DBG_LEVEL_ERR, #PX_DBG_LEVEL_WARN, #PX_DBG_LEVEL_INFO or
 *  #PX_DBG_LEVEL_VERB macro to add code conditionally, for example:
 *  
 *      @code{.c}
 *      void tx_data(void * data, size_t nr_of_bytes)
 *      {
 *      // PX_DBG=1 and PX_DBG_CFG_MSG_LEVEL includes INFO messages?
 *      #if PX_DBG_LEVEL_INFO
 *          // Report content of buffer
 *          PX_DBG_TRACE_HEXDUMP(data, nr_of_bytes);
 *      #endif
 *  
 *          // Rest of function...
 *      }
 *      @endcode
 *  
 *  @tip_s
 *  A naming convention is used to start a function, variable or macro name with
 *  an underscore ('_') to indicate that it is used internally by this C module
 *  and should not be used directly, for example _px_dbg_log_err() or 
 *  _px_dbg_name[].
 *  @tip_e
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_compiler.h"
#include "px_pgm_P.h"

// Define PX_DBG for the benefit of Doxygen references
#ifdef __DOX__
    /// Set flag to disable (PX_DBG=0) or enable (PX_DBG=1) debug.
    #define PX_DBG 1
#endif

// PX_DBG symbol defined in Makefile?
#if defined(PX_DBG)
    // Yes. Include project specific configuration. See "px_dbg_cfg_template.h"
    #include "px_dbg_cfg.h"
#else
    // No: Remove all debugging code.
    #define PX_DBG                     0
    #define PX_DBG_CFG_MSG_LEVEL       PX_DBG_CFG_MSG_LEVEL_NONE
    #define PX_DBG_CFG_BUF_SIZE        32
    #define PX_DBG_CFG_COLOR           0
#endif

// Check that all project specific options have been specified in "px_dbg_cfg.h"
#if (    !defined(PX_DBG              ) \
      || !defined(PX_DBG_CFG_MSG_LEVEL) \
      || !defined(PX_DBG_CFG_BUF_SIZE ) \
      || !defined(PX_DBG_CFG_COLOR    )  )
#error "One or more options not defined in 'px_dbg_cfg.h'"
#endif

// Include debug color definitions *AFTER* PX_DBG_CFG_COLOR has been defined
#include "px_dbg_color.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS _________________________________________________________ */
/// @name Debug message level bitmask definitions
//@{
/// None
#define PX_DBG_CFG_MSG_LEVEL_NONE   0
/// Errors
#define PX_DBG_CFG_MSG_LEVEL_ERR    (1 << 0)
/// Warnings
#define PX_DBG_CFG_MSG_LEVEL_WARN   (1 << 1)
/// Info
#define PX_DBG_CFG_MSG_LEVEL_INFO   (1 << 2)
/// Verbose
#define PX_DBG_CFG_MSG_LEVEL_VERB   (1 << 3)

/// All (does not include VERBOSE)
#define PX_DBG_CFG_MSG_LEVEL_ALL    (PX_DBG_CFG_MSG_LEVEL_ERR | PX_DBG_CFG_MSG_LEVEL_WARN | PX_DBG_CFG_MSG_LEVEL_INFO)
//@}

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Output error debug information: module name, line and variable argument
 *  user format string.
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
void _px_dbg_log_err(const char * name,
                     uint16_t     line,
                     const char * format, ...);

/**
 *  Output warning debug information: module name, line and variable argument
 *  user format string.
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
void _px_dbg_log_warn(const char * name,
                      uint16_t     line,
                      const char * format, ...);

/**
 *  Output info debug information: module name, line and variable argument
 *  user format string.
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
void _px_dbg_log_info(const char * name,
                      uint16_t     line,
                      const char * format, ...);

/**
 *  Output info debug information: module name, line and variable argument
 *  user format string.
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
void _px_dbg_log_verb(const char * name,
                      uint16_t     line,
                      const char * format, ...);

/**
 *  Report that an assertion has failed and block indefinitely.
 *  
 *  For supported architectures it will also generate a debug breakpoint.
 *  
 *  @param name         Module / file name
 *  @param line         Line number 
 *  @param expression   Test expression string for assertion
 */
void _px_dbg_assert(const char * name, 
                    uint16_t     line, 
                    const char * expression) PX_ATTR_NORETURN;

/**
 *  Output debug information: variable argument user format string.
 *  
 *  This function does not automatically append a new line character ('\\n') so
 *  that multiple strings can be sent on the same line.
 *  
 *  @param format   User format string 
 *  @param ...      Variable number of arguments 
 */
void _px_dbg_trace(const char * format, ...);

/**
 *  Output debug information: content of a buffer as an array of HEX values.
 *  
 *  @param data         Pointer to buffer containing data to display
 *  @param nr_of_bytes  Size of buffer (in bytes)
 */
void _px_dbg_trace_data(const void * data, size_t nr_of_bytes);

/**
 *  Output debug information: content of a buffer as a spaced table of HEX 
 *  values and ASCII characters.
 *  
 *  @param data         Pointer to buffer containing data to display
 *  @param nr_of_bytes  Size of buffer (in bytes)
 */
void _px_dbg_trace_hexdump(const void * data, size_t nr_of_bytes);

/* _____MACROS_______________________________________________________________ */
/// Debug message level ERROR enabled?
#define PX_DBG_LEVEL_ERR  (PX_DBG && ((PX_DBG_CFG_MSG_LEVEL) & PX_DBG_CFG_MSG_LEVEL_ERR))
/// Debug message level WARN enabled?
#define PX_DBG_LEVEL_WARN (PX_DBG && ((PX_DBG_CFG_MSG_LEVEL) & PX_DBG_CFG_MSG_LEVEL_WARN))
/// Debug message level INFO enabled?
#define PX_DBG_LEVEL_INFO (PX_DBG && ((PX_DBG_CFG_MSG_LEVEL) & PX_DBG_CFG_MSG_LEVEL_INFO))
/// Debug message level VERBOSE enabled?
#define PX_DBG_LEVEL_VERB (PX_DBG && ((PX_DBG_CFG_MSG_LEVEL) & PX_DBG_CFG_MSG_LEVEL_VERB))

// PX_DBG enabled?
#if PX_DBG

#if PX_DBG_LEVEL_ERR
/// Macro to display a formatted ERROR message
#define PX_DBG_ERR(format, ...)    _px_dbg_log_err(_px_dbg_name, (uint16_t)__LINE__, PX_PGM_STR(format), ## __VA_ARGS__)
#else
#define PX_DBG_ERR(format, ...)    ((void)0)
#endif

#if PX_DBG_LEVEL_WARN
/// Macro to display a formatted WARNING message
#define PX_DBG_WARN(format, ...)   _px_dbg_log_warn(_px_dbg_name, (uint16_t)__LINE__, PX_PGM_STR(format), ## __VA_ARGS__)
#else
#define PX_DBG_WARN(format, ...)   ((void)0)
#endif

#if PX_DBG_LEVEL_INFO
/// Macro to display a formatted INFO message
#define PX_DBG_INFO(format, ...)   _px_dbg_log_info(_px_dbg_name, (uint16_t)__LINE__, PX_PGM_STR(format), ## __VA_ARGS__)
#else
#define PX_DBG_INFO(format, ...)   ((void)0)
#endif

#if PX_DBG_LEVEL_VERB
/// Macro to display a formatted INFO message
#define PX_DBG_VERB(format, ...)   _px_dbg_log_verb(_px_dbg_name, (uint16_t)__LINE__, PX_PGM_STR(format), ## __VA_ARGS__)
#else
#define PX_DBG_VERB(format, ...)   ((void)0)
#endif

// Must module name be declared?
#if (PX_DBG_CFG_MSG_LEVEL != PX_DBG_CFG_MSG_LEVEL_NONE)
/**
 *  Macro to declare a debug module name string once for each file to reduce 
 *  code size.
 */
#define PX_DBG_DECL_NAME(name) \
            PX_ATTR_UNUSED static const char _px_dbg_name[] PX_ATTR_PGM = name;
#else
#define PX_DBG_DECL_NAME(name)
#endif

/**
 *  Macro to output debug information if #PX_DBG = 1
 *  
 *  This macro does not append a new line character ('\\n') so that multiple
 *  strings can be sent on the same line.
 *  
 *  @param[in] format   Format string following by a variable list of arguments.
 *  
 */
#define PX_DBG_TRACE(format, ...) _px_dbg_trace(PX_PGM_STR(format), ## __VA_ARGS__)

/**
 *  Macro to output the content of a buffer if #PX_DBG = 1
 *  
 *  The data is displayed as an array of HEX values.
 *  
 *  @param data         Pointer to buffer containing data to display
 *  @param nr_of_bytes  Size of buffer (in bytes)
 *  
 */
#define PX_DBG_TRACE_DATA(data, nr_of_bytes) _px_dbg_trace_data(data, nr_of_bytes)

/**
 *  Macro to output the content of a buffer if #PX_DBG = 1
 *  
 *  The data is displayed as a spaced table of HEX values and ASCII characters.
 *  
 *  @param data         Pointer to buffer containing data to display
 *  @param nr_of_bytes  Size of buffer (in bytes)
 *  
 */
#define PX_DBG_TRACE_HEXDUMP(data, nr_of_bytes) _px_dbg_trace_hexdump(data, nr_of_bytes)

/**
 *  Macro that will test an expression, and block indefinitely if false.
 *  
 *  This macro will perform the test and if false, will output the filename and
 *  line number with the test appended. The macro will then block indefinitely.
 *  
 *  For supported architectures, it will generate a debug breakpoint.
 *  
 *  @param[in] expression   Expression that evaluates to a boolean value
 *                          (true or false)
 */
#define PX_DBG_ASSERT(expression) \
            do \
            { \
                if(!(expression)) \
                { \
                    _px_dbg_assert(_px_dbg_name, (uint16_t)__LINE__, PX_PGM_STR(#expression)); \
                } \
            } while(0)
#else

// PX_DBG = 0; Remove debug reporting code
#define PX_DBG_ERR(format, ...)                    ((void)0)
#define PX_DBG_WARN(format, ...)                   ((void)0)
#define PX_DBG_INFO(format, ...)                   ((void)0)
#define PX_DBG_VERB(format, ...)                   ((void)0)
#define PX_DBG_DECL_NAME(name)
#define PX_DBG_TRACE(format, ...)                  ((void)0)
#define PX_DBG_TRACE_DATA(data, nr_of_bytes)       ((void)0)
#define PX_DBG_TRACE_HEXDUMP(data, nr_of_bytes)    ((void)0)
#define PX_DBG_ASSERT(expression)                  ((void)0)
#define PX_DBG_LOG(level, format, ...)             ((void)0)

#endif

/// @}
#ifdef __cplusplus
}
#endif

#endif // #ifndef __PX_DBG_H__
