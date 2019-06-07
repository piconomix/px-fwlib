/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2006 Pieter Conradie <https://piconomix.com>
 
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.

    Title:          px_m41t00.h : Driver for an ST M41T00 Real Time Clock
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_rtc_m41t00.h"
#include "px_dbg.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_DBG_DECL_NAME("m41t00");

/// BCD Date/Time
typedef struct
{
    uint8_t sec;
    uint8_t min;
    uint8_t century_hours;
    uint8_t day_of_week;
    uint8_t day_of_month;
    uint8_t month;
    uint8_t year;
} px_m41t00_bcd_time_t;

/// M41T00 Data
typedef struct
{
    uint8_t                adr;
    px_m41t00_bcd_time_t   bcd_time;
    uint8_t                control;
} px_m41t00_data_t;

/// Stop bit
#define PX_M41T00_ST_ADR   0x00
#define PX_M41T00_ST_BIT   (1<<7)

/// Century Enable Bit
#define PX_M41T00_CEB_ADR  0x02
#define PX_M41T00_CEB_BIT  (1<<7)

/// Century Bit
#define PX_M41T00_CB_ADR   0x02
#define PX_M41T00_CB_BIT   (1<<6)

// Control register map

/// Output level bit
#define PX_M41T00_OUT_BIT  (1<<7)

/// Frequency test bit
#define PX_M41T00_FT_BIT   (1<<6)

/// Sign bit
#define PX_M41T00_S_BIT    (1<<5)

/// Clock calibration
#define PX_M41T00_CAL_MASK 0x1F

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static px_i2c_handle *  px_m41t00_i2c_handle;
static px_m41t00_data_t px_m41t00_data;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static uint8_t px_m41t00_bcd_to_byte(uint8_t bcd_val)
{
    return (bcd_val&0xf) + ((bcd_val&0xf0) >> 4)*10;
}

static uint8_t px_m41t00_byte_to_bcd(uint8_t byte_val)
{
    return (byte_val%10) + ((byte_val/10) << 4);
}

static bool px_m41t00_rd_data(void)
{
    // Set address to start reading from
    px_m41t00_data.adr = 0;

    // Send address to read
    if(!px_i2c_wr(px_m41t00_i2c_handle, 
                  &px_m41t00_data.adr, 
                  1, 
                  PX_I2C_FLAG_START_AND_END))
    {
        PX_DBG_ERR("Unable to write address");
        return false;
    }

    // Read data
    if(!px_i2c_rd(px_m41t00_i2c_handle, 
                  &px_m41t00_data.bcd_time,
                  sizeof(px_m41t00_data.bcd_time) + sizeof(px_m41t00_data.control), 
                  PX_I2C_FLAG_REP_START_AND_STOP))
    {
        PX_DBG_ERR("Unable to read data");
        return false;
    }

    // Success
    return true;
}

static bool px_m41t00_wr_data(void)
{
    // Set address to start writing to
    px_m41t00_data.adr = 0;

    // Write data
    if(!px_i2c_wr(px_m41t00_i2c_handle, 
                  &px_m41t00_data, 
                  sizeof(px_m41t00_data), 
                  PX_I2C_FLAG_START_AND_STOP))
    {
        PX_DBG_ERR("Unable to write data");
        return false;
    }

    // Success
    return true;
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_m41t00_init(px_i2c_handle_t * px_i2c_handle)
{
    px_m41t00_i2c_handle = px_i2c_handle;
}

bool px_m41t00_get_time(px_m41t00_time_t * px_rtc_time)
{    
    uint8_t i;

    // Populate with default values
    px_rtc_time->sec          = 0;
    px_rtc_time->min          = 0;
    px_rtc_time->hour         = 0;
    px_rtc_time->day_of_week  = 1;
    px_rtc_time->day_of_month = 1;
    px_rtc_time->month        = 1;
    px_rtc_time->year         = 7;

    // Read RTC data
    if(!px_m41t00_rd_data())
    {
        return false;
    }

    // See if timer has been stopped
    if(((uint8_t*)(&px_m41t00_data.bcd_time))[PX_M41T00_ST_ADR]  & PX_M41T00_ST_BIT)
    {
        PX_DBG_INFO("RTC has been stopped!");
        return false;
    }

    // Mask off century bits
    ((uint8_t*)(&px_m41t00_data.bcd_time))[PX_M41T00_CEB_ADR] &= ~PX_M41T00_CEB_BIT;
    ((uint8_t*)(&px_m41t00_data.bcd_time))[PX_M41T00_CB_ADR]  &= ~PX_M41T00_CB_BIT;

    // Convert to binary format
    for(i=0;i<sizeof(*px_rtc_time);i++)
    {
        ((uint8_t*)px_rtc_time)[i] = px_m41t00_bcd_to_byte(((uint8_t*)(&px_m41t00_data.bcd_time))[i]);
    }

    // Sanity check
    if((px_rtc_time->month        < 1   ) || (px_rtc_time->month        > 12  )) return false;
    if((px_rtc_time->day_of_month < 1   ) || (px_rtc_time->day_of_month > 31  )) return false;
    if(px_rtc_time->hour          > 23  ) return false;
    if(px_rtc_time->min           > 59  ) return false;
    if(px_rtc_time->sec           > 59  ) return false;

    return true;
}

bool px_m41t00_set_time(const px_m41t00_time_t * px_rtc_time)
{    
    int i;

    // Convert to BCD format
    for(i=0;i<sizeof(px_m41t00_data.bcd_time);i++)
    {
        ((uint8_t*)(&px_m41t00_data.bcd_time))[i] = px_m41t00_byte_to_bcd(((uint8_t*)px_rtc_time)[i]);
    }

    // Set control bits
    ((uint8_t*)(&px_m41t00_data.bcd_time))[PX_M41T00_ST_ADR]  &= ~PX_M41T00_ST_BIT;
    ((uint8_t*)(&px_m41t00_data.bcd_time))[PX_M41T00_CEB_ADR] &= ~PX_M41T00_CEB_BIT;
    ((uint8_t*)(&px_m41t00_data.bcd_time))[PX_M41T00_CB_ADR]  &= ~PX_M41T00_CB_BIT;    

    px_m41t00_data.control = 0x00;

    return px_m41t00_wr_data();
}

