# ------------------------------------------------------------------------------
# px-fwlib AVR Arduino UNO multifunction shield example
# ------------------------------------------------------------------------------

PROJECT = multifunction_shield
MCU = atmega328p

# Path to root of Piconomix FW Library (relative to this Makefile)
PX_FWLIB = ../../../../..
ARCH = arch/avr
BSP = $(PX_FWLIB)/boards/avr/arduino_uno

SRC += src/main.c
SRC += $(PX_FWLIB)/devices/general/src/px_mfs.c
SRC += $(BSP)/src/px_board.c
SRC += $(PX_FWLIB)/$(ARCH)/src/px_gpio.c
SRC += $(PX_FWLIB)/$(ARCH)/src/px_uart.c
SRC += $(PX_FWLIB)/$(ARCH)/src/px_uart_stdio.c

CXXSRC +=
ASRC +=

INCDIRS += cfg
INCDIRS += $(BSP)/inc
INCDIRS += $(PX_FWLIB)
INCDIRS += $(PX_FWLIB)/$(ARCH)/inc
INCDIRS += $(PX_FWLIB)/common/inc
INCDIRS += $(PX_FWLIB)/devices/general/inc
INCDIRS += $(PX_FWLIB)/utils/inc

CDEFS +=
CXXDEFS +=
ADEFS +=

CFLAGS += -gdwarf-2
CFLAGS += -funsigned-char
CFLAGS += -funsigned-bitfields
CFLAGS += -fshort-enums
CFLAGS += -Wall
CFLAGS += -Wstrict-prototypes
CFLAGS += -std=gnu99
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections
CFLAGS_DEBUG += -Og
CFLAGS_RELEASE += -Os

CXXFLAGS += -gdwarf-2
CXXFLAGS += -funsigned-char
CXXFLAGS += -funsigned-bitfields
CXXFLAGS += -fshort-enums
CXXFLAGS += -fno-exceptions
CXXFLAGS += -Wall
CXXFLAGS += -std=gnu++11
CXXFLAGS += -ffunction-sections
CXXFLAGS += -fdata-sections
CXXFLAGS_DEBUG += -Og
CXXFLAGS_RELEASE += -Os

AFLAGS +=

PRINTF_LIB_MIN = -Wl,-u,vfprintf -lprintf_min
PRINTF_LIB = $(PRINTF_LIB_MIN)
SCANF_LIB =
MATH_LIB = -lm
EXTRALIBDIRS =
EXTRALIBS =
EXTMEMOPTS =

LDFLAGS += -Wl,-gc-sections,--relax
LDFLAGS += $(EXTMEMOPTS)
LDFLAGS += $(PRINTF_LIB) $(SCANF_LIB) $(MATH_LIB)

include $(PX_FWLIB)/$(ARCH)/MakeRules.mk
