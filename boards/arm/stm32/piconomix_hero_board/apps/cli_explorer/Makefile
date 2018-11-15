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
# make             = Build project
# make check       = Check that source files exist
# make clean       = Delete intermediate output files and final binaries
# make mostlyclean = Delete intermediate output files, but preserve final 
#                    binaries
# make openocd     = Create script and launch OpenOCD GDB and Telnet server
# make gdb         = Create script and launch GDB
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
# - sh (sh.exe on MS-Windows platform; required for 'make check')
# - rm (to delete files and directories)
#
# ------------------------------------------------------------------------------
# Creation Date:  2018-03-17
# ------------------------------------------------------------------------------

# (1) Project name
PROJECT = cli_explorer

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
SRC += src/px_cli_cmds.c
SRC += src/px_cli_cmds_adc.c
SRC += src/px_cli_cmds_gpio.c
SRC += src/px_cli_cmds_i2c.c
SRC += src/px_cli_cmds_lcd.c
SRC += src/px_cli_cmds_mem.c
SRC += src/px_cli_cmds_rtc.c
SRC += src/px_cli_cmds_sd.c
SRC += src/px_cli_cmds_sf.c
SRC += src/px_cli_cmds_spi.c
SRC += src/px_cli_cmds_uart.c
SRC += src/px_sd_fatfs_diskio.c
SRC += src/px_xmodem_glue.c

SRC += src/stm32l0xx_it.c
SRC += src/usb_device.c
SRC += src/usbd_cdc_if.c
SRC += src/usbd_conf.c
SRC += src/usbd_desc.c

SRC += $(BSP)/px_board.c

SRC += $(PICOLIB)/$(ARCH)/px_adc.c
SRC += $(PICOLIB)/$(ARCH)/px_gpio.c
SRC += $(PICOLIB)/$(ARCH)/px_i2c.c
SRC += $(PICOLIB)/$(ARCH)/px_rtc.c
SRC += $(PICOLIB)/$(ARCH)/px_spi.c
SRC += $(PICOLIB)/$(ARCH)/px_sysclk.c
SRC += $(PICOLIB)/$(ARCH)/px_uart.c
#SRC += $(PICOLIB)/$(ARCH)/px_uart_stdio.c
SRC += $(PICOLIB)/$(ARCH)/px_usb_cdc_stdio.c
SRC += $(PICOLIB)/comms/px_cli.c
SRC += $(PICOLIB)/comms/px_vt100.c
SRC += $(PICOLIB)/comms/px_xmodem.c
SRC += $(PICOLIB)/devices/display/px_lcd_st7567_jhd12864.c
SRC += $(PICOLIB)/devices/mem/px_at25s.c
SRC += $(PICOLIB)/devices/mem/px_sd.c
SRC += $(PICOLIB)/gfx/px_gfx.c
SRC += $(PICOLIB)/gfx/fonts/px_gfx_font_5x7.c
SRC += $(PICOLIB)/gfx/images/px_gfx_img_hero_logo_64x42.c
SRC += $(PICOLIB)/utils/px_circ_buffer.c
SRC += $(PICOLIB)/utils/px_dbg.c
SRC += $(PICOLIB)/utils/px_rtc_util.c
SRC += $(PICOLIB)/utils/px_systmr.c

SRC += $(STMCUBE)/Drivers/CMSIS/Device/ST/STM32L0xx/Source/Templates/system_stm32l0xx.c
SRC += $(STMCUBE)/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal.c
SRC += $(STMCUBE)/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_cortex.c
SRC += $(STMCUBE)/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_pcd.c
SRC += $(STMCUBE)/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_pcd_ex.c
SRC += $(STMCUBE)/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_utils.c

SRC += $(STMCUBE_USB)/Class/CDC/Src/usbd_cdc.c
SRC += $(STMCUBE_USB)/Core/Src/usbd_core.c
SRC += $(STMCUBE_USB)/Core/Src/usbd_ctlreq.c
SRC += $(STMCUBE_USB)/Core/Src/usbd_ioreq.c

SRC += $(CHAN_FATFS)/ff.c

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
INCDIRS += res
INCDIRS += src
INCDIRS += $(BSP)
INCDIRS += $(PICOLIB)
INCDIRS += $(PICOLIB)/$(ARCH)
INCDIRS += $(PICOLIB)/common
INCDIRS += $(PICOLIB)/comms
INCDIRS += $(PICOLIB)/devices/display
INCDIRS += $(PICOLIB)/devices/mem
INCDIRS += $(PICOLIB)/gfx
INCDIRS += $(PICOLIB)/gfx/fonts
INCDIRS += $(PICOLIB)/gfx/images
INCDIRS += $(PICOLIB)/utils
INCDIRS += $(STMCUBE)/Drivers/STM32L0xx_HAL_Driver/Inc
INCDIRS += $(STMCUBE)/Drivers/STM32L0xx_HAL_Driver/Legacy
INCDIRS += $(STMCUBE)/Drivers/CMSIS/Device/ST/STM32L0xx/Include
INCDIRS += $(STMCUBE)/Drivers/CMSIS/Include
INCDIRS += $(STMCUBE_USB)/Class/CDC/Inc
INCDIRS += $(STMCUBE_USB)/Core/Inc
INCDIRS += $(CHAN_FATFS)

# (6a) Place preprocessor DEFINE macros here for C sources (GCC option -D)
CDEFS += PX_DBG=1
CDEFS += STM32L072xx
CDEFS += USE_FULL_LL_DRIVER
#CDEFS += USE_HAL_DRIVER

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

#---------------- Compiler Options C++ -----------------------------------------
#  -g*:          generate debugging information
#  -O*:          optimization level (0,1,2,3 or s)
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
CPPFLAGS += -gdwarf-2
CPPFLAGS += -Os
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
CPPFLAGS += --specs=nano.specs

#---------------- Assembler Options --------------------------------------------
#  -Wa,...:   tell GCC to pass this to the assembler.
ASFLAGS += 

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

#---------------- Debugging Options --------------------------------------------
# OpenOCD script filename
OPENOCD_SCRIPT ?= openocd.cfg

# Specify the OpenOCD interface script
#OPENOCD_INTERFACE = interface/cmsis-dap.cfg
#OPENOCD_INTERFACE = interface/stlink-v2.cfg
OPENOCD_INTERFACE ?= interface/stlink-v2-1.cfg

# Specify the board or target
OPENOCD_TARGET ?= target/stm32l0.cfg

# GDB server port
#GDB_PORT ?= 3333
GDB_PORT ?= 2331

# GDB configuration script filename
GDB_SCRIPT ?= init.gdb

#===============================================================================

include $(PICOLIB)/$(ARCH)/MakeRules.mk
