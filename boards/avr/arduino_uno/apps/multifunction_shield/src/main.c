/* =============================================================================
     ____    ___    ____    ___    _   _    ___  __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \ |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | || |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| || |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/ |_|  |_| |___| /_/\_\

    Copyright (c) 2026 Randall Young

    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          main.c : Multifunction shield AVR test example
    Author(s):      Randall Young
    Creation Date:  2026-08-22

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdio.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_mfs.h"
#include "px_board.h"
#include "px_uart.h"
#include "px_uart_stdio.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
#define APP_DEBOUNCE_TICKS 3
#define APP_LONG_PRESS_TICKS 100
#define APP_ADC_BIT_PERIOD_MS 500
#define APP_ADC_ONE_BEEP_MS 180
#define APP_ADC_ZERO_BEEP_MS 100
#define APP_ADC_ONE_TONE_HALF_PERIOD_US 250
#define APP_ADC_ZERO_TONE_HALF_PERIOD_US 500
#define APP_ADC_START_FLASH_MS 220
#define APP_ADC_END_PAUSE_MS 1000
#define APP_ADC_END_BEEP_MS 180
#define APP_ADC_END_GAP_MS 140
#define APP_ADC_DONE_TONE_HALF_PERIOD_US 333
#define APP_ADC_DONE_PAIR_GAP_MS 320

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static px_mfs_t         mfs;
static px_uart_handle_t uart;

static const px_mfs_cfg_t mfs_cfg =
{
    .data_pin  = {PX_GPIO(B, 0, PX_GPIO_DIR_OUT, PX_GPIO_INIT_LO)},
    .clock_pin = {PX_GPIO(D, 7, PX_GPIO_DIR_OUT, PX_GPIO_INIT_LO)},
    .latch_pin = {PX_GPIO(D, 4, PX_GPIO_DIR_OUT, PX_GPIO_INIT_LO)},

    // D1..D4 are wired to D13..D10 and are active low.
    .led =
    {
        {PX_GPIO(B, 5, PX_GPIO_DIR_OUT, PX_GPIO_INIT_HI)},
        {PX_GPIO(B, 4, PX_GPIO_DIR_OUT, PX_GPIO_INIT_HI)},
        {PX_GPIO(B, 3, PX_GPIO_DIR_OUT, PX_GPIO_INIT_HI)},
        {PX_GPIO(B, 2, PX_GPIO_DIR_OUT, PX_GPIO_INIT_HI)},
    },
    .buzzer_pin = {PX_GPIO(D, 3, PX_GPIO_DIR_OUT, PX_GPIO_INIT_HI)},

    .button =
    {
        {PX_GPIO(C, 1, PX_GPIO_DIR_IN, PX_GPIO_INIT_PULL_UP)},
        {PX_GPIO(C, 2, PX_GPIO_DIR_IN, PX_GPIO_INIT_PULL_UP)},
        {PX_GPIO(C, 3, PX_GPIO_DIR_IN, PX_GPIO_INIT_PULL_UP)},
    },
    .ldr_pin    = {PX_GPIO(C, 0, PX_GPIO_DIR_IN, PX_GPIO_INIT_HIZ)},
    .sensor_pin = {PX_GPIO(C, 4, PX_GPIO_DIR_IN, PX_GPIO_INIT_HIZ)},
    .ir_pin     = {PX_GPIO(D, 2, PX_GPIO_DIR_IN, PX_GPIO_INIT_PULL_UP)},

    .ldr_adc_channel    = 0,
    .sensor_adc_channel = 4,
    .adc_reference      = (1 << REFS0),
    .adc_prescaler      = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0),

    // Standard shield digit order: D1, D2, D3, D4, left to right.
    .digit_select       = {0xf1, 0xf2, 0xf4, 0xf8},
    .display_blank      = 0xff,
    .display_digit_first = false,
    .led_active_low     = true,
    .buzzer_active_low  = true,
    .button_active_low  = true,
    .ir_active_low      = true,
};

static uint8_t  led_mask;
static uint8_t  display_page;
static uint16_t ldr_value;
static uint16_t sensor_value;
static uint16_t app_ms;
static uint16_t beep_remaining_ms;
static bool     button_candidate[PX_MFS_BUTTON_COUNT];
static bool     button_state[PX_MFS_BUTTON_COUNT];
static uint8_t  button_counter[PX_MFS_BUTTON_COUNT];
static uint8_t  button_hold_ticks[PX_MFS_BUTTON_COUNT];
static bool     button_long_fired[PX_MFS_BUTTON_COUNT];

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */
static void app_init(void);
static void app_delay_ms(uint16_t delay_ms);
static void app_play_tone_ms(uint16_t duration_ms, uint16_t half_period_us);
static void app_set_hex_display(uint16_t value, uint8_t decimal_point_mask);
static void app_run_adc_show(uint16_t value);
static void app_startup_test(void);
static void app_button_pressed(uint8_t button);
static void app_button_long_pressed(uint8_t button);
static void app_poll_buttons(void);
static void app_update_display(void);

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void app_init(void)
{
    px_board_init();

    px_uart_init();
    px_uart_open2(&uart,
                  PX_UART_NR_0,
                  115200,
                  PX_UART_DATA_BITS_8,
                  PX_UART_PARITY_NONE,
                  PX_UART_STOP_BITS_1);
    px_uart_stdio_init(&uart);

    px_mfs_init(&mfs, &mfs_cfg);
    px_interrupts_enable();
}

static void app_delay_ms(uint16_t delay_ms)
{
    while(delay_ms-- != 0)
    {
        px_mfs_display_refresh(&mfs);
        px_board_delay_ms(1);
    }
}

static void app_play_tone_ms(uint16_t duration_ms, uint16_t half_period_us)
{
    uint16_t cycles;

    /* A complete high/low cycle is two half-periods. */
    cycles = (uint16_t)(((uint32_t)duration_ms * 1000u)
                        / (2u * half_period_us));
    while(cycles-- != 0)
    {
        px_mfs_buzzer_set(&mfs, true);
        px_board_delay_us(half_period_us);
        px_mfs_display_refresh(&mfs);
        px_mfs_buzzer_set(&mfs, false);
        px_board_delay_us(half_period_us);
        px_mfs_display_refresh(&mfs);
    }
    px_mfs_buzzer_set(&mfs, false);
}

static void app_set_hex_display(uint16_t value, uint8_t decimal_point_mask)
{
    uint8_t position;

    for(position = 0; position < PX_MFS_DISPLAY_DIGITS; position++)
    {
        uint8_t shift = (uint8_t)((PX_MFS_DISPLAY_DIGITS - 1u - position) * 4u);
        uint8_t nibble = (uint8_t)((value >> shift) & 0x0fu);
        char digit = (nibble < 10u)
                     ? (char)('0' + nibble)
                     : (char)('A' + nibble - 10u);

        px_mfs_display_set_char(&mfs,
                                position,
                                digit,
                                (decimal_point_mask & (uint8_t)(1u << position)) != 0);
    }
}

static void app_run_adc_show(uint16_t value)
{
    static const uint8_t decimal_point_orbit[6] =
    {
        0x01, 0x02, 0x04, 0x08, 0x04, 0x02,
    };
    uint8_t bit_index;

    value &= 0x03ffu;
    beep_remaining_ms = 0;
    px_mfs_buzzer_set(&mfs, false);

    /* Announce the optional test before showing the first bit. */
    px_mfs_leds_set(&mfs, 0);
    px_mfs_display_set_text(&mfs, "BITS");
    app_delay_ms(APP_ADC_START_FLASH_MS);
    px_mfs_display_clear(&mfs);
    app_delay_ms(APP_ADC_START_FLASH_MS);
    px_mfs_display_set_text(&mfs, "BITS");
    app_delay_ms(APP_ADC_START_FLASH_MS);

    /*
     * Show the ten ADC bits from MSB to LSB. Exactly one LED is lit: D1
     * (top) means one and D4 (bottom) means zero. The decimal point makes a
     * left-to-right, then right-to-left orbit.
     */
    for(bit_index = 0; bit_index < 10u; bit_index++)
    {
        uint8_t bit_value = (uint8_t)((value >> (9u - bit_index)) & 1u);
        uint8_t show_leds = bit_value
                            ? (uint8_t)(1u << PX_MFS_LED_D1)
                            : (uint8_t)(1u << PX_MFS_LED_D4);
        uint16_t beep_ms = bit_value
                           ? APP_ADC_ONE_BEEP_MS
                           : APP_ADC_ZERO_BEEP_MS;
        uint16_t tone_half_period_us = bit_value
                                       ? APP_ADC_ONE_TONE_HALF_PERIOD_US
                                       : APP_ADC_ZERO_TONE_HALF_PERIOD_US;

        app_set_hex_display(value, decimal_point_orbit[bit_index % 6u]);
        px_mfs_leds_set(&mfs, show_leds);

        /* A one is a clear high tone; a zero is a clear low tone. */
        app_play_tone_ms(beep_ms, tone_half_period_us);
        app_delay_ms((uint16_t)(APP_ADC_BIT_PERIOD_MS - beep_ms));
    }

    /* Hold the final value long enough to write down the ten-bit result. */
    app_delay_ms(APP_ADC_END_PAUSE_MS);

    /* Leave a clear completion indication and two grouped double beeps. */
    px_mfs_buzzer_set(&mfs, false);
    px_mfs_leds_set(&mfs, led_mask);
    px_mfs_display_set_text(&mfs, "DONE");
    app_play_tone_ms(APP_ADC_END_BEEP_MS, APP_ADC_DONE_TONE_HALF_PERIOD_US);
    app_delay_ms(APP_ADC_END_GAP_MS);
    app_play_tone_ms(APP_ADC_END_BEEP_MS, APP_ADC_DONE_TONE_HALF_PERIOD_US);
    app_delay_ms(APP_ADC_DONE_PAIR_GAP_MS);
    app_play_tone_ms(APP_ADC_END_BEEP_MS, APP_ADC_DONE_TONE_HALF_PERIOD_US);
    app_delay_ms(APP_ADC_END_GAP_MS);
    app_play_tone_ms(APP_ADC_END_BEEP_MS, APP_ADC_DONE_TONE_HALF_PERIOD_US);
    app_delay_ms(APP_ADC_END_GAP_MS);
    app_update_display();
}

static void app_startup_test(void)
{
    uint16_t i;

    printf("px-fwlib Arduino multifunction shield test\n");
    printf("S1 toggles D1, S2 toggles D2, S3 changes the display page and toggles D3.\n");
    printf("Display pages: A0 potentiometer/LDR, A4 raw sensor input, TEST.\n");
    printf("Hold S1 for about 1 second to run the optional ADC bit show.\n");

    // Segment test: every segment, decimal point and every digit must light.
    px_mfs_display_set_text(&mfs, "8888");
    for(i = 0; i < PX_MFS_DISPLAY_DIGITS; i++)
    {
        px_mfs_display_set_decimal_point(&mfs, i, true);
    }
    px_mfs_leds_set(&mfs, 0x0f);
    px_mfs_buzzer_set(&mfs, true);
    app_delay_ms(150);
    px_mfs_buzzer_set(&mfs, false);
    app_delay_ms(150);

    // Fast full-width hexadecimal sweep: 0000, 0101, ... FFFF.
    // A single decimal point chases across the four digits while the LEDs chase.
    for(i = 0; i < 256u; i++)
    {
        app_set_hex_display((uint16_t)(i * 0x0101u),
                            (uint8_t)(1u << (i & 0x03u)));
        px_mfs_leds_set(&mfs, (uint8_t)(1u << (i & 0x03u)));
        app_delay_ms(6);
    }

    // Exercise every LED independently.
    for(i = 0; i < PX_MFS_LED_COUNT; i++)
    {
        px_mfs_leds_set(&mfs, (uint8_t)(1u << i));
        px_mfs_display_set_u16(&mfs, (uint16_t)(i + 1), true);
        app_delay_ms(120);
    }

    px_mfs_leds_set(&mfs, 0);
    px_mfs_display_set_text(&mfs, "TEST");
    app_delay_ms(300);
    px_mfs_display_set_u16(&mfs, 0, false);
}

static void app_button_pressed(uint8_t button)
{
    switch(button)
    {
    case PX_MFS_BUTTON_S1:
        led_mask ^= (uint8_t)(1u << PX_MFS_LED_D1);
        break;
    case PX_MFS_BUTTON_S2:
        led_mask ^= (uint8_t)(1u << PX_MFS_LED_D2);
        break;
    case PX_MFS_BUTTON_S3:
        led_mask ^= (uint8_t)(1u << PX_MFS_LED_D3);
        display_page = (uint8_t)((display_page + 1) % 3);
        break;
    default:
        return;
    }

    px_mfs_leds_set(&mfs, led_mask);
    px_mfs_buzzer_set(&mfs, true);
    beep_remaining_ms = 45;
    printf("button S%u pressed, LED mask=0x%02X, page=%u\n",
           (unsigned)(button + 1),
           (unsigned)led_mask,
           (unsigned)display_page);
}

static void app_button_long_pressed(uint8_t button)
{
    if(button == PX_MFS_BUTTON_S1)
    {
        app_run_adc_show((display_page == 0) ? ldr_value : sensor_value);
        printf("ADC bit show: %s=0x%03X (%u)\n",
               (display_page == 0) ? "A0" : "A4",
               (unsigned)(((display_page == 0) ? ldr_value : sensor_value) & 0x03ffu),
               (unsigned)((display_page == 0) ? ldr_value : sensor_value));
    }
}

static void app_poll_buttons(void)
{
    uint8_t i;

    for(i = 0; i < PX_MFS_BUTTON_COUNT; i++)
    {
        bool candidate = px_mfs_button_is_pressed(&mfs, (px_mfs_button_t)i);

        if(candidate != button_candidate[i])
        {
            button_candidate[i] = candidate;
            button_counter[i] = 0;
        }
        else if(button_counter[i] < APP_DEBOUNCE_TICKS)
        {
            button_counter[i]++;
        }

        if((button_counter[i] >= APP_DEBOUNCE_TICKS)
           && (button_state[i] != button_candidate[i]))
        {
            button_state[i] = button_candidate[i];
            if(button_state[i])
            {
                app_button_pressed(i);
            }
            else
            {
                button_hold_ticks[i] = 0;
                button_long_fired[i] = false;
            }
        }

        if(button_state[i])
        {
            if(button_hold_ticks[i] < APP_LONG_PRESS_TICKS)
            {
                button_hold_ticks[i]++;
            }
            if((button_hold_ticks[i] >= APP_LONG_PRESS_TICKS)
               && !button_long_fired[i])
            {
                button_long_fired[i] = true;
                app_button_long_pressed(i);
            }
        }
    }
}

static void app_update_display(void)
{
    switch(display_page)
    {
    case 0:
        px_mfs_display_set_u16(&mfs, ldr_value, false);
        break;
    case 1:
        px_mfs_display_set_u16(&mfs, sensor_value, false);
        break;
    default:
        px_mfs_display_set_text(&mfs, "TEST");
        break;
    }
}

/* _____PUBLIC FUNCTIONS_____________________________________________________ */
int main(void)
{
    app_init();
    app_startup_test();

    while(true)
    {
        // Keep the multiplexed display alive. This call is intentionally fast.
        px_mfs_display_refresh(&mfs);
        px_board_delay_ms(1);
        app_ms++;

        if((app_ms % 10) == 0)
        {
            app_poll_buttons();
        }

        if((app_ms % 100) == 0)
        {
            ldr_value    = px_mfs_read_ldr(&mfs);
            sensor_value = px_mfs_read_sensor(&mfs);
            app_update_display();

            // If an optional active-low IR receiver is fitted, D4 indicates activity.
            if(px_mfs_ir_is_active(&mfs))
            {
                led_mask |= (uint8_t)(1u << PX_MFS_LED_D4);
            }
            else
            {
                led_mask &= (uint8_t)~(1u << PX_MFS_LED_D4);
            }
            px_mfs_leds_set(&mfs, led_mask);
        }

        if((app_ms % 500) == 0)
        {
            printf("A0=%u A4=%u buttons=%u%u%u LEDs=0x%02X IR=%u\n",
                   (unsigned)ldr_value,
                   (unsigned)sensor_value,
                   (unsigned)button_state[0],
                   (unsigned)button_state[1],
                   (unsigned)button_state[2],
                   (unsigned)led_mask,
                   (unsigned)px_mfs_ir_is_active(&mfs));
        }

        if(beep_remaining_ms != 0)
        {
            beep_remaining_ms--;
            if(beep_remaining_ms == 0)
            {
                px_mfs_buzzer_set(&mfs, false);
            }
        }
    }
}
