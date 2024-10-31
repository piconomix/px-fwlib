#ifndef __PX_LOG_FT_H__
#define __PX_LOG_FT_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2021 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          px_log_ft.h : Flow Trace logging module
    Author(s):      Pieter Conradie
    Creation Date:  2021-05-31

============================================================================= */
/** 
 *  @ingroup UTILS
 *  @defgroup PX_LOG_FT px_log_ft.h : Flow Trace logging module
 *  
 *  Log flow of a program to a persistent RAM buffer for post mortem inspection.
 *  
 *  File(s):
 *  - utils/inc/px_log_ft.h
 *  - utils/inc/px_log_ft_cfg_template.h
 *  - utils/src/px_log_ft.c
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"

// Define PX_LOG_FT for the benefit of Doxygen references
#ifdef __DOX__
    /// Set flag to disable (PX_LOG_FT=0) or enable (PX_LOG_FT=1) logging.
    #define PX_LOG_FT 1
#endif

// PX_LOG_FT symbol defined in Makefile?
#if defined(PX_LOG_FT)
    // Yes. Include project specific configuration. See "px_log_ft_cfg_template.h"
    #include "px_log_ft_cfg.h"
#else
    // No: Remove all log code.
    #define PX_LOG_FT              0
    #define PX_LOG_FT_CFG_BUF_SIZE 16
    typedef enum
    {
        PX_LOG_FT_NAME_NONE = 0,
    } px_log_ft_name_t;
#endif

// Check that all project specific options have been specified in "px_log_ft_cfg.h"
#if (    !defined(PX_LOG_FT             ) \
      || !defined(PX_LOG_FT_CFG_BUF_SIZE)  )
#error "One or more options not defined in 'px_log_ft_cfg.h'"
#endif

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS _________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise flow trace logging module.
 */
void _px_log_ft_init(void);

/**
 *  Internal function to log a name and file line number.
 *
 *  @param name     Custom name value as defined in 'px_log_ft_cfg.h'
 *  @param line     File line number
 */
void _px_log_ft_tag(const px_log_ft_name_t name, uint16_t line);

/**
 *  Internal function to log a name, file line number and parameter.
 *
 *  @param name     Custom name value as defined in 'px_log_ft_cfg.h'
 *  @param line     File line number
 *  @param param    Parameter
 */
void _px_log_ft_tag_param(const px_log_ft_name_t name, uint16_t line, uint8_t param);

/**
 *  Report flow trace log.
 */
void px_log_ft_report(void);

/* _____MACROS_______________________________________________________________ */
// PX_LOG_FT enabled?
#if PX_LOG_FT

/// Macro to declare a name value once at the top of each C file.
#define PX_LOG_FT_NAME(name)   PX_ATTR_UNUSED static const px_log_ft_name_t px_log_ft_name = name;

/// Macro to initialise debug flow trace module
#define PX_LOG_FT_INIT() \
    do \
    { \
        _px_log_ft_init(); \
    } \
    while(0)

/// Macro to log a name and file line number
#define PX_LOG_FT_TAG() \
    do \
    { \
        _px_log_ft_tag(px_log_ft_name, (uint16_t)__LINE__); \
    } \
    while(0)

/// Macro to log a name and a specified file line number
#define PX_LOG_FT_TAG_LINE(line) \
    do \
    { \
        _px_log_ft_tag(px_log_ft_name, line); \
    } \
    while(0)

/// Macro to log a name, file line number and parameter
#define PX_LOG_FT_TAG_PARAM(param) \
    do \
    { \
        _px_log_ft_tag_param(px_log_ft_name, (uint16_t)__LINE__, param); \
    } \
    while(0)

/// Macro to log a name, specified file line number and parameter
#define PX_LOG_FT_TAG_LINE_PARAM(line, param) \
    do \
    { \
        _px_log_ft_tag_param(px_log_ft_name, line, param); \
    } \
    while(0)

#else
    // PX_LOG_FT = 0; Remove debug flow trace code
    #define PX_LOG_FT_NAME(name)
    #define PX_LOG_FT_INIT()
    #define PX_LOG_FT_TAG()
    #define PX_LOG_FT_TAG_LINE(line)
    #define PX_LOG_FT_TAG_PARAM(param)
    #define PX_LOG_FT_TAG_LINE_PARAM(line, param)
#endif

#ifdef __cplusplus
}
#endif

/// @}
#endif
