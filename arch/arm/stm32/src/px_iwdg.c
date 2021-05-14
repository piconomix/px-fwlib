/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2021 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md

    Title:          px_iwdg.h : Watchdog module
    Author(s):      Pieter Conradie
    Creation Date:  2021-05-08

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_iwdg.h"
#include "px_lib_stm32cube.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("px_iwdg");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_iwdg_init(px_iwdg_prescaler_t prescaler, uint16_t reload, uint16_t window)
{
    PX_LOG_ASSERT(reload < 0x1000);
    PX_LOG_ASSERT(window < 0x1000);

    // Enable peripheral
    LL_IWDG_Enable(IWDG);
    // Enable write access to prescaler, reload and window registers
    LL_IWDG_EnableWriteAccess(IWDG);
    // Set clock (40 kHz LSI) prescaler
    LL_IWDG_SetPrescaler(IWDG, prescaler);
    // Set counter reload value (maximum period before Watchdog will reset processor)
    LL_IWDG_SetReloadCounter(IWDG, reload);
    // Set watchdog counter window
    if(window != 0)
    {
        LL_IWDG_SetWindow(IWDG, window);
    }
    // Wait until all values have been updated
    while(!LL_IWDG_IsReady(IWDG))
    {
        ;
    }
#if 0
    // Freeze watchdog when processor is halted by debugger
    LL_DBGMCU_APB1_GRP1_FreezePeriph(LL_DBGMCU_APB1_GRP1_IWDG_STOP);
#endif
}

void px_iwdg_reload_counter(void)
{
    LL_IWDG_ReloadCounter(IWDG);
}

