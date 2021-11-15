/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_stm32_flash.ld : STM32 Linker Script for GNU ld
    Author(s):      Pieter Conradie
    Creation Date:  2018-05-18

============================================================================= */

/*
 *  This linker script is originally for the STM32cubeL0 library and in
 *  particular the following C startup code:
 *  px-fwlib/libs/STM32Cube/L0/Drivers/CMSIS/Device/ST/STM32L0xx/Source/Templates/gcc/startup_stm32l072xx.s
 *
 *  The default linker script for normal executables packaged with the GNU ARM 
 *  toolchain was used as reference:
 *  (arm-none-eabi/lib/ldscripts/armelf.x)
 *  
 *  Documentation references:
 *  
 *  [1] "GNU ld (GNU linker) 2.30 documentation"
 *      https://sourceware.org/binutils/docs-2.30/ld/index.html
 *  
 *  [2] "Application Binary Interface (ABI) for the ARM Architecture", ARM
 *      http://infocenter.arm.com/help/topic/com.arm.doc.ihi0036b/IHI0036B_bsabi.pdf
 *  
 *  [3] "System V Application Binary Interface", SCO
 *      http://www.sco.com/developers/devspecs/gabi41.pdf
 *      § "Special Sections", page 4-18 to 4-20, gives short explanation of each 
 *      section (.bss, .text, ...)
 *      http://www.sco.com/developers/gabi/latest/ch4.sheader.html#special_sections
 *  
 *  Notes:
 *  
 *  Full output section description:
 *  
 *       section [address] [(type)] :
 *       [AT(lma)]
 *       [ALIGN(section_align) | ALIGN_WITH_INPUT]
 *       [SUBALIGN(subsection_align)]
 *       [constraint]
 *       {
 *           output-section-command
 *           output-section-command
 *           ...
 *       } [>region] [AT>lma_region] [:phdr :phdr ...] [=fillexp] [,]
 *  
 *  See [1], §3.6.1, "Output Section Description"
 *  https://sourceware.org/binutils/docs-2.30/ld/Output-Section-Description.html#Output-Section-Description
 *  
 *  VMA = virtual memory address; executing address of section
 *  LMA = load memory address; address at which the section will be loaded
 *  See [1], §3.1 "Basic Linker Script Concepts"
 *  https://sourceware.org/binutils/docs-2.30/ld/Basic-Script-Concepts.html#Basic-Script-Concepts
 *  
 *  KEEP = mark sections that should not be eliminated with "--gc-sections"
 *  See [1]. §3.6.4.4 "Input Section and Garbage Collection"
 *  https://sourceware.org/binutils/docs/ld/Input-Section-Keep.html#Input-Section-Keep
 * 
 *  PROVIDE = define a symbol only if it is referenced and is not defined by any
 *  object. See [1], §3.5.2 "PROVIDE"
 *  https://sourceware.org/binutils/docs-2.30/ld/PROVIDE.html#PROVIDE
 *
 */

/* Set the output format based on the desired endianness (default, big, little) */
OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm","elf32-littlearm")

/* Specify the output machine architecture */
OUTPUT_ARCH(arm)

/* Specify the program entry point (first instruction to be executed) */
ENTRY(Reset_Handler)

/* Define the flash size allocated for the application if not already specified */
FLASH_SIZE = DEFINED(FLASH_SIZE)? FLASH_SIZE : 32k;

/* Define the SRAM size allocated for the application if not already specified */
SRAM_SIZE = DEFINED(SRAM_SIZE)? SRAM_SIZE : 8k;

/* Describe the location and size of blocks of memory in the target */
FLASH_OFFSET = DEFINED(FLASH_OFFSET)? FLASH_OFFSET : 0;
FLASH_ORIGIN = 0x08000000 + FLASH_OFFSET;
FLASH_LENGTH = FLASH_SIZE - FLASH_OFFSET;
SRAM_LENGTH  = SRAM_SIZE;

MEMORY
{
    FLASH (rx)   : ORIGIN = FLASH_ORIGIN, LENGTH = FLASH_LENGTH
    SRAM  (w!rw) : ORIGIN = 0x20000000, LENGTH = SRAM_LENGTH
}

/* Define the stack size allocated for the application if not already specified */
STACK_SIZE = DEFINED(STACK_SIZE)? STACK_SIZE : 1024;

/* Define the heap size allocated for the application if not already specified */
HEAP_SIZE = DEFINED(HEAP_SIZE)? HEAP_SIZE : 1024;

/* Define the start address of the stack (placed at the end of SRAM) */
STACK_START = ORIGIN(SRAM) + LENGTH(SRAM) - STACK_SIZE;

/* 
 * Inform linker how to map input sections into output sections, and where to 
 * place the output sections in memory.
 */
SECTIONS
{    
    /*
     * ISR_VECTOR section contains the ARM vector table as defined in 
     * for example startup_stm32l072xx.s
     */
    .isr_vector :
    {
          KEEP (*(.isr_vector))
          . = ALIGN(4);
    } > FLASH
    
    /*
     * INIT section holds executable instructions that contribute to the
     * process initialization code.
     */
    .init :
    {
          KEEP(*(.init))
          . = ALIGN(4);
    } > FLASH
    
    /* TEXT segment holds executable instructions and constant data */
    .text :
    {
          /* .text sections (executable code) */
          *(.text .text.*)
    
          /* ARM-Thumb interworking veneers (arm to thumb code) */
          *(.glue_7)
          /* ARM-Thumb interworking veneers (thumb to arm code) */
          *(.glue_7t)
    
          /* Section unwinding and source language info */
          *(.eh_frame)
    
          . = ALIGN(4);
    } > FLASH
    
    /*
     * FINI section holds executable instructions that contribute to the
     * process termination code.
     */
    .fini :
    {
          KEEP(*(.fini))
          . = ALIGN(4);
    } > FLASH  
    
    /*
     * RODATA section hold read-only data (constants, strings, ...)
     */
    .rodata :
    { 
          *(.rodata .rodata.* .gnu.linkonce.r.*)
          . = ALIGN(4);
    } > FLASH

    .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } > FLASH
    .ARM :
    {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
    } > FLASH
    
    .preinit_array     :
    {
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array*))
        PROVIDE_HIDDEN (__preinit_array_end = .);
    } > FLASH

    /*
     * INIT_ARRAY section holds an array of function pointers to call for
     * C++ static constructors and C library initialization code.
     */
    .init_array :
    {
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array*))
        PROVIDE_HIDDEN (__init_array_end = .);
    } > FLASH

    /*
     * FINI_ARRAY section holds an array of function pointers to call for
     * C++ static destructors C library finalization code.
     */
    .fini_array :
    {
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.fini_array*))
        PROVIDE_HIDDEN (__fini_array_end = .);
    } > FLASH
    
    /*
     * DATA section holds initialized variables. Initialisation values are 
     * stored in FLASH (LMA) and copied to SRAM (VMA) on startup.
     */
    .data :
    {
          /* Create symbol to mark start of initialization data in FLASH (LMA) */
          _sidata = LOADADDR(.data);
          /* Create symbol to mark start of initialized data in SRAM (VMA) */
          _sdata  = . ;
          
          /* .data sections (initialized variables) */
          *(.data .data.*)
          . = ALIGN(4);

          /* Functions that is stored in FLASH, but must be executed in SRAM */
          *(.RamFunc)
          . = ALIGN(4);
          
          /* Create symbol to mark end of initialized data in SRAM (VMA) */
          _edata = .;
    } > SRAM AT> FLASH
    
    /* BSS section holds uninitialized variables. It is set to zero on startup. */
    .bss (NOLOAD) :
    {
          /* Create symbols to mark start of unitialized data in SRAM */
          _sbss         = .;
          __bss_start__ = _sbss;
    
          /* .bss sections (uninitialized variables) */
          *(.bss .bss.*)
    
          /* 
           * Collect symbols that do not have a particular input section. See
           * https://sourceware.org/binutils/docs/ld/Input-Section-Common.html
           */
          *(COMMON)
    
          . = ALIGN(4);
    
          /* Create symbols to mark end of unitialized data in SRAM */
          _ebss       = . ;
          __bss_end__ = _ebss;
    } > SRAM

    /*
     * NOINIT section holds uninitialized variables that should not be set to 
     * zero on startup.
     */
    .noinit (NOLOAD) :
    {
          KEEP (*(.noinit))
          . = ALIGN(4);
    } > SRAM
    
    /*
     * HEAP section allocates space for the application heap
     */
    .heap :
    {
        . = ALIGN(8);
        /* Mark the start of heap (if used) */
        __end__ = . ;
        _end    = . ;
        PROVIDE (end = . );

        . = . + HEAP_SIZE;
    } > SRAM

    /*
     * STACK section allocates space for the application stack. It is placed
     * at the end of SRAM.
     */
    .stack STACK_START :
    {
        . += STACK_SIZE;
        _estack = .;
    } > SRAM
    
    /* Stabs debugging sections */
    .stab          0 : { *(.stab) }
    .stabstr       0 : { *(.stabstr) }
    .stab.excl     0 : { *(.stab.excl) }
    .stab.exclstr  0 : { *(.stab.exclstr) }
    .stab.index    0 : { *(.stab.index) }
    .stab.indexstr 0 : { *(.stab.indexstr) }
    .comment       0 : { *(.comment) }
    
    /* 
     * DWARF debug sections.
     * Symbols in the DWARF debugging sections are relative to the beginning
     * of the section so we begin them at 0.
     */
    
    /* DWARF 1 */
    .debug          0 : { *(.debug) }
    .line           0 : { *(.line) }
    
    /* GNU DWARF 1 extensions */
    .debug_srcinfo  0 : { *(.debug_srcinfo) }
    .debug_sfnames  0 : { *(.debug_sfnames) }
    
    /* DWARF 1.1 and DWARF 2 */
    .debug_aranges  0 : { *(.debug_aranges) }
    .debug_pubnames 0 : { *(.debug_pubnames) }
    
    /* DWARF 2 */
    .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
    .debug_abbrev   0 : { *(.debug_abbrev) }
    .debug_line     0 : { *(.debug_line) }
    .debug_frame    0 : { *(.debug_frame) }
    .debug_str      0 : { *(.debug_str) }
    .debug_loc      0 : { *(.debug_loc) }
    .debug_macinfo  0 : { *(.debug_macinfo) }
    
    /* SGI/MIPS DWARF 2 extensions */
    .debug_weaknames 0 : { *(.debug_weaknames) }
    .debug_funcnames 0 : { *(.debug_funcnames) }
    .debug_typenames 0 : { *(.debug_typenames) }
    .debug_varnames  0 : { *(.debug_varnames) }
    
    /* DWARF 3 */
    .debug_pubtypes 0 : { *(.debug_pubtypes) }
    .debug_ranges   0 : { *(.debug_ranges) }
      
    .ARM.attributes 0 : { KEEP (*(.ARM.attributes)) KEEP (*(.gnu.attributes)) }
    .note.gnu.arm.ident 0 : { KEEP (*(.note.gnu.arm.ident)) }
    
    /*
     * DISCARD section.
     * Discard input sections. See [1], §3.6.7 "Output Section Discarding"
     * https://sourceware.org/binutils/docs/ld/Output-Section-Discarding.html#Output-Section-Discarding
     */
    /DISCARD/ :
    {
        *(.note.GNU-stack) *(.gnu_debuglink)
    }
}
