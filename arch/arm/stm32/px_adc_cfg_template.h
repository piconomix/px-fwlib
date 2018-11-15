#ifndef __PX_ADC_CFG_H__
#define __PX_ADC_CFG_H__
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
    
    Title:          px_adc_cfg.h : ADC Peripheral Driver configuration
    Author(s):      Pieter Conradie
    Creation Date:  2018-11-04

============================================================================= */

/** 
 *  @addtogroup STM32_ADC
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

/* _____DEFINITIONS__________________________________________________________ */
/// Enable/disable support for ADC1 peripheral
#define PX_ADC_CFG_ADC1_EN              1

/// Specify ADC clock source (see 'px_adc_cfg_clk_t')
#define PX_ADC_CFG_CLK                  PX_ADC_CFG_CLK_PCLK

/// Specify ADC sampling time (see 'px_adc_cfg_sample_time_t')
#define PX_ADC_CFG_SAMPLE_TIME          PX_ADC_CFG_SAMPLE_TIME_7_5

/// Specify ADC resolution (see 'px_adc_cfg_res_t')
#define PX_ADC_CFG_RES                  PX_ADC_CFG_RES_12_BITS

/// Specify ADC oversampling (see 'px_adc_cfg_oversampling_t')
#define PX_ADC_CFG_OVERSAMPLING         PX_ADC_CFG_OVERSAMPLING_NONE

/// Specify ADC oversampling (see 'px_adc_cfg_oversampling_shift_t')
#define PX_ADC_CFG_OVERSAMPLING_SHIFT   PX_ADC_CFG_OVERSAMPLING_SHIFT_NONE

/// @}
#endif // #ifndef __PX_ADC_CFG_H__
