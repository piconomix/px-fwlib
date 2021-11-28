# ------------------------------------------------------------------------------
# Microchip AVR Makefile Template by Pieter Conradie <https://piconomix.com>
#
# Based on:
# - WinAVR Makefile Template written by Eric B. Weddington, Jörg Wunsch, et al.
# - WinARM template makefile by Martin Thomas, Kaiserslautern, Germany
# - DMBS - Dean's Makefile Build System by Dean Camera [https://github.com/abcminiuser/dmbs]
# - http://stackoverflow.com Q&A research
# - Automatically generate makefile dependencies (http://www.microhowto.info/howto/automatically_generate_makefile_dependencies.html)
# - GNU make manual (https://www.gnu.org/software/make/manual/make.html)
#
# On command line:
#
# make                    = Build project (debug version)
# make build=debug        = Build project (debug version)
# make build=release      = Build project (release version)
# make clean              = Delete build and final output files
# make mostlyclean        = Delete build files, but preserve final output files
# make program            = Program target using command line utility
# make reset              = Reset target using command line utility
#
# To create a new project, copy this Makefile to your project directory and:
#
# 1. Specify a project name
# 2. Specify the MCU options that the project will be compiled for
# 3. Specify paths
# 4. List source files to be included in project WITH PATHS (relative to Makefile)
# 5. Add include paths
# 6. Specify any extra preprocessor definitions for source files
#
# Makefile script special requirements:
# - make 3.81 or greater (requires working eval function)
# - sh (sh.exe on MS-Windows platform)
# - rm (to delete files and directories)
# - mkdir (to create directories)
#
# ------------------------------------------------------------------------------
# Creation Date:  2012-07-13
# ------------------------------------------------------------------------------

# (1) Project name
PROJECT = adc

# (2) MCU option(s)
MCU = atmega328p

# (3a) Path to root of Piconomix FW Library (relative to Makefile)
PX_FWLIB = ../../../..

# (3b) Path to microcontroller architecture (relative to Piconomix FW Library)
ARCH = arch/avr

# (3c) Path to board support package (relative to Makefile)
BSP = $(PX_FWLIB)/boards/avr/px_scorpion

# (4a) List C source files WITH PATHS (relative to Makefile) here
SRC += $(PROJECT).c
SRC += $(BSP)/src/px_board.c

# (4b) List C++ source files WITH PATHS (relative to Makefile) here
CXXSRC += 

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
INCDIRS += $(BSP)/inc
INCDIRS += $(PX_FWLIB)
INCDIRS += $(PX_FWLIB)/$(ARCH)/inc
INCDIRS += $(PX_FWLIB)/common/inc
INCDIRS += $(PX_FWLIB)/utils/inc

# (6a) Place preprocessor DEFINE macros here for C sources (GCC option -D)
#CDEFS += PX_LOG=1
CDEFS += 

# (6b) Place preprocessor DEFINE macros here for C++ sources (GCC option -D)
#CXXDEFS += __STDC_LIMIT_MACROS
#CXXDEFS += __STDC_CONSTANT_MACROS
CXXDEFS += 

# (6c) Place preprocessor DEFINE macros here for ASM sources (GCC option -D)
ADEFS += 

#---------------- Compiler Options C -------------------------------------------
#  -g*:          generate debugging information
#  -O*:          optimization level (0,1,2,3 or s)
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
CFLAGS += -gdwarf-2
CFLAGS += -funsigned-char
CFLAGS += -funsigned-bitfields
CFLAGS += -fshort-enums
CFLAGS += -Wall
CFLAGS += -Wstrict-prototypes
#CFLAGS += -mshort-calls
#CFLAGS += -fno-unit-at-a-time
#CFLAGS += -Wundef
#CFLAGS += -Wunreachable-code
#CFLAGS += -Wsign-compare
CFLAGS += -std=gnu99
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections

CFLAGS_DEBUG   += -Og
CFLAGS_RELEASE += -Os

#---------------- Compiler Options C++ -----------------------------------------
#  -g*:          generate debugging information
#  -O*:          optimization level (0,1,2,3 or s)
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
CXXFLAGS += -gdwarf-2
CXXFLAGS += -funsigned-char
CXXFLAGS += -funsigned-bitfields
CXXFLAGS += -fshort-enums
CXXFLAGS += -fno-exceptions
CXXFLAGS += -Wall
CXXFLAGS += -Wundef
#CXXFLAGS += -mshort-calls
#CXXFLAGS += -fno-unit-at-a-time
#CXXFLAGS += -Wstrict-prototypes
#CXXFLAGS += -Wunreachable-code
#CXXFLAGS += -Wsign-compare
CXXFLAGS += -std=gnu++11
CXXFLAGS += -ffunction-sections
CXXFLAGS += -fdata-sections

CXXFLAGS_DEBUG   += -Og
CXXFLAGS_RELEASE += -Os

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
#     Each directory must be separated by a space.
#     Use forward slashes for directory separators.
#     For a directory that has spaces, enclose it in quotes.
EXTRALIBDIRS = 

# List any extra libraries here.
#     Each library must be separated by a space.
#     The "lib" prefix and ".a" suffix must be removed.
EXTRALIBS = 

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

include $(PX_FWLIB)/$(ARCH)/MakeRules.mk
