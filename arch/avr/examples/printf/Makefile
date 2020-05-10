# ------------------------------------------------------------------------------
# Microchip AVR Makefile Template by Pieter Conradie <https://piconomix.com>
#
# Based on:
# - WinAVR Makefile Template written by Eric B. Weddington, Jörg Wunsch, et al.
# - WinARM template makefile by Martin Thomas, Kaiserslautern, Germany 
# - LUFA Makefiles by Dean Camera [www.lufa-lib.org]
# - http://stackoverflow.com Q&A research
#
# On command line:
#
# make             = Build project
# make check       = Check that source files exist
# make clean       = Delete intermediate output files and final binaries
# make mostlyclean = Delete intermediate output files, but preserve final 
#                    binaries
#
# To create a new project, copy this Makefile to your project directory and:
#
# 1. Specify a project name
# 2. Specify the MCU that the project will be compiled for
# 3. Specify paths
# 4. List source files to be included in project WITH PATHS (relative to Makefile)
# 5. Add include paths
# 6. Specify any extra preprocessor definitions for source files
#
# NOTE(S):
# - The MCU frequency F_CPU is defined in 'px_board.h' and should be #included 
#   first before including files that are dependant on it,
#   for example <avr/delay.h>
#
# Makefile script special requirements:
# - make 3.81 or greater (requires working eval function)
# - sh (sh.exe on MS-Windows platform; required for 'make check')
# - rm (to delete files and directories)
#
# ------------------------------------------------------------------------------
# Creation Date:  2012-07-13
# ------------------------------------------------------------------------------

# (1) Project name
PROJECT = printf

# (2) MCU name
MCU = atmega328p

# (3a) Path to root of Piconomix FW Library (relative to Makefile)
PX_LIB = ../../../..

# (3b) Path to microcontroller architecture (relative to Piconomix FW Library)
ARCH = arch/avr

# (3c) Path to board support package (relative to Makefile)
BSP = $(PX_LIB)/boards/avr/px_scorpion

# (4a) List C source files WITH PATHS (relative to Makefile) here
SRC += main.c
SRC += $(PX_LIB)/$(ARCH)/src/px_uart.c
SRC += $(PX_LIB)/$(ARCH)/src/px_uart_stdio.c
SRC += $(BSP)/src/px_board.c

# (4b) List C++ source files WITH PATHS (relative to Makefile) here
CPPSRC += 

# (4c) List Assembler source files WITH PATHS (relative to Makefile) here
#
#      Make them always end in a capital .S.  Files ending in a lowercase .s
#      will not be considered source files but generated files (assembler
#      output from the compiler).
#      Even though the MS-DOS/MS-Windows filesystem matches both .s and .S the 
#      same, it will preserve the spelling of the filenames, and gcc itself does
#      care about how the name is spelled on its command-line.
ASRC +=

# (5) List all directories to look for include files here
#
#     Each directory must be separated by a space.
#     Use forward slashes for directory separators.
#     For a directory that has spaces, enclose it in quotes.
#
#     The order specified here is very important!
#
#     If you want to overide an include file with the same name, your file must 
#     be found first in the search order specified below.
INCDIRS += cfg
INCDIRS += inc
INCDIRS += src
INCDIRS += $(PX_LIB)
INCDIRS += $(PX_LIB)/$(ARCH)/inc
INCDIRS += $(PX_LIB)/common/inc
INCDIRS += $(PX_LIB)/utils/inc
INCDIRS += $(BSP)/inc

# (6a) Place preprocessor DEFINE macros here for C sources (GCC option -D)
#CDEFS += PX_DBG=0
CDEFS += 

# (6b) Place preprocessor DEFINE macros here for C++ sources (GCC option -D)
#CPPDEFS += __STDC_LIMIT_MACROS
#CPPDEFS += __STDC_CONSTANT_MACROS
CPPDEFS += 

# (6c) Place preprocessor DEFINE macros here for ASM sources (GCC option -D)
ADEFS += 

# Object files directory
#     Do NOT make this empty and do NOT use the current directory (.)
OBJDIR = OBJ

#---------------- Compiler Options C -------------------------------------------
#  -g*:          generate debugging information
#  -O*:          optimization level (0,1,2,3 or s)
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
CFLAGS += -gdwarf-2
CFLAGS += -Os
CFLAGS += -funsigned-char
CFLAGS += -funsigned-bitfields
CFLAGS += -fpack-struct
CFLAGS += -fshort-enums
CFLAGS += -Wall
CFLAGS += -Wstrict-prototypes
#CFLAGS += -mshort-calls
#CFLAGS += -fno-unit-at-a-time
#CFLAGS += -Wundef
#CFLAGS += -Wunreachable-code
#CFLAGS += -Wsign-compare
CFLAGS += -std=gnu99
CFLAGS += -ffunction-sections -fdata-sections

#---------------- Compiler Options C++ -----------------------------------------
#  -g*:          generate debugging information
#  -O*:          optimization level (0,1,2,3 or s)
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
CPPFLAGS += -gdwarf-2
CPPFLAGS += -Os
CPPFLAGS += -funsigned-char
CPPFLAGS += -funsigned-bitfields
CPPFLAGS += -fpack-struct
CPPFLAGS += -fshort-enums
CPPFLAGS += -fno-exceptions
CPPFLAGS += -Wall
CPPFLAGS += -Wundef
#CPPFLAGS += -mshort-calls
#CPPFLAGS += -fno-unit-at-a-time
#CPPFLAGS += -Wstrict-prototypes
#CPPFLAGS += -Wunreachable-code
#CPPFLAGS += -Wsign-compare
CPPFLAGS += -std=gnu99

#---------------- Assembler Options --------------------------------------------
#  -Wa,...:   tell GCC to pass this to the assembler.
AFLAGS += 

#---------------- Library Options ----------------------------------------------
# Minimalistic printf version
PRINTF_LIB_MIN = -Wl,-u,vfprintf -lprintf_min

# Floating point printf version (requires MATH_LIB = -lm below)
PRINTF_LIB_FLOAT = -Wl,-u,vfprintf -lprintf_flt

# If this is left blank, then it will use the Standard printf version.
PRINTF_LIB = 
#PRINTF_LIB = $(PRINTF_LIB_MIN)
#PRINTF_LIB = $(PRINTF_LIB_FLOAT)

# Minimalistic scanf version
SCANF_LIB_MIN = -Wl,-u,vfscanf -lscanf_min

# Floating point + %[ scanf version (requires MATH_LIB = -lm below)
SCANF_LIB_FLOAT = -Wl,-u,vfscanf -lscanf_flt

# If this is left blank, then it will use the Standard scanf version.
SCANF_LIB = 
#SCANF_LIB = $(SCANF_LIB_MIN)
#SCANF_LIB = $(SCANF_LIB_FLOAT)

MATH_LIB = -lm

# List any extra directories to look for libraries here.
#     Each directory must be seperated by a space.
#     Use forward slashes for directory separators.
#     For a directory that has spaces, enclose it in quotes.
EXTRALIBDIRS = 

#---------------- External Memory Options --------------------------------------
# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# used for variables (.data/.bss) and heap (malloc()).
#EXTMEMOPTS = -Wl,-Tdata=0x801100,--defsym=__heap_end=0x80ffff

# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# only used for heap (malloc()).
#EXTMEMOPTS = -Wl,--section-start,.data=0x801100,--defsym=__heap_end=0x80ffff

EXTMEMOPTS =

#---------------- Linker Options -----------------------------------------------
#  -Wl,...:     tell GCC to pass this to linker.
LDFLAGS += -Wl,-gc-sections,--relax
LDFLAGS += $(EXTMEMOPTS)
LDFLAGS += $(PRINTF_LIB) $(SCANF_LIB) $(MATH_LIB)
#LDFLAGS += -T linker_script.ld

#===============================================================================

include $(PX_LIB)/$(ARCH)/MakeRules.mk
