#ifndef __PX_EXTI_H__
#define __PX_EXTI_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2020 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
 
    Title:          px_exti.h : External interrupt peripheral driver
    Author(s):      Pieter Conradie
    Creation Date:  2020-04-13

============================================================================= */

/** 
 *  @ingroup STM32
 *  @defgroup STM32_EXTI px_exti.h : External interrupt peripheral driver
 *  
 *  Driver for external interrupts.
 *  
 *  File(s):
 *  - arch/arm/stm32/px_exti.h
 *  - arch/arm/stm32/px_exti_cfg_template.h
 *  - arch/arm/stm32/px_exti.c
 *  
 *  The driver must be configured by supplying a project specific 
 *  "px_exti_cfg.h". "px_exti_cfg_template.h" can be copied, renamed and 
 *  modified to supply compile time options.
 *  
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

// Include project specific config. See "px_exti_cfg_template.h"
#include "px_exti_cfg.h"

// Check that all project specific options have been specified in "px_exti_cfg.h"
#if (   !defined(PX_EXTI_CFG_INT0_EN    ) \
     || !defined(PX_EXTI_CFG_INT1_EN    ) \
     || !defined(PX_EXTI_CFG_INT2_EN    ) \
     || !defined(PX_EXTI_CFG_INT3_EN    ) \
     || !defined(PX_EXTI_CFG_INT4_EN    ) \
     || !defined(PX_EXTI_CFG_INT5_EN    ) \
     || !defined(PX_EXTI_CFG_INT6_EN    ) \
     || !defined(PX_EXTI_CFG_INT7_EN    ) \
     || !defined(PX_EXTI_CFG_INT8_EN    ) \
     || !defined(PX_EXTI_CFG_INT9_EN    ) \
     || !defined(PX_EXTI_CFG_INT10_EN   ) \
     || !defined(PX_EXTI_CFG_INT11_EN   ) \
     || !defined(PX_EXTI_CFG_INT12_EN   ) \
     || !defined(PX_EXTI_CFG_INT13_EN   ) \
     || !defined(PX_EXTI_CFG_INT14_EN   ) \
     || !defined(PX_EXTI_CFG_INT15_EN)  )
#error "One or more options not defined in 'px_exti_cfg.h'"
#endif
#if (   (PX_EXTI_CFG_INT0_EN  > 1) \
     || (PX_EXTI_CFG_INT1_EN  > 1) \
     || (PX_EXTI_CFG_INT2_EN  > 1) \
     || (PX_EXTI_CFG_INT3_EN  > 1) \
     || (PX_EXTI_CFG_INT4_EN  > 1) \
     || (PX_EXTI_CFG_INT5_EN  > 1) \
     || (PX_EXTI_CFG_INT6_EN  > 1) \
     || (PX_EXTI_CFG_INT7_EN  > 1) \
     || (PX_EXTI_CFG_INT8_EN  > 1) \
     || (PX_EXTI_CFG_INT9_EN  > 1) \
     || (PX_EXTI_CFG_INT10_EN > 1) \
     || (PX_EXTI_CFG_INT11_EN > 1) \
     || (PX_EXTI_CFG_INT12_EN > 1) \
     || (PX_EXTI_CFG_INT13_EN > 1) \
     || (PX_EXTI_CFG_INT14_EN > 1) \
     || (PX_EXTI_CFG_INT15_EN > 1) )
#error "PX_EXTI_CFG_INTXX_EN must be 0 or 1"
#endif

#ifdef __cplusplus
extern "C"
{
#endif
/* _____DEFINITIONS__________________________________________________________ */
/// External Interupt line
typedef enum
{
    PX_EXTI_LINE_0      = 0,
    PX_EXTI_LINE_1      = 1,
    PX_EXTI_LINE_2      = 2,
    PX_EXTI_LINE_3      = 3,
    PX_EXTI_LINE_4      = 4,
    PX_EXTI_LINE_5      = 5,
    PX_EXTI_LINE_6      = 6,
    PX_EXTI_LINE_7      = 7,
    PX_EXTI_LINE_8      = 8,
    PX_EXTI_LINE_9      = 9,
    PX_EXTI_LINE_10     = 10,
    PX_EXTI_LINE_11     = 11,
    PX_EXTI_LINE_12     = 12,
    PX_EXTI_LINE_13     = 13,
    PX_EXTI_LINE_14     = 14,
    PX_EXTI_LINE_15     = 15,
} px_exti_line_t;

/// External interrupt port
typedef enum
{
    PX_EXTI_PORT_A      = 0,
    PX_EXTI_PORT_B      = 1,
    PX_EXTI_PORT_C      = 2,
    PX_EXTI_PORT_D      = 3,
    PX_EXTI_PORT_E      = 4,
    PX_EXTI_PORT_H      = 5,
} px_exti_port_t;

/// External interrupt type
typedef enum
{
    PX_EXTI_TYPE_FALLING_EDGE = 0,
    PX_EXTI_TYPE_RISING_EDGE,
} px_exti_type_t;

/* _____TYPE DEFINITIONS_____________________________________________________ */
/**
 *  Definition for a pointer to a function that will be called when the external 
 *  interrupt triggers.
 */
typedef void (*px_exti_handler_t)(void);

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise External Interrupt driver.
 */
void px_exti_init(void);

/**
 *  Configure External Interrupt
 *  
 *  @param exti_port        Port (A, B,C, D, E or H)
 *  @param exti_line        Line (0, 1, 2, ... or 15)
 *  @param exti_type        Falling or Rising Edge
 *  @param exti_handler     Handler function to be called when interrupt is triggered
 */
void px_exti_open(px_exti_port_t    exti_port,
                  px_exti_line_t    exti_line,
                  px_exti_type_t    exti_type,
                  px_exti_handler_t exti_handler);

/**
 *  Enable External Interrupt
 *  
 *  @param exti_line        Line (0, 1, 2, ... or 15)
 */
void px_exti_enable(px_exti_line_t exti_line);

/**
 *  Disable External Interrupt
 *  
 *  @param exti_line        Line (0, 1, 2, ... or 15)
 */
void px_exti_disable(px_exti_line_t exti_line);

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif // #ifndef __PX_EXTI_H__
