# ------------------------------------------------------------------------------
# STM32 Makefile Template by Pieter Conradie <https://piconomix.com>
#
# Based on:
# - WinAVR Makefile Template written by Eric B. Weddington, Jörg Wunsch, et al.
# - WinARM template makefile by Martin Thomas, Kaiserslautern, Germany 
# - LUFA Makefiles by Dean Camera [www.lufa-lib.org]
# - http://stackoverflow.com Q&A research
#
# On command line:
#
# make                    = Build project (release version)
# make build=debug        = Build project (debug version)
# make build=release      = Build project (release version)
# make build=release-boot = Build project (release for bootloader version)
# make check              = Check that source files exist
# make clean              = Delete intermediate output files and final binaries
# make mostlyclean        = Delete intermediate output files, but preserve final 
#                           binaries
# make openocd            = Create script and launch OpenOCD GDB and Telnet server
# make gdb                = Create script and launch GDB
#
# To create a new project, copy this Makefile to your project directory and:
#
# 1. Specify a project name and offset to reserve space for bootloader
# 2. Specify the MCU options that the project will be compiled for
# 3. Specify paths
# 4. List source files to be included in project WITH PATHS (relative to Makefile)
# 5. Add include paths
# 6. Specify any extra preprocessor definitions for source files
#
# Makefile script special requirements:
# - make 3.81 or greater (requires working eval function)
# - sh (sh.exe on MS-Windows platform; required for 'make check')
# - rm (to delete files and directories)
#
# ------------------------------------------------------------------------------
# Creation Date:  2018-03-17
# ------------------------------------------------------------------------------

# (1a) Project name
PROJECT = bootloader_usb_uf2

# (1b) Offset from start of FLASH to reserve space for bootloader (release for bootloader build)
#BOOTLOADER_SIZE = 0x4000

# (2) MCU option(s)
MCU = -mthumb -mcpu=cortex-m0plus

# (3a) Path to root of Piconomix FW Library (relative to Makefile)
PICOLIB = ../../../../../..

# (3b) Path to microcontroller architecture (relative to Piconomix FW Library)
ARCH = arch/arm/stm32

# (3c) Path to board support package (relative to Makefile)
BSP = $(PICOLIB)/boards/arm/stm32/piconomix_hero_board

# (3d) Path to ST STMCube library (relative to Makefile)
STMCUBE = $(PICOLIB)/libs/STM32Cube/L0

# (3d) Path to ST STMCube USB library (relative to Makefile)
STMCUBE_USB = $(PICOLIB)/libs/STM32Cube_USB

# (3e) Path to ChaN's FatFs library (relative to Makefile)
CHAN_FATFS = $(PICOLIB)/libs/ChaN_FatFs

# (4a) List C source files WITH PATHS (relative to Makefile) here
SRC += src/main.c
SRC += src/stm32l0xx_it.c
SRC += src/usb_device.c
SRC += src/usbd_conf.c
SRC += src/usbd_desc.c
SRC += src/usbd_storage_if.c
SRC += $(BSP)/px_board.c
SRC += $(PICOLIB)/$(ARCH)/px_flash.c
SRC += $(PICOLIB)/$(ARCH)/px_gpio.c
SRC += $(PICOLIB)/$(ARCH)/px_sysclk.c
SRC += $(PICOLIB)/$(ARCH)/px_uart.c
SRC += $(PICOLIB)/comms/px_uf2.c
SRC += $(PICOLIB)/utils/px_circ_buffer.c
SRC += $(PICOLIB)/utils/px_dbg.c
SRC += $(PICOLIB)/utils/px_systmr.c
SRC += $(STMCUBE)/Drivers/CMSIS/Device/ST/STM32L0xx/Source/Templates/system_stm32l0xx.c
SRC += $(STMCUBE)/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal.c
SRC += $(STMCUBE)/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_cortex.c
SRC += $(STMCUBE)/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_pcd.c
SRC += $(STMCUBE)/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_pcd_ex.c
SRC += $(STMCUBE)/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_usb.c
SRC += $(STMCUBE)/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_utils.c
SRC += $(STMCUBE_USB)/Class/MSC/Src/usbd_msc.c
SRC += $(STMCUBE_USB)/Class/MSC/Src/usbd_msc_bot.c
SRC += $(STMCUBE_USB)/Class/MSC/Src/usbd_msc_data.c
SRC += $(STMCUBE_USB)/Class/MSC/Src/usbd_msc_scsi.c
SRC += $(STMCUBE_USB)/Core/Src/usbd_core.c
SRC += $(STMCUBE_USB)/Core/Src/usbd_ctlreq.c
SRC += $(STMCUBE_USB)/Core/Src/usbd_ioreq.c

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
ASRC += $(STMCUBE)/Drivers/CMSIS/Device/ST/STM32L0xx/Source/Templates/gcc/startup_stm32l072xx.s

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
INCDIRS += src
INCDIRS += $(BSP)
INCDIRS += $(PICOLIB)
INCDIRS += $(PICOLIB)/$(ARCH)
INCDIRS += $(PICOLIB)/common
INCDIRS += $(PICOLIB)/comms
INCDIRS += $(PICOLIB)/data
INCDIRS += $(PICOLIB)/devices/comms
INCDIRS += $(PICOLIB)/devices/display
INCDIRS += $(PICOLIB)/devices/general
INCDIRS += $(PICOLIB)/devices/mem
INCDIRS += $(PICOLIB)/devices/rtc
INCDIRS += $(PICOLIB)/devices/sensor
INCDIRS += $(PICOLIB)/gfx
INCDIRS += $(PICOLIB)/gfx/fonts
INCDIRS += $(PICOLIB)/gfx/images
INCDIRS += $(PICOLIB)/utils
INCDIRS += $(CHAN_FATFS)
INCDIRS += $(STMCUBE)/Drivers/CMSIS/Device/ST/STM32L0xx/Include
INCDIRS += $(STMCUBE)/Drivers/CMSIS/Include
INCDIRS += $(STMCUBE)/Drivers/STM32L0xx_HAL_Driver/Inc
INCDIRS += $(STMCUBE)/Drivers/STM32L0xx_HAL_Driver/Legacy
INCDIRS += $(STMCUBE_USB)/Class/MSC/Inc
INCDIRS += $(STMCUBE_USB)/Core/Inc

# (6a) Place preprocessor DEFINE macros here for C sources (GCC option -D)
CDEFS += PX_DBG=0
CDEFS += STM32L072xx
CDEFS += USE_FULL_LL_DRIVER
#CDEFS += USE_HAL_DRIVER
#CDEFS += BOOT_CLEAR_REST_OF_FLASH

# (6b) Place preprocessor DEFINE macros here for C++ sources (GCC option -D)
#CPPDEFS += __STDC_LIMIT_MACROS
#CPPDEFS += __STDC_CONSTANT_MACROS
CPPDEFS += 

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
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += --specs=nano.specs

CFLAGS_DEBUG += -Og

CFLAGS_RELEASE += -Os

#---------------- Compiler Options C++ -----------------------------------------
#  -g*:          generate debugging information
#  -O*:          optimization level (0,1,2,3 or s)
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
CPPFLAGS += -gdwarf-2
CPPFLAGS += -funsigned-char
CPPFLAGS += -funsigned-bitfields
CPPFLAGS += -fshort-enums
CPPFLAGS += -fno-exceptions
CPPFLAGS += -Wall
#CPPFLAGS += -Wundef
#CPPFLAGS += -mshort-calls
#CPPFLAGS += -fno-unit-at-a-time
#CPPFLAGS += -Wstrict-prototypes
#CPPFLAGS += -Wunreachable-code
#CPPFLAGS += -Wsign-compare
CPPFLAGS += -std=gnu99
CPPFLAGS += -ffunction-sections -fdata-sections
CPPFLAGS += --specs=nano.specs

CPPFLAGS_DEBUG += -Og

CPPFLAGS_RELEASE += -Os

#---------------- Assembler Options --------------------------------------------
#  -Wa,...:   tell GCC to pass this to the assembler.
AFLAGS += 

#---------------- Library Options ----------------------------------------------
# Floating point printf version
PRINTF_LIB_FLOAT = -Wl,-u,_printf_float

# If this is left blank, then it will use the Standard printf version.
PRINTF_LIB = 
#PRINTF_LIB = $(PRINTF_LIB_FLOAT)

# Floating point + %[ scanf version (requires MATH_LIB = -lm below)
SCANF_LIB_FLOAT = -Wl,-u,_scanf_float

# If this is left blank, then it will use the Standard scanf version.
SCANF_LIB = 
#SCANF_LIB = $(SCANF_LIB_FLOAT)

# List any extra directories to look for libraries here.
#     Each directory must be seperated by a space.
#     Use forward slashes for directory separators.
#     For a directory that has spaces, enclose it in quotes.
EXTRALIBDIRS = 

#---------------- Linker Options -----------------------------------------------
#  -Wl,...:     tell GCC to pass this to linker.
LDFLAGS += -Wl,-gc-sections,--relax
LDFLAGS += --specs=nosys.specs
LDFLAGS += --specs=nano.specs
LDFLAGS += $(PRINTF_LIB) $(SCANF_LIB)
LDFLAGS += -T $(BSP)/STM32L072xB.ld

LDFLAGS_DEBUG += -Og

LDFLAGS_RELEASE += -Os

#---------------- Debugging Options --------------------------------------------
# OpenOCD script filename
OPENOCD_SCRIPT ?= openocd.cfg

# Specify the OpenOCD interface script
OPENOCD_INTERFACE = interface/cmsis-dap.cfg

# Specify the board or target
OPENOCD_TARGET ?= target/stm32l0.cfg

# GDB server port
#GDB_PORT ?= 3333
GDB_PORT ?= 2331

# GDB configuration script filename
GDB_SCRIPT ?= init.gdb

#===============================================================================

include $(PICOLIB)/$(ARCH)/MakeRules.mk
