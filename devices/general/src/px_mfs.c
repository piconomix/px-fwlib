/* =============================================================================
     ____    ___    ____    ___    _   _    ___  __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \ |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | || |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| || |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/ |_|  |_| |___| /_/\_\

    Copyright (c) 2026 Randall Young

    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          px_mfs.c : Arduino multifunction shield driver
    Author(s):      Randall Young
    Creation Date:  2026-08-22

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <avr/io.h>
#include <string.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_mfs.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
/*
 *  The standard shield display is a common-anode, active-low segment display.
 *  Bit 0..6 are A..G and bit 7 is the decimal point.
 */
static const uint8_t px_mfs_digit_map[10] =
{
    0xc0, 0xf9, 0xa4, 0xb0, 0x99,
    0x92, 0x82, 0xf8, 0x80, 0x90,
};

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */
static uint8_t px_mfs_encode_char(char ch);
static void    px_mfs_shift_out(const px_mfs_cfg_t * cfg, uint8_t data);
static void    px_mfs_shift_pair(const px_mfs_cfg_t * cfg, uint8_t digit, uint8_t segment);

/* _____LOCAL FUNCTIONS______________________________________________________ */
static uint8_t px_mfs_encode_char(char ch)
{
    if((ch >= 'a') && (ch <= 'z'))
    {
        ch = (char)(ch - 'a' + 'A');
    }

    if((ch >= '0') && (ch <= '9'))
    {
        return px_mfs_digit_map[(uint8_t)(ch - '0')];
    }

    switch(ch)
    {
    case 'A': return 0x88;
    case 'B': return 0x83;
    case 'C': return 0xc6;
    case 'D': return 0xa1;
    case 'E': return 0x86;
    case 'F': return 0x8e;
    case 'G': return 0xc2;
    case 'H': return 0x89;
    case 'I': return 0xcf;
    case 'J': return 0xe1;
    case 'L': return 0xc7;
    case 'N': return 0xab;
    case 'O': return 0xc0;
    case 'P': return 0x8c;
    case 'R': return 0xaf;
    case 'S': return 0x92;
    case 'T': return 0x87;
    case 'U': return 0xc1;
    case '-': return 0xbf;
    case '_': return 0xf7;
    case '=': return 0xb7;
    case ' ': return 0xff;
    default:  return 0xff;
    }
}

static void px_mfs_shift_out(const px_mfs_cfg_t * cfg, uint8_t data)
{
    uint8_t bit;

    for(bit = 0x80; bit != 0; bit >>= 1)
    {
        if((data & bit) != 0)
        {
            px_gpio_out_set_hi(&cfg->data_pin);
        }
        else
        {
            px_gpio_out_set_lo(&cfg->data_pin);
        }
        px_gpio_out_set_hi(&cfg->clock_pin);
        px_gpio_out_set_lo(&cfg->clock_pin);
    }
}

static void px_mfs_shift_pair(const px_mfs_cfg_t * cfg, uint8_t digit, uint8_t segment)
{
    px_gpio_out_set_lo(&cfg->latch_pin);
    if(cfg->display_digit_first)
    {
        px_mfs_shift_out(cfg, digit);
        px_mfs_shift_out(cfg, segment);
    }
    else
    {
        px_mfs_shift_out(cfg, segment);
        px_mfs_shift_out(cfg, digit);
    }
    px_gpio_out_set_hi(&cfg->latch_pin);
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_mfs_init(px_mfs_t * mfs, const px_mfs_cfg_t * cfg)
{
    uint8_t i;

    mfs->cfg   = cfg;
    mfs->digit = 0;

    px_gpio_init(&cfg->data_pin);
    px_gpio_init(&cfg->clock_pin);
    px_gpio_init(&cfg->latch_pin);
    px_gpio_init(&cfg->buzzer_pin);
    px_gpio_init(&cfg->ldr_pin);
    px_gpio_init(&cfg->sensor_pin);
    px_gpio_init(&cfg->ir_pin);

    for(i = 0; i < PX_MFS_LED_COUNT; i++)
    {
        px_gpio_init(&cfg->led[i]);
    }
    for(i = 0; i < PX_MFS_BUTTON_COUNT; i++)
    {
        px_gpio_init(&cfg->button[i]);
    }

    // Keep the display and all outputs inactive until the first refresh.
    px_gpio_out_set_lo(&cfg->latch_pin);
    px_mfs_display_clear(mfs);
    px_mfs_leds_set(mfs, 0);
    px_mfs_buzzer_set(mfs, false);

    // Select the configured voltage reference and disable the ADC for now.
    ADMUX  = cfg->adc_reference;
    ADCSRA = 0;
}

void px_mfs_display_refresh(px_mfs_t * mfs)
{
    const px_mfs_cfg_t * cfg = mfs->cfg;
    uint8_t               digit = mfs->digit;

    // Turn every digit off while changing the shift-register contents. This
    // prevents visible ghosting on shields with slower 74HC595 variants.
    px_mfs_shift_pair(cfg, cfg->display_blank, cfg->display_blank);
    px_mfs_shift_pair(cfg, cfg->digit_select[digit], mfs->display[digit]);

    if(++mfs->digit == PX_MFS_DISPLAY_DIGITS)
    {
        mfs->digit = 0;
    }
}

void px_mfs_display_clear(px_mfs_t * mfs)
{
    uint8_t i;

    for(i = 0; i < PX_MFS_DISPLAY_DIGITS; i++)
    {
        mfs->display[i] = mfs->cfg->display_blank;
    }
}

void px_mfs_display_set_raw(px_mfs_t * mfs, const uint8_t segment[PX_MFS_DISPLAY_DIGITS])
{
    memcpy(mfs->display, segment, PX_MFS_DISPLAY_DIGITS);
}

void px_mfs_display_set_char(px_mfs_t * mfs, uint8_t position, char ch, bool decimal_point)
{
    if(position >= PX_MFS_DISPLAY_DIGITS)
    {
        return;
    }

    mfs->display[position] = px_mfs_encode_char(ch);
    px_mfs_display_set_decimal_point(mfs, position, decimal_point);
}

void px_mfs_display_set_text(px_mfs_t * mfs, const char * text)
{
    uint8_t position = 0;

    px_mfs_display_clear(mfs);
    while((*text != '\0') && (position < PX_MFS_DISPLAY_DIGITS))
    {
        if(*text == '.')
        {
            if(position != 0)
            {
                mfs->display[position - 1] &= (uint8_t)~PX_MFS_SEGMENT_DP;
            }
        }
        else
        {
            mfs->display[position++] = px_mfs_encode_char(*text);
        }
        text++;
    }
}

void px_mfs_display_set_u16(px_mfs_t * mfs, uint16_t value, bool leading_zero)
{
    uint8_t i;

    if(value > 9999)
    {
        px_mfs_display_set_text(mfs, "----");
        return;
    }

    px_mfs_display_clear(mfs);
    for(i = PX_MFS_DISPLAY_DIGITS; i != 0; i--)
    {
        mfs->display[i - 1] = px_mfs_digit_map[value % 10];
        value /= 10;
        if((value == 0) && !leading_zero)
        {
            break;
        }
    }
}

void px_mfs_display_set_i16(px_mfs_t * mfs, int16_t value, bool leading_zero)
{
    int32_t magnitude;
    uint8_t i;

    if(value >= 0)
    {
        px_mfs_display_set_u16(mfs, (uint16_t)value, leading_zero);
        return;
    }

    magnitude = -(int32_t)value;
    if(magnitude > 999)
    {
        px_mfs_display_set_text(mfs, "----");
        return;
    }

    px_mfs_display_clear(mfs);
    for(i = PX_MFS_DISPLAY_DIGITS; i > 1; i--)
    {
        mfs->display[i - 1] = px_mfs_digit_map[magnitude % 10];
        magnitude /= 10;
        if((magnitude == 0) && !leading_zero)
        {
            break;
        }
    }
    mfs->display[0] = px_mfs_encode_char('-');
}

void px_mfs_display_set_decimal_point(px_mfs_t * mfs, uint8_t position, bool enabled)
{
    if(position >= PX_MFS_DISPLAY_DIGITS)
    {
        return;
    }

    if(enabled)
    {
        mfs->display[position] &= (uint8_t)~PX_MFS_SEGMENT_DP;
    }
    else
    {
        mfs->display[position] |= PX_MFS_SEGMENT_DP;
    }
}

void px_mfs_led_set(px_mfs_t * mfs, px_mfs_led_t led, bool on)
{
    bool output_high;

    if((uint8_t)led >= PX_MFS_LED_COUNT)
    {
        return;
    }

    output_high = mfs->cfg->led_active_low ? !on : on;
    if(output_high)
    {
        px_gpio_out_set_hi(&mfs->cfg->led[led]);
    }
    else
    {
        px_gpio_out_set_lo(&mfs->cfg->led[led]);
    }
}

void px_mfs_leds_set(px_mfs_t * mfs, uint8_t mask)
{
    uint8_t i;

    for(i = 0; i < PX_MFS_LED_COUNT; i++)
    {
        px_mfs_led_set(mfs, (px_mfs_led_t)i, (mask & (1u << i)) != 0);
    }
}

void px_mfs_buzzer_set(px_mfs_t * mfs, bool on)
{
    bool output_high = mfs->cfg->buzzer_active_low ? !on : on;

    if(output_high)
    {
        px_gpio_out_set_hi(&mfs->cfg->buzzer_pin);
    }
    else
    {
        px_gpio_out_set_lo(&mfs->cfg->buzzer_pin);
    }
}

bool px_mfs_button_is_pressed(const px_mfs_t * mfs, px_mfs_button_t button)
{
    bool input_high;

    if((uint8_t)button >= PX_MFS_BUTTON_COUNT)
    {
        return false;
    }

    input_high = px_gpio_in_is_hi(&mfs->cfg->button[button]);
    return mfs->cfg->button_active_low ? !input_high : input_high;
}

bool px_mfs_ir_is_active(const px_mfs_t * mfs)
{
    bool input_high = px_gpio_in_is_hi(&mfs->cfg->ir_pin);

    return mfs->cfg->ir_active_low ? !input_high : input_high;
}

uint16_t px_mfs_adc_sample(const px_mfs_t * mfs, uint8_t channel)
{
    uint16_t value;

    if(channel > 15)
    {
        return 0;
    }

    ADMUX  = (uint8_t)(mfs->cfg->adc_reference | (channel & 0x0f));
    ADCSRA = (uint8_t)((1u << ADEN) | mfs->cfg->adc_prescaler);
    PX_BIT_SET_HI(ADCSRA, ADSC);
    while(PX_BIT_IS_HI(ADCSRA, ADSC))
    {
        ;
    }
    value = ADC;

    ADCSRA = 0;
    ADMUX  = mfs->cfg->adc_reference;
    return value;
}

uint16_t px_mfs_read_ldr(const px_mfs_t * mfs)
{
    return px_mfs_adc_sample(mfs, mfs->cfg->ldr_adc_channel);
}

uint16_t px_mfs_read_sensor(const px_mfs_t * mfs)
{
    return px_mfs_adc_sample(mfs, mfs->cfg->sensor_adc_channel);
}
