#ifndef __PX_ADC_H__
#define __PX_ADC_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2014 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
 
    Title:          px_adc.h : ADC Driver
    Author(s):      Pieter Conradie
    Creation Date:  2014-06-01

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
/// Number of ADC channels that are sampled
#define PX_ADC_NR_OF_CHANNELS  4

/// ADC Channel statistic data that is accumulated
typedef struct
{
    uint16_t min;  ///< Minimum value observed during sampling period
    uint16_t max;  ///< Maximum value observed during sampling period
    uint32_t sum;  ///< Accumulated sum over sampling perdiod
} px_adc_data_t;

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
void                  px_adc_init               (void);

uint16_t              px_adc_sample             (uint8_t channel);

void                  px_adc_start              (uint16_t sampling_freq_hz, 
                                                 uint16_t nr_of_samples_per_sum);
void                  px_adc_stop               (void);
bool                  px_adc_data_ready         (void);
void                  px_adc_rst_data_ready_flag(void);
const px_adc_data_t * px_adc_get_data           (uint8_t channel);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

#endif
