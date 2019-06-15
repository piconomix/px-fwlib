#ifndef __PX_CFG_H__
#define __PX_CFG_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2014 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
 
    Title:          px_cfg.h : Persistent config settings for application
    Author(s):      Pieter Conradie
    Creation Date:  2014-06-10

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_adc.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Linear fit variables
typedef struct
{
    float scale;
    float offset;
} cfg_adc_linear_fit_t;

/// Config settings
typedef struct
{
    uint16_t             nr_of_samples;
    cfg_adc_linear_fit_t fit[PX_ADC_NR_OF_CHANNELS];
} cfg_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */
extern cfg_t cfg;

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
extern void cfg_init(void);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

#endif // #ifndef __PX_CFG_H__
