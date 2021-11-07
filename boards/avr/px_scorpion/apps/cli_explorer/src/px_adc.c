/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2014 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
 
    Title:          px_adc.h : ADC Driver
    Author(s):      Pieter Conradie
    Creation Date:  2014-06-01

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <avr/interrupt.h>
#include <avr/io.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_adc.h"
#include "px_board.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("adc");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static uint8_t        px_adc_channel;
static uint16_t       px_adc_sample_counter;
static uint16_t       px_adc_nr_of_samples_per_sum;

static uint8_t        px_adc_page;
static px_adc_data_t  px_adc_data_page0[PX_ADC_NR_OF_CHANNELS];
static px_adc_data_t  px_adc_data_page1[PX_ADC_NR_OF_CHANNELS];
static volatile bool  px_adc_data_ready_flag;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void px_adc_data_rst(px_adc_data_t * adc_data)
{
    adc_data->min = PX_MAX_OF_TYPE(uint16_t);
    adc_data->max = PX_MIN_OF_TYPE(uint16_t);
    adc_data->sum = 0;
}

static void px_adc_data_update(px_adc_data_t * adc_data, uint16_t val)
{
    // Is this the minimum value so far?
    if(adc_data->min > val)
    {
        // Remember minimum value
        adc_data->min = val;
    }
    // Is this the maximum value so far?
    if(adc_data->max < val)
    {
        // Remember maximum value
        adc_data->max = val;
    }
    // Accumulate sum
    adc_data->sum += val;
}

static void px_adc_select_channel(uint8_t channel)
{
    // Select AVCC as reference with external capacitor at AREF pin
    switch(channel)
    {
    case 0:
        ADMUX = (0<<REFS1) | (1<<REFS0) | (ADC0<<MUX0);
        break;
    case 1:
        ADMUX = (0<<REFS1) | (1<<REFS0) | (ADC1<<MUX0);
        break;
    case 2:
        ADMUX = (0<<REFS1) | (1<<REFS0) | (ADC2<<MUX0);
        break;
    case 3:
        ADMUX = (0<<REFS1) | (1<<REFS0) | (ADC3<<MUX0);
        break;
    default:
        break;
    }
}

ISR(ADC_vect)
{
    uint8_t i;

    // Retrieve sampled value
    volatile uint16_t val = ADC;

    // Update sample data
    if(px_adc_page == 0)
    {
        // Update page 0 data
        px_adc_data_update(&px_adc_data_page0[px_adc_channel], val);
    }
    else
    {
        // Update page 1 data
        px_adc_data_update(&px_adc_data_page1[px_adc_channel], val);
    }

    // Select next ADC channel
    if(++px_adc_channel < PX_ADC_NR_OF_CHANNELS)
    {
        px_adc_select_channel(px_adc_channel);
        // Start conversion
        PX_BIT_SET_HI(ADCSRA, ADSC);
        return;
    }
    else
    {
        // Reset to first channel
        px_adc_channel = 0;
    }

    // Must more samples be accumulated?
    if(px_adc_sample_counter != 0)
    {
        px_adc_sample_counter--;
        return;
    }
    // Reset sample counter
    px_adc_sample_counter = px_adc_nr_of_samples_per_sum;

    // Change to other page
    if(px_adc_page == 0)
    {
        // Change to page 1
        px_adc_page = 1;

        // Reset page 1 data
        for(i = 0; i < PX_ADC_NR_OF_CHANNELS; i++)
        {
            px_adc_data_rst(&px_adc_data_page1[i]);
        }
    }
    else
    {
        // Change to page 0
        px_adc_page = 0;

        // Reset page 0 data
        for(i = 0; i < PX_ADC_NR_OF_CHANNELS; i++)
        {
            px_adc_data_rst(&px_adc_data_page0[i]);
        }
    }

    // Set flag to indicate that data is ready
    px_adc_data_ready_flag = true;
}

ISR(TIMER1_COMPA_vect)
{
    // Previous conversions finished?
    if(px_adc_channel != 0)
    {
        return;
    }
    // Start with ADC0
    px_adc_select_channel(0);
    // Start conversion
    PX_BIT_SET_HI(ADCSRA, ADSC);
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_adc_init(void)
{
    // Disable digital input buffer on ADC pins 1 and 0 to conserve power
    DIDR0 = (1<<1) | (1<<0);
}

uint16_t px_adc_sample(uint8_t channel)
{
    // Select AVCC as reference with external capacitor at AREF pin
    // Select ADC0 (MUX3...0 = 0000b)
    ADMUX =   (0<<REFS1) | (1<<REFS0) 
            | (0<<MUX3)  | (0<<MUX2)  | (0<<MUX1) | (0<<MUX0);

    // Select channel
    px_adc_select_channel(channel);

    // Enable ADC; Select ADC Prescaler of 128 resulting in an ADC clock
    // frequency of 57.6 kHz with F_CPU = 7.3728 MHz
    ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);

    // Start conversion
    PX_BIT_SET_HI(ADCSRA, ADSC);

    // Wait until conversion is finished, which normally takes 13 ADC clock 
    // cycles (226 us).
    while(PX_BIT_IS_HI(ADCSRA, ADSC))
    {
        ;
    }

    // Disable ADC
    ADCSRA = 0;

    // Return sampled value
    return ADC;
}

void px_adc_start(uint16_t sampling_freq_hz, uint16_t nr_of_samples_per_sum)
{
    uint8_t i;

    // Reset all data 
    for(i = 0; i < PX_ADC_NR_OF_CHANNELS; i++)
    {
        px_adc_data_rst(&px_adc_data_page0[i]);
        px_adc_data_rst(&px_adc_data_page1[i]);
    }
    px_adc_channel               = 0;
    px_adc_page                  = 0;
    px_adc_sample_counter        = nr_of_samples_per_sum;
    px_adc_nr_of_samples_per_sum = nr_of_samples_per_sum;
    px_adc_data_ready_flag       = false;

    /*
     *  Enable ADC and Enable Interrupt.
     *  Select ADC Prescaler of 32 resulting in an ADC clock frequency of
     *  230.4 kHz with F_CPU = 7.3728 MHz
     */
    ADCSRA =   (1<<ADEN)  | (0<<ADSC)  | (1<<ADIE) 
             | (1<<ADPS2) | (0<<ADPS1) | (1<<ADPS0);

    /* 
     *  Start 16-bit TMR1 with clock prescaler CLK/8. For F_CPU = 7372800 Hz,
     *  resolution is 135.6 ns and lowest frequency is 112.5 Hz
     *
     *  Select Clear Timer on Compare match (CTC) mode of operation. This means
     *  that when TCNT1 reaches the OCR1A value, OCF1A flag will be set and
     *  TCNT1 will be reset to 0.
     */ 
    TCCR1A = (0<<WGM11) | (0<<WGM10);
    TCCR1B = (0<<WGM13) | (1<<WGM12) | (0<<CS12) | (0<<CS11) | (1<<CS10);

    // Reset counter
    TCNT1 = 0;

    // Calculate and set delay
    OCR1A  = (uint16_t)PX_UDIV_ROUND(F_CPU, sampling_freq_hz) - 1;

    // Enable TMR interrupt on compare match
    PX_BIT_SET_HI(TIMSK1, OCIE1A);
}

void px_adc_stop(void)
{
    // Disable TMR
    TCCR1B = 0;
    // Disable ADC
    ADCSRA = 0;
}

bool px_adc_data_ready(void)
{
    return px_adc_data_ready_flag;
}

void px_adc_rst_data_ready_flag(void)
{
    px_adc_data_ready_flag = false;
}

const px_adc_data_t * px_adc_get_data(uint8_t channel)
{
    if(px_adc_page == 0)
    {
        return &px_adc_data_page1[channel];
    }
    else
    {
        return &px_adc_data_page0[channel];
    }
}
