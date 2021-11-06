#ifndef __PX_ADC_CFG_H__
#define __PX_ADC_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_adc_cfg.h : ADC Peripheral Driver configuration
    Author(s):      Pieter Conradie
    Creation Date:  2018-11-04

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

/* _____DEFINITIONS__________________________________________________________ */
/// Enable/disable support for ADC1 peripheral
#define PX_ADC_CFG_ADC1_EN              1

/// Specify ADC clock source (see 'px_adc_cfg_clk_t')
#define PX_ADC_CFG_CLK                  PX_ADC_CFG_CLK_PCLK

/// Specify ADC sampling time (see 'px_adc_cfg_sample_time_t')
#define PX_ADC_CFG_SAMPLE_TIME          PX_ADC_CFG_SAMPLE_TIME_160_5

/// Specify ADC resolution (see 'px_adc_cfg_res_t')
#define PX_ADC_CFG_RES                  PX_ADC_CFG_RES_12_BITS

/// Specify ADC oversampling (see 'px_adc_cfg_oversampling_t')
#define PX_ADC_CFG_OVERSAMPLING         PX_ADC_CFG_OVERSAMPLING_NONE

/// Specify ADC oversampling (see 'px_adc_cfg_oversampling_shift_t')
#define PX_ADC_CFG_OVERSAMPLING_SHIFT   PX_ADC_CFG_OVERSAMPLING_SHIFT_NONE

#endif
