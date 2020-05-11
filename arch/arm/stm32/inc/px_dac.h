#ifndef __PX_DAC_H__
#define __PX_DAC_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
 
    Title:          px_dac.h : DAC peripheral driver
    Author(s):      Pieter Conradie
    Creation Date:  2018-11-15

============================================================================= */

/** 
 *  @ingroup STM32
 *  @defgroup STM32_DAC px_dac.h : DAC peripheral driver
 *  
 *  Driver to output analogue values.
 *  
 *  File(s):
 *  - arch/arm/stm32/inc/px_dac.h
 *  - arch/arm/stm32/inc/px_dac_cfg_template.h
 *  - arch/arm/stm32/src/px_dac.c
 *  
 *  The driver must be configured by supplying a project specific "px_dac_cfg.h".
 *  "px_dac_cfg_template.h" can be copied, renamed and modified to supply 
 *  compile time options.
 *  
 *  @par Example:
 *  @include arch/arm/stm32/test/px_dac_test.c
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

// Include project specific configuration. See "px_dac_cfg_template.h"
#include "px_dac_cfg.h"

// Check that all project specific options have been specified in "px_dac_cfg.h"
#if (   !defined(PX_DAC_CFG_DAC1_EN    ) \
     || !defined(PX_DAC_CFG_DAC1_CH1_EN) \
     || !defined(PX_DAC_CFG_DAC1_CH2_EN)  )
#error "One or more options not defined in 'px_dac_cfg.h'"
#endif
#if (  (PX_DAC_CFG_DAC1_EN     > 1) \
     ||(PX_DAC_CFG_DAC1_CH1_EN > 1) \
     ||(PX_DAC_CFG_DAC1_CH2_EN > 1)  )
#error "PX_DAC_CFG_DACx_EN must be 0 or 1"
#endif

/// Number of enabled peripherals
#define PX_DAC_CFG_PER_COUNT    (PX_DAC_CFG_DAC1_EN)
#if (PX_DAC_CFG_PER_COUNT == 0)
#error "No peripherals enabled"
#endif

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
/// Specify DAC peripheral
typedef enum
{
    PX_DAC_PER_1 = 1,
} px_dac_per_t;

/// Specify DAC channel
typedef enum
{
    PX_DAC_CHANNEL_1       = 0,
    PX_DAC_CHANNEL_2       = 1,
} px_dac_channel_t;

/// Define DAC handle
typedef struct
{
    struct px_dac_data_s * dac_data;        ///< DAC peripheral data
} px_dac_handle_t;

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise DAC driver.
 */
void px_dac_init(void);

/** 
 *  Open DAC peripheral using predefined (default) parameters.
 *  
 *  @param handle       Pointer to handle data structure
 *  @param peripheral   DAC peripheral
 *  
 *  @retval false       Error: peripheral was not opened
 *  @retval true        Success: peripheral was opened
 */
bool px_dac_open(px_dac_handle_t * handle,
                 px_dac_per_t      peripheral);

/**
 *  Close specified device.
 *  
 *  @param handle       Pointer to handle data structure
 *  
 *  @retval true        Success 
 *  @retval false       Specified device was already closed (or not opened)
 */
bool px_dac_close(px_dac_handle_t * handle);

/**
 *  Perform a single output on the specified DAC channel
 *  
 *  @param handle       Pointer to handle data structure
 *  @param channel      DAC channel (1 or 2) 
 *  @param data         Data value to output
 *  
 */
void px_dac_wr(px_dac_handle_t * handle, 
               px_dac_channel_t  channel,
               uint16_t          data);

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif // #ifndef __PX_DAC_H__
