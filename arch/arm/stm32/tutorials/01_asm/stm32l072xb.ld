/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md

============================================================================= */

/* Set the output format based on the desired endianness (default, big, little) */
OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm","elf32-littlearm")

/* Specify the output machine architecture */
OUTPUT_ARCH(arm)

/* Specify the program entry point (first instruction to be executed) */
ENTRY(main)

/* Describe the location and size of blocks of memory in the target */
MEMORY
{
    FLASH (rx)   : ORIGIN = 0x08000000, LENGTH = 128k
    SRAM  (w!rw) : ORIGIN = 0x20000000, LENGTH = 20k
}

/* 
 *  Inform linker how to map input sections into output sections
 *  and where to place the output sections in memory.
 */
SECTIONS
{    
    /* ISR_VECTOR section contains the ARM vector table */
    .isr_vector :
    {
          KEEP (*(.isr_vector))
          . = ALIGN(4);
    } > FLASH
        
    /* TEXT segment holds executable instructions and constant data */
    .text :
    {
          *(.text .text.*)    
          . = ALIGN(4);
    } > FLASH
}
