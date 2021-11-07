/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2021 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          px_wwdg.h : Watchdog module
    Author(s):      Pieter Conradie
    Creation Date:  2021-05-08

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_wwdg.h"
#include "px_stm32cube.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("px_wwdg");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static uint8_t px_wwdg_counter_reload;

#ifdef PX_WWDG_CFG_EARLY_WARNING_FLAG
PX_ATTR_SECTION(".noinit") volatile uint32_t px_wwdg_ew_flag;
#endif

#ifdef PX_WWDG_CFG_EARLY_WARNING_REGS
PX_ATTR_SECTION(".noinit") volatile uint32_t px_wwdg_r0;
PX_ATTR_SECTION(".noinit") volatile uint32_t px_wwdg_r1;
PX_ATTR_SECTION(".noinit") volatile uint32_t px_wwdg_r2;
PX_ATTR_SECTION(".noinit") volatile uint32_t px_wwdg_r3;
PX_ATTR_SECTION(".noinit") volatile uint32_t px_wwdg_r12;
PX_ATTR_SECTION(".noinit") volatile uint32_t px_wwdg_lr;
PX_ATTR_SECTION(".noinit") volatile uint32_t px_wwdg_pc;
PX_ATTR_SECTION(".noinit") volatile uint32_t px_wwdg_psr;
#endif

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
#ifdef PX_WWDG_CFG_EARLY_WARNING_FLAG
void WWDG_IRQHandler(void)
{
    LL_WWDG_ClearFlag_EWKUP(WWDG);
    px_wwdg_ew_flag = 0xdeadc0de;
}
#endif

#ifdef PX_WWDG_CFG_EARLY_WARNING_REGS
/*
 *  Origin of register saving code here:
 *  https://stackoverflow.com/questions/38618440/retrieving-return-address-of-an-exception-on-arm-cortex-m0
 *
 *  Credit: Andrea Biondo
 */
PX_ATTR_NORETURN void px_wwdg_save_regs(uint32_t * stack_adr)
{
    px_wwdg_r0  = stack_adr[0]; // (sp + 0x00)
    px_wwdg_r1  = stack_adr[1]; // (sp + 0x04)
    px_wwdg_r2  = stack_adr[2]; // (sp + 0x08)
    px_wwdg_r3  = stack_adr[3]; // (sp + 0x0c)
    px_wwdg_r12 = stack_adr[4]; // (sp + 0x10)
    px_wwdg_lr  = stack_adr[5]; // (sp + 0x14)
    px_wwdg_pc  = stack_adr[6]; // (sp + 0x18)
    px_wwdg_psr = stack_adr[7]; // (sp + 0x1c)

    // Block forever
    while(true) {;}
}

void WWDG_IRQHandler(void) PX_ATTR_NAKED;
void WWDG_IRQHandler(void)
{
#if (__CORTEX_M > 0x03U)
    // Arm Cortex M3/M4 implementation
    __asm__ __volatile__
    (
        "tst lr, #4                     \n\t"
        "ite eq                         \n\t"
        "mrseq r0, msp                  \n\t"
        "mrsne r0, psp                  \n\t"
        "ldr r1, =px_wwdg_save_regs     \n\t"
        "bx r1                          \n\t"
    );
#else
    // Arm Cortex M0/M0+ implementation
    __asm__ __volatile__
    (
        "mrs r0, msp                    \n\t"
        "mov r1, lr                     \n\t"
        "mov r2, #4                     \n\t"
        "tst r1, r2                     \n\t"
        "beq px_wwdg_save_regs_call     \n\t"
        "mrs r0, psp                    \n\t"
        "px_wwdg_save_regs_call:        \n\t"
        "ldr r1, =px_wwdg_save_regs     \n\t"
        "bx  r1                         \n\t"
    );
#endif
}
#endif


/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_wwdg_init(px_wwdg_prescaler_t prescaler, uint8_t counter, uint8_t window)
{
    PX_LOG_ASSERT((window  > 0x40) && (window  < 0x80));
    PX_LOG_ASSERT((counter > 0x40) && (counter < 0x80));

    // Enable peripheral clock
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_WWDG);
    // Set clock (PCLK / 4096) prescaler
    LL_WWDG_SetPrescaler(WWDG, ((uint32_t)prescaler) << WWDG_CFR_WDGTB_Pos);
    // Set watchdog counter window
    if(window != 0)
    {
        LL_WWDG_SetWindow(WWDG, window);
    }
    // Save counter value and load counter
    px_wwdg_counter_reload = counter;
    LL_WWDG_SetCounter(WWDG, counter);

#ifdef PX_WWDG_CFG_EARLY_WARNING_FLAG
    // Reset early warning flag
    px_wwdg_ew_flag = 0;
    // Clear interrupt flag
    LL_WWDG_ClearFlag_EWKUP(WWDG);
    // Enable early wake up interrupt
    NVIC_SetPriority(WWDG_IRQn, 0);
    NVIC_EnableIRQ(WWDG_IRQn);
    LL_WWDG_EnableIT_EWKUP(WWDG);
#endif

#ifdef PX_WWDG_CFG_EARLY_WARNING_REGS
    // Reset saved register values
    px_wwdg_r0  = 0;
    px_wwdg_r1  = 0;
    px_wwdg_r2  = 0;
    px_wwdg_r3  = 0;
    px_wwdg_r12 = 0;
    px_wwdg_lr  = 0;
    px_wwdg_pc  = 0;
    px_wwdg_psr = 0;
    // Clear interrupt flag
    LL_WWDG_ClearFlag_EWKUP(WWDG);
    // Enable early wake up interrupt
    NVIC_SetPriority(WWDG_IRQn, 0);
    NVIC_EnableIRQ(WWDG_IRQn);
    LL_WWDG_EnableIT_EWKUP(WWDG);
#endif

#if 0
    // Freeze watchdog when processor is halted by debugger
    LL_DBGMCU_APB1_GRP1_FreezePeriph(LL_DBGMCU_APB1_GRP1_WWDG_STOP);
#endif

    // Enable peripheral
    LL_WWDG_Enable(WWDG);
}

void px_wwdg_reload_counter(void)
{
    if(!LL_WWDG_IsEnabled(WWDG))
    {
        return;
    }
    if(px_wwdg_counter_reload <= 0x40)
    {
        return;
    }
    LL_WWDG_SetCounter(WWDG, px_wwdg_counter_reload);
}

void px_wwdg_halt(void)
{
    LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_WWDG);
}

void px_wwdg_resume(void)
{
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_WWDG);
}

void px_wwdg_dbg_report(void)
{
#ifdef PX_WWDG_CFG_EARLY_WARNING_FLAG
    PX_LOG_TRACE("ew_flag = 0x%08lX\n", px_wwdg_ew_flag);
#endif

#ifdef PX_WWDG_CFG_EARLY_WARNING_REGS
    PX_LOG_TRACE("r0 = 0x%08lX\n", px_wwdg_r0);
    PX_LOG_TRACE("r1 = 0x%08lX\n", px_wwdg_r1);
    PX_LOG_TRACE("r2 = 0x%08lX\n", px_wwdg_r2);
    PX_LOG_TRACE("r3 = 0x%08lX\n", px_wwdg_r3);
    PX_LOG_TRACE("r1 = 0x%08lX\n", px_wwdg_r12);
    PX_LOG_TRACE("lr = 0x%08lX\n", px_wwdg_lr);
    PX_LOG_TRACE("pc = 0x%08lX\n", px_wwdg_pc);
    PX_LOG_TRACE("psr = 0x%08lX\n", px_wwdg_psr);
#endif
}

