#ifndef __PX_DAC_H__
#define __PX_DAC_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
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
 *  - arch/arm/stm32/px_dac.h
 *  - arch/arm/stm32/px_dac_cfg_template.h
 *  - arch/arm/stm32/px_dac.c
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

// Include project specific config. See "px_dac_cfg_template.h"
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
/* _____DEFINITIONS _________________________________________________________ */
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
