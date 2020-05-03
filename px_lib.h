#ifndef __PX_LIB_H__
#define __PX_LIB_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2020 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
 
    Title:          px_lib.h : Top level include file for whole library
    Author(s):      Pieter Conradie
    Creation Date:  2020-05-01

============================================================================= */

#include "px_lib_cfg.h"

#ifdef PX_ARCH_ARM_STM32_CFG
#include "arch/arm/stm32/inc/px_adc.h"
#include "arch/arm/stm32/inc/px_dac.h"
#include "arch/arm/stm32/inc/px_eeprom.h"
#include "arch/arm/stm32/inc/px_exti.h"
#include "arch/arm/stm32/inc/px_flash.h"
#include "arch/arm/stm32/inc/px_gpio.h"
#include "arch/arm/stm32/inc/px_i2c.h"
#include "arch/arm/stm32/inc/px_rtc.h"
#include "arch/arm/stm32/inc/px_spi.h"
#include "arch/arm/stm32/inc/px_sysclk.h"
#include "arch/arm/stm32/inc/px_uart.h"
#include "arch/arm/stm32/inc/px_uart_stdio.h"
#include "arch/arm/stm32/inc/px_usb_cdc_stdio.h"
#endif

#ifdef PX_ARCH_AVR_CFG
#include "arch/avr/inc/px_async_tmr.h"
#include "arch/avr/inc/px_flash.h"
#include "arch/avr/inc/px_gpio.h"
#include "arch/avr/inc/px_gpio_init_atmega328p.h"
#include "arch/avr/inc/px_i2c.h"
#include "arch/avr/inc/px_spi.h"
#include "arch/avr/inc/px_sysclk.h"
#include "arch/avr/inc/px_uart.h"
#include "arch/avr/inc/px_uart_defs.h"
#include "arch/avr/inc/px_uart_stdio.h"
#endif

#include "common/inc/px_compiler.h"
#include "common/inc/px_defines.h"
#include "common/inc/px_pgm_P.h"

#include "comms/inc/px_cli.h"
#include "comms/inc/px_hdlc.h"
#include "comms/inc/px_nmea.h"
#include "comms/inc/px_one_wire.h"
#include "comms/inc/px_uf2.h"
#include "comms/inc/px_uf2_defs.h"
#include "comms/inc/px_vt100.h"
#include "comms/inc/px_xmodem.h"

#include "data/inc/px_log_fs.h"
#include "data/inc/px_log_fs_glue.h"

#include "devices/comms/inc/px_nrf905.h"
#include "devices/display/inc/px_lcd_st7565p_tse2g0330e.h"
#include "devices/display/inc/px_lcd_st7567_jhd12864.h"
#include "devices/general/inc/px_kbd_matrix.h"
#include "devices/mem/inc/px_at25s.h"
#include "devices/mem/inc/px_at45d.h"
#include "devices/mem/inc/px_cat25m.h"
#include "devices/mem/inc/px_eeprom_24cxx.h"
#include "devices/mem/inc/px_sd.h"
#include "devices/rtc/inc/px_m41t00.h"
#include "devices/sensor/inc/px_bme280.h"
#include "devices/sensor/inc/px_bmp280.h"
#include "devices/sensor/inc/px_c1098.h"
#include "devices/sensor/inc/px_ds18b20.h"
#include "devices/sensor/inc/px_gt511.h"
#include "devices/sensor/inc/px_veml6075.h"

#include "gfx/fonts/inc/px_gfx_fonts.h"
#include "gfx/fonts/inc/px_gfx_font_11x14.h"
#include "gfx/fonts/inc/px_gfx_font_3x5.h"
#include "gfx/fonts/inc/px_gfx_font_5x7.h"
#include "gfx/images/inc/px_gfx_img_hero_logo.h"
#include "gfx/inc/px_gfx.h"
#include "gfx/inc/px_gfx_display.h"
#include "gfx/inc/px_gfx_obj.h"
#include "gfx/inc/px_gfx_objects.h"
#include "gfx/inc/px_gfx_obj_graph.h"
#include "gfx/inc/px_gfx_obj_label.h"
#include "gfx/inc/px_gfx_obj_window.h"

#include "utils/inc/px_crc16.h"
#include "utils/inc/px_crc32.h"
#include "utils/inc/px_dbg.h"
#include "utils/inc/px_debounce.h"
#include "utils/inc/px_link_list.h"
#include "utils/inc/px_queue.h"
#include "utils/inc/px_ring_buf.h"
#include "utils/inc/px_ring_buf_macros.h"
#include "utils/inc/px_rtc_util.h"
#include "utils/inc/px_systmr.h"
#include "utils/inc/px_xtea.h"

#endif // #ifndef __PX_LIB_H__
