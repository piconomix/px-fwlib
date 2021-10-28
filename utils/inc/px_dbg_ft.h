#ifndef __PX_DBG_FT_H__
#define __PX_DBG_FT_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2021 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          px_dbg_ft.h : Debug Flow Trace module
    Author(s):      Pieter Conradie
    Creation Date:  2021-05-31

============================================================================= */
/** 
 *  @ingroup UTILS
 *  @defgroup PX_DBG_FT px_dbg_ft.h : Debug Flow Trace module
 *  
 *  Log flow of a program to a RAM buffer for post mortem inspection.
 *  
 *  File(s):
 *  - utils/inc/px_dbg_ft.h
 *  - utils/inc/px_dbg_ft_cfg_template.h
 *  - utils/src/px_dbg_ft.c
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

// Define PX_DBG_FT for the benefit of Doxygen references
#ifdef __DOX__
    /// Set flag to disable (PX_DBG_FT=0) or enable (PX_DBG_FT=1) log output.
    #define PX_DBG_FT 1
#endif

// PX_DBG_FT symbol defined in Makefile?
#if defined(PX_DBG_FT)
    // Yes. Include project specific configuration. See "px_dbg_ft_cfg_template.h"
    #include "px_dbg_ft_cfg.h"
#else
    // No: Remove all log code.
    #define PX_DBG_FT              0
    #define PX_DBG_FT_CFG_BUF_SIZE 16
    typedef enum
    {
        PX_DBG_FT_NAME_NONE = 0,
    } px_dbg_ft_name_t;
#endif

// Check that all project specific options have been specified in "px_dbg_ft_cfg.h"
#if (    !defined(PX_DBG_FT             ) \
      || !defined(PX_DBG_FT_CFG_BUF_SIZE)  )
#error "One or more options not defined in 'px_dbg_ft_cfg.h'"
#endif

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS _________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
/// Index of next empty position; Placed in .noinit section so that it is not set to zero by C initialization code before it can be inspected
extern uint8_t px_dbg_ft_buf_index;

/// Buffer for log output; Placed in .noinit section so that it is not set to zero by C initialization code before it can be inspected
extern uint32_t px_dbg_ft_buf[PX_DBG_FT_CFG_BUF_SIZE];

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise debug flow trace module.
 */
void _px_dbg_ft_init(void);

/**
 *  Internal function to log a name and file line number.
 *
 *  @param name     Custom name value as defined in 'px_dbg_ft_cfg.h'
 *  @param line     File line number
 */
void _px_dbg_ft_log(const px_dbg_ft_name_t name, uint16_t line);

/**
 *  Internal function to log a name, file line number and parameter.
 *
 *  @param name     Custom name value as defined in 'px_dbg_ft_cfg.h'
 *  @param line     File line number
 *  @param param    Parameter
 */
void _px_dbg_ft_log_param(const px_dbg_ft_name_t name, uint16_t line, uint8_t param);

/**
 *  Report flow trace logged in buffer.
 */
void px_dbg_ft_report(void);

/* _____MACROS_______________________________________________________________ */
// PX_DBG_FT enabled?
#if PX_DBG_FT

/// Macro to declare a name value once at the top of each C file.
#define PX_DBG_FT_NAME(name)   PX_ATTR_UNUSED static const px_dbg_ft_name_t px_dbg_ft_name = name;

/// Macro to initialise debug flow trace module
#define PX_DBG_FT_INIT() \
    do \
    { \
        _px_dbg_ft_init(); \
    } \
    while(0)

/// Macro to log a name and file line number
#define PX_DBG_FT_LOG() \
    do \
    { \
        _px_dbg_ft_log(px_dbg_ft_name, (uint16_t)__LINE__); \
    } \
    while(0)

/// Macro to log a name and a specified file line number
#define PX_DBG_FT_LOG_LINE(line) \
    do \
    { \
        _px_dbg_ft_log(px_dbg_ft_name, line); \
    } \
    while(0)

/// Macro to log a name, file line number and parameter
#define PX_DBG_FT_LOG_PARAM(param) \
    do \
    { \
        _px_dbg_ft_log_param(px_dbg_ft_name, (uint16_t)__LINE__, param); \
    } \
    while(0)

/// Macro to log a name, specified file line number and parameter
#define PX_DBG_FT_LOG_LINE_PARAM(line, param) \
    do \
    { \
        _px_dbg_ft_log_param(px_dbg_ft_name, line, param); \
    } \
    while(0)

#else
    // PX_DBG_FT = 0; Remove debug flow trace code
    #define PX_DBG_FT_NAME(name)
    #define PX_DBG_FT_INIT()
    #define PX_DBG_FT_LOG()
    #define PX_DBG_FT_LOG_LINE(line)
    #define PX_DBG_FT_LOG_PARAM(param)
    #define PX_DBG_FT_LOG_LINE_PARAM(line, param)
#endif

#ifdef __cplusplus
}
#endif

/// @}
#endif
