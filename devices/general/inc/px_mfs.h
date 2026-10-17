#ifndef __PX_MFS_H__
#define __PX_MFS_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___  __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \ |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | || |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| || |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/ |_|  |_| |___| /_/\_\

    Copyright (c) 2026 Randall Young

    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          px_mfs.h : Arduino multifunction shield driver
    Author(s):      Randall Young
    Creation Date:  2026-08-22

============================================================================= */

/**
 *  @ingroup DEVICES_GENERAL
 *  @defgroup PX_MFS px_mfs.h : Arduino multifunction shield driver
 *
 *  Driver for the common Arduino UNO multifunction shield. The shield uses
 *  two cascaded 74HC595 devices to multiplex a four-digit, common-anode
 *  seven-segment display.
 *
 *  The driver deliberately receives its GPIO mapping at run time. This keeps
 *  the driver reusable with any AVR board that exposes the shield signals,
 *  while the example supplies the normal Arduino UNO mapping.
 *
 *  The A4 sensor connector is exposed as a raw ADC input. A DS18B20 connected
 *  there can instead be used with the library's existing px_one_wire and
 *  px_ds18b20 drivers; do not sample A4 as an ADC while using 1-Wire.
 *
 *  File(s):
 *  - devices/general/inc/px_mfs.h
 *  - devices/general/src/px_mfs.c
 *
 *  @{
 */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"
#include "px_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
#define PX_MFS_DISPLAY_DIGITS 4
#define PX_MFS_LED_COUNT      4
#define PX_MFS_BUTTON_COUNT   3

/// Segment bit used for the decimal point in the raw display byte.
#define PX_MFS_SEGMENT_DP     (1u << 7)

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Shield LED selector. LED D1 is the bit nearest the display in the example.
typedef enum
{
    PX_MFS_LED_D1 = 0,
    PX_MFS_LED_D2,
    PX_MFS_LED_D3,
    PX_MFS_LED_D4,
} px_mfs_led_t;

/// Shield push-button selector.
typedef enum
{
    PX_MFS_BUTTON_S1 = 0,
    PX_MFS_BUTTON_S2,
    PX_MFS_BUTTON_S3,
} px_mfs_button_t;

/**
 *  Hardware mapping and electrical polarity configuration.
 *
 *  The display bytes are the bytes that are sent to the 74HC595 outputs. For
 *  the usual shield these are active-low segment bytes and digit-select bytes
 *  {0xF1, 0xF2, 0xF4, 0xF8}. The classic shield sends the segment byte first,
 *  followed by the digit-select byte.
 */
typedef struct
{
    px_gpio_handle_t data_pin;       ///< 74HC595 SER, normally Arduino D8
    px_gpio_handle_t clock_pin;      ///< 74HC595 SRCLK, normally Arduino D7
    px_gpio_handle_t latch_pin;      ///< 74HC595 RCLK, normally Arduino D4

    px_gpio_handle_t led[PX_MFS_LED_COUNT];
    px_gpio_handle_t buzzer_pin;     ///< Normally Arduino D3
    px_gpio_handle_t button[PX_MFS_BUTTON_COUNT];
    px_gpio_handle_t ldr_pin;        ///< Potentiometer / LDR input, normally A0
    px_gpio_handle_t sensor_pin;     ///< Optional LM35/DS18B20 connector, A4
    px_gpio_handle_t ir_pin;         ///< Optional IR receiver input, normally D2

    uint8_t ldr_adc_channel;
    uint8_t sensor_adc_channel;
    uint8_t adc_reference;           ///< ADMUX reference bits, e.g. (1 << REFS0)
    uint8_t adc_prescaler;           ///< ADCSRA ADPS bits, e.g. (7 << ADPS0)

    uint8_t digit_select[PX_MFS_DISPLAY_DIGITS];
    uint8_t display_blank;            ///< Byte that disables all segments/digits
    bool    display_digit_first;      ///< false for the classic shield wiring
    bool    led_active_low;           ///< Usual shield LEDs are active low
    bool    buzzer_active_low;        ///< Standard shield buzzer is active low
    bool    button_active_low;        ///< Buttons normally use pull-ups
    bool    ir_active_low;
} px_mfs_cfg_t;

/// Driver state. Keep one object per shield.
typedef struct
{
    const px_mfs_cfg_t * cfg;
    uint8_t               display[PX_MFS_DISPLAY_DIGITS];
    uint8_t               digit;
} px_mfs_t;

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/// Initialise GPIO, ADC and display state.
void px_mfs_init(px_mfs_t * mfs, const px_mfs_cfg_t * cfg);

/// Shift one display digit to the shield. Call regularly, typically every 1 ms.
void px_mfs_display_refresh(px_mfs_t * mfs);

/// Blank all four display positions.
void px_mfs_display_clear(px_mfs_t * mfs);

/// Set four raw segment bytes.
void px_mfs_display_set_raw(px_mfs_t * mfs, const uint8_t segment[PX_MFS_DISPLAY_DIGITS]);

/// Set one display position to an ASCII character.
void px_mfs_display_set_char(px_mfs_t * mfs, uint8_t position, char ch, bool decimal_point);

/// Set up to four ASCII characters; a period adds a decimal point to the prior character.
void px_mfs_display_set_text(px_mfs_t * mfs, const char * text);

/// Display an unsigned value in the range 0..9999.
void px_mfs_display_set_u16(px_mfs_t * mfs, uint16_t value, bool leading_zero);

/// Display a signed value in the range -999..9999.
void px_mfs_display_set_i16(px_mfs_t * mfs, int16_t value, bool leading_zero);

/// Set or clear the decimal point on one display position.
void px_mfs_display_set_decimal_point(px_mfs_t * mfs, uint8_t position, bool enabled);

/// Set one LED on or off. The API uses logical on/off regardless of polarity.
void px_mfs_led_set(px_mfs_t * mfs, px_mfs_led_t led, bool on);

/// Set all four LEDs from a logical bit mask, bit 0 = D1.
void px_mfs_leds_set(px_mfs_t * mfs, uint8_t mask);

/// Set the buzzer output. A timed beep can be built by polling an application timer.
void px_mfs_buzzer_set(px_mfs_t * mfs, bool on);

/// Read one of the three buttons. Debouncing is intentionally application-owned.
bool px_mfs_button_is_pressed(const px_mfs_t * mfs, px_mfs_button_t button);

/// Read the optional IR receiver input.
bool px_mfs_ir_is_active(const px_mfs_t * mfs);

/// Take one blocking 10-bit ADC sample from an AVR ADC channel.
uint16_t px_mfs_adc_sample(const px_mfs_t * mfs, uint8_t channel);

/// Take one sample from the shield's A0 potentiometer/LDR input.
uint16_t px_mfs_read_ldr(const px_mfs_t * mfs);

/// Take one sample from the shield's optional A4 sensor input.
uint16_t px_mfs_read_sensor(const px_mfs_t * mfs);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
