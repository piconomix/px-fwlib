#ifndef __PX_ADC_H__
#define __PX_ADC_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
 
    Title:          px_adc.h : ADC peripheral driver
    Author(s):      Pieter Conradie
    Creation Date:  2018-11-04

============================================================================= */

/** 
 *  @ingroup STM32
 *  @defgroup STM32_ADC px_adc.h : ADC peripheral driver
 *  
 *  Driver to perform analogue measurements.
 *  
 *  File(s):
 *  - arch/arm/stm32/inc/px_adc.h
 *  - arch/arm/stm32/inc/px_adc_cfg_template.h
 *  - arch/arm/stm32/src/px_adc.c
 *  
 *  The driver must be configured by supplying a project specific "px_adc_cfg.h".
 *  "px_adc_cfg_template.h" can be copied, renamed and modified to supply 
 *  compile time options.
 *  
 *  @par Example:
 *  @include arch/arm/stm32/test/px_adc_test.c
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

// Include project specific configuration. See "px_adc_cfg_template.h"
#include "px_adc_cfg.h"

// Check that all project specific options have been specified in "px_adc_cfg.h"
#if (   !defined(PX_ADC_CFG_ADC1_EN           ) \
     || !defined(PX_ADC_CFG_CLK               ) \
     || !defined(PX_ADC_CFG_SAMPLE_TIME       ) \
     || !defined(PX_ADC_CFG_RES               ) \
     || !defined(PX_ADC_CFG_OVERSAMPLING      ) \
     || !defined(PX_ADC_CFG_OVERSAMPLING_SHIFT) )
#error "One or more options not defined in 'px_adc_cfg.h'"
#endif
#if (PX_ADC_CFG_ADC1_EN > 1)
#error "PX_ADC_CFG_ADCx_EN must be 0 or 1"
#endif

/// Number of enabled peripherals
#define PX_ADC_CFG_PER_COUNT    (PX_ADC_CFG_ADC1_EN)
#if (PX_ADC_CFG_PER_COUNT == 0)
#error "No peripherals enabled"
#endif

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
/// Specify ADC peripheral
typedef enum
{
    PX_ADC_NR_1 = 1,
} px_adc_nr_t;

/// Specify ADC channel
typedef enum
{
    PX_ADC_CH0  = 0,                        ///< ADC Channel 0
    PX_ADC_CH1  = 1,                        ///< ADC Channel 1
    PX_ADC_CH2  = 2,                        ///< ADC Channel 2
    PX_ADC_CH3  = 3,                        ///< ADC Channel 3
    PX_ADC_CH4  = 4,                        ///< ADC Channel 4
    PX_ADC_CH5  = 5,                        ///< ADC Channel 5
    PX_ADC_CH6  = 6,                        ///< ADC Channel 6
    PX_ADC_CH7  = 7,                        ///< ADC Channel 7
    PX_ADC_CH8  = 8,                        ///< ADC Channel 8
    PX_ADC_CH9  = 9,                        ///< ADC Channel 9
    PX_ADC_CH10 = 10,                       ///< ADC Channel 10
    PX_ADC_CH11 = 11,                       ///< ADC Channel 11
    PX_ADC_CH12 = 12,                       ///< ADC Channel 12
    PX_ADC_CH13 = 13,                       ///< ADC Channel 13
    PX_ADC_CH14 = 14,                       ///< ADC Channel 14
    PX_ADC_CH15 = 15,                       ///< ADC Channel 15
    PX_ADC_CH16 = 16,                       
    PX_ADC_CH17 = 17,                       ///< Internal Voltage Reference (Vrefint)
    PX_ADC_CH18 = 18,                       ///< Temperature sensor (Tsense)
} px_adc_ch_t;

/// Specify ADC peripheral clock source
typedef enum
{
    PX_ADC_CFG_CLK_PCLK      = 0,           ///< Peripheral Clock
    PX_ADC_CFG_CLK_PCLK_DIV2 = 1,           ///< Peripheral Clock / 2
    PX_ADC_CFG_CLK_PCLK_DIV4 = 2,           ///< Peripheral Clock / 4
} px_adc_cfg_clk_t;

/// Specify ADC sampling time
typedef enum
{
    PX_ADC_CFG_SAMPLE_TIME_1_5   = 0,       ///< 1.5 ADC clock cycles
    PX_ADC_CFG_SAMPLE_TIME_3_5   = 1,       ///< 3.5 ADC clock cycles
    PX_ADC_CFG_SAMPLE_TIME_7_5   = 2,       ///< 7.5 ADC clock cycles
    PX_ADC_CFG_SAMPLE_TIME_12_5  = 3,       ///< 12.5 ADC clock cycles
    PX_ADC_CFG_SAMPLE_TIME_19_5  = 4,       ///< 19.5 ADC clock cycles
    PX_ADC_CFG_SAMPLE_TIME_39_5  = 5,       ///< 39.5 ADC clock cycles
    PX_ADC_CFG_SAMPLE_TIME_79_5  = 6,       ///< 79.5 ADC clock cycles
    PX_ADC_CFG_SAMPLE_TIME_160_5 = 7,       ///< 160.5 ADC clock cycles
} px_adc_cfg_sample_time_t;

/// Specify ADC resolution
typedef enum
{
    PX_ADC_CFG_RES_12_BITS = 0,             ///< 12-bit resolution
    PX_ADC_CFG_RES_10_BITS = 1,             ///< 10-bit resolution
    PX_ADC_CFG_RES_8_BITS  = 2,             ///< 8-bit resolution
    PX_ADC_CFG_RES_6_BITS  = 3,             ///< 6-bit resolution
} px_adc_cfg_res_t;

/// Specify ADC oversampling
typedef enum
{
    PX_ADC_CFG_OVERSAMPLING_NONE = 0,       ///< No oversampling
    PX_ADC_CFG_OVERSAMPLING_2    = 1,       ///< 2 x oversampling
    PX_ADC_CFG_OVERSAMPLING_4    = 2,       ///< 4 x oversampling
    PX_ADC_CFG_OVERSAMPLING_8    = 3,       ///< 8 x oversampling
    PX_ADC_CFG_OVERSAMPLING_16   = 4,       ///< 16 x oversampling
    PX_ADC_CFG_OVERSAMPLING_32   = 5,       ///< 32 x oversampling
    PX_ADC_CFG_OVERSAMPLING_64   = 6,       ///< 64 x oversampling
    PX_ADC_CFG_OVERSAMPLING_128  = 7,       ///< 128 x oversampling
    PX_ADC_CFG_OVERSAMPLING_256  = 8,       ///< 256 x oversampling
} px_adc_cfg_oversampling_t;

/// Specify ADC oversampling
typedef enum
{
    PX_ADC_CFG_OVERSAMPLING_SHIFT_NONE = 0, ///< No shift
    PX_ADC_CFG_OVERSAMPLING_SHIFT_1    = 1, ///< Shift 1-bit
    PX_ADC_CFG_OVERSAMPLING_SHIFT_2    = 2, ///< Shift 2-bits
    PX_ADC_CFG_OVERSAMPLING_SHIFT_3    = 3, ///< Shift 3-bits
    PX_ADC_CFG_OVERSAMPLING_SHIFT_4    = 4, ///< Shift 4-bits
    PX_ADC_CFG_OVERSAMPLING_SHIFT_5    = 5, ///< Shift 5-bits
    PX_ADC_CFG_OVERSAMPLING_SHIFT_6    = 6, ///< Shift 6-bits
    PX_ADC_CFG_OVERSAMPLING_SHIFT_7    = 7, ///< Shift 7-bits
    PX_ADC_CFG_OVERSAMPLING_SHIFT_8    = 8, ///< Shift 8-bits
} px_adc_cfg_oversampling_shift_t;

/// Define ADC handle
typedef struct
{
    struct px_adc_per_s * adc_per;          ///< ADC peripheral data
} px_adc_handle_t;

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise ADC driver.
 */
void px_adc_init(void);

/** 
 *  Open ADC peripheral using predefined (default) parameters.
 *  
 *  @param handle       Pointer to handle data structure
 *  @param adc_nr       ADC peripheral number
 *  
 *  @retval false       Error: peripheral was not opened
 *  @retval true        Success: peripheral was opened
 */
bool px_adc_open(px_adc_handle_t * handle,
                 px_adc_nr_t       adc_nr);

/**
 *  Close specified device.
 *  
 *  @param handle       Pointer to handle data structure
 *  
 *  @retval true        Success 
 *  @retval false       Specified device was already closed (or not opened)
 */
bool px_adc_close(px_adc_handle_t * handle);

/**
 *  Perform a single measurement on the specified ADC channel
 *  
 *  @param handle       Pointer to handle data structure
 *  @param ch           Specified ADC channel
 *  
 *  @return uint16_t    Raw ADC value
 */
uint16_t px_adc_sample(px_adc_handle_t * handle, 
                       px_adc_ch_t       ch);

/* _____MACROS_______________________________________________________________ */
/// Convert ADC channel number into bit mask
#define PX_ADC_CH_TO_BITMASK(ch)    ((uint32_t)1<<(ch))

#ifdef __cplusplus
}
#endif

/// @}
#endif
