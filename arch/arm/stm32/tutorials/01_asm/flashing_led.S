/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

============================================================================= */

/* Use 16-bit Thumb instruction set */
.thumb
/* Use modern UAL (Unified Assembler Language) common syntax for ARM and Thumb instructions */
.syntax unified

/* RCC_IOPENR (Reset and Clock Control - GPIO clock enable register) address */
.equ RCC_IOPENR,    0x4002102c
/* GPIOH_MODER (GPIO port H mode register) address */
.equ GPIOH_MODER,   0x50001c00
/* GPIOH_BSRR (GPIO port H bit set/reset register) address */
.equ GPIOH_BSRR,    0x50001c18

/* ISR_VECTOR section contains the ARM vector table; see linker script "stm32l072xb.ld" */
.section .isr_vector,"a",%progbits
.type  vector_table, %object
vector_table:
    .word 0x20005000            /* Set SP (Stack Pointer) to end of SRAM */
    .word main                  /* Set PC (Program Counter) to main() function address */
    .word nmi_handler           /* Address of NMI (Non Maskable Interrupt) handler function */
    .word hard_fault_handler    /* Address of Hard Fault handler function */

/* TEXT section holds executable instructions and constant data; see linker script "stm32l072xb.ld" */
.text

/* nmi_handler() function */
.global nmi_handler
.type  nmi_handler, %function
nmi_handler:
    /* Loop here forever */
    b       nmi_handler

/* hard_fault_handler() function */
.global hard_fault_handler
.type  hard_fault_handler, %function
hard_fault_handler:
    /* Loop here forever */
    b       hard_fault_handler

/* main() function */
.global main
.type  main, %function
main:
    /* Read RCC_IOPENR register value */
    ldr     r0, =RCC_IOPENR
    ldr     r1, [r0]
    /* Set RCC_IOPENR register bit 7 to enable clock of Port H */    
    ldr     r2, =0x00000080
    orrs    r1, r2
    str     r1, [r0]
    /* Read GPIOH_MODER register value */
    ldr     r0, =GPIOH_MODER
    ldr     r1, [r0]
    /* Set MODE0[1:0] = 01 (General purpose output mode) */
    ldr     r2, =0xfffffffd
    ands    r1, r2
    ldr     r2, =0x00000001
    orrs    r1, r2
    /* Write updated GPIOH_MODER register value */
    str     r1, [r0]

    /* Load R0 with GPIOH_BSRR address */
    ldr     r0, =GPIOH_BSRR    
_main_loop:
    /* Set PH0 output to enable LED */
    ldr     r1, =0x00000001
    str     r1, [r0]
    /* Call delay function; Return address is stored in LR (Link Register / R13) */
    bl      delay
    /* Clear PH0 output to disable LED */
    ldr     r1, =0x00010000
    str     r1, [r0]
    /* Call delay function; Return address is stored in LR (Link Register / R13) */
    bl      delay
    /* Loop forever */
    b       _main_loop

/*
 * delay() function:
 *
 * Wait ~ 500 ms. Default system clock is 2.1 MHz after startup from reset
 * which means that one instruction clock cycle is 476 ns.
 */
.global delay
.type  delay, %function
delay:
    /*
     * Save R0 and LR (Link Register / R13) register content on stack (SP / R14).
     * LR contains the program address to return to when function finishes.
     */
    push    {r0, lr}
    /* 
     * Set R0 to 0x00040000 (262 144) by loading it with 0x04 and shifting 
     * it left by 16 bits (multiplying it with 2^16 = 65 536)
     */
    movs	r0, #0x04
    lsls	r0, #16
    /*
     * These two 16-bit Thumb instructions use less program memory space than
     * using a "ldr r0, =0x00040000" instruction, because the latter generates a 
     * 16-bit instruction plus a 32-bit constant value that is stored in 
     * program memory (at the end in the literal pool).
     */

    /* Decrement R0 until it is zero; this is the actual delay loop */
_delay_loop:
    nop                     /* 1 instruction clock cycle */
    subs    r0, #1          /* 1 instruction clock cycle */
    bne     _delay_loop     /* 2 instruction clock cycles if branch is taken */
    /* 
     *  Restore R0 saved value from stack and restore LR (Link Register / R13) 
     *  saved value directly into PC (Program Counter / R15) to return.
     */
    pop     {r0, pc}

/* Place literal pool (constants) here after the main() and delay() function */
.pool

