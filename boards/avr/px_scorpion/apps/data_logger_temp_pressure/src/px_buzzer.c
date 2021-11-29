/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2013 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          Buzzer Driver
    Author(s):      pieter.conradie
    Creation Date:  2013-06-17

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_buzzer.h"
#include "px_board.h"
#include "px_systmr.h"
#include "px_pgm_P.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static const uint16_t px_buzzer_piano_freq_table[89] PX_ATTR_PGM = 
        {0, 27, 29, 31, 33, 35, 37, 39, 41, 44, 46, 49, 52, 55, 58, 62, 65, 69,
         73, 78, 82, 87, 92, 98, 104, 110, 117, 123, 131, 139, 147, 156, 165,
         175, 185, 196, 208, 220, 233, 247, 262, 277, 294, 311, 330, 349, 370,
         392, 415, 440, 466, 494, 523, 554, 587, 622, 659, 698, 740, 784, 831,
         880, 932, 988, 1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661,
         1760, 1865, 1976, 2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322,
         3520, 3729, 3951, 4186};

static px_systmr_t           px_buzzer_systmr;
static const buzzer_tune_t * px_buzzer_tune_start;
static const buzzer_tune_t * buzzer_tune;
static bool                  px_buzzer_beep_flag;
static bool                  px_buzzer_active_flag;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static uint16_t px_buzzer_piano_key_to_freq_hz(const uint8_t key)
{
    if(key >= PX_SIZEOF_ARRAY(px_buzzer_piano_freq_table))
    {
        return 0;
    }
    return px_pgm_rd_u16(&px_buzzer_piano_freq_table[key]);
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_buzzer_init(void)
{
}

bool px_buzzer_is_active(void)
{
    return px_buzzer_active_flag;
}

void px_buzzer_off(void)
{
    TCCR0A = 0;
    TCCR0B = 0;

    px_buzzer_active_flag = false;
}

void px_buzzer_on(const uint16_t freq_hz)
{
    uint16_t ocra;

    // Sanity check
    if(freq_hz == 0)
    {
        px_buzzer_off();
        return;
    }

    // Calculate period from specified frequency
    ocra = PX_UDIV_ROUND( PX_UDIV_ROUND(F_CPU,(2*64)), freq_hz) - 1;
    // Cap lowest frequency
    if(ocra > 255)
    {
        ocra = 255;
    }

    /* 
     *  Start Timer 0 with clock prescaler CLK/64 and CTC mode. Output on PD6 (OC0A).
     * 
     *  For F_CPU = 7372800 Hz, resolution is 8.7 us -> 57600 Hz to 225 Hz
     */
    TCCR0A =  (0 << COM0A1) | (1 << COM0A0) | (1 << WGM01) | (0 << WGM00);
    TCCR0B =  (0 << WGM02) | (1 << CS02) | (0 << CS01) | (0 << CS00);

    // Reset counter
    TCNT0 = 0;

    // Set frequency
    OCR0A  = (uint8_t)ocra;

    px_buzzer_active_flag = true;
}

void px_buzzer_beep(const uint16_t freq_hz, const px_systmr_ticks_t px_systmr_ticks)
{
    px_buzzer_on(freq_hz);
    px_systmr_start(&px_buzzer_systmr, px_systmr_ticks);
    px_buzzer_beep_flag = true;
}

void px_buzzer_play(const buzzer_tune_t *tune)
{    
    px_buzzer_tune_start = tune;
    buzzer_tune       = tune;
    // Set timer to expire immediately so that tune will start
    px_systmr_start(&px_buzzer_systmr, 0);
}

bool px_buzzer_task(void)
{
    uint8_t           piano_key;
    px_systmr_ticks_t duration_ticks;
    uint16_t          freq_hz;

    // Has beep started
    if(px_buzzer_beep_flag == true)
    {
        // Has beep ended
        if(px_systmr_has_expired(&px_buzzer_systmr) == false)
        {
            // Task busy
            return true;
        }
        else
        {
            px_buzzer_off();
            px_buzzer_beep_flag = false;
            // Task finished
            return false;
        }
    }

    // Has tune ended?
    if(buzzer_tune == NULL)
    {
        px_buzzer_off();
        // Task finished
         return false;
    }

    // Busy with note/pause?
    if(px_systmr_has_expired(&px_buzzer_systmr) == false)
    {
        // Task busy
        return true;
    }

    // Get new key and duration
    piano_key      = px_pgm_rd_u8(&buzzer_tune->piano_key);
    duration_ticks = px_pgm_rd_u16(&buzzer_tune->duration_ticks);

    // repeat tune?
    if((piano_key == PX_BUZZER_TUNE_REPEAT) && (duration_ticks == 0))
    {
        px_buzzer_off();
        // Restart tune
        buzzer_tune = px_buzzer_tune_start;
        // Task not finished
        return true;
    }

    // End of tune?
    if((piano_key == PX_BUZZER_TUNE_END) && (duration_ticks == 0))
    {
        px_buzzer_off();
        // Mark tune as finished
        px_buzzer_tune_start = NULL;
        buzzer_tune          = NULL;
        // Task finished
        return false;
    }

    // Start note
    freq_hz = px_buzzer_piano_key_to_freq_hz(piano_key);
    px_buzzer_on(freq_hz);

    // Start timer to wait for end of note/pause
    px_systmr_start(&px_buzzer_systmr, duration_ticks);

    // Next note
    buzzer_tune++;

    // Task busy
    return true;
}

