#ifndef __PX_NMEA_H__
#define __PX_NMEA_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2010 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_nmea.h : NMEA parser
    Author(s):      Pieter Conradie
    Creation Date:  2010-05-28

============================================================================= */

/** 
 *  @ingroup COMMS
 *  @defgroup PX_NMEA px_nmea.h : NMEA parser
 *  
 *  GPS NMEA protocol parser.
 *  
 *  File(s):
 *  - comms/inc/px_nmea.h
 *  - comms/src/px_nmea.c
 *  
 *  @see http://en.wikipedia.org/wiki/NMEA_0183
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
// NMEA output strings
#define PX_NMEA_GGA_STR "GGA" ///< Time, position and fix type data
#define PX_NMEA_GLL_STR "GLL" ///< Latitude, longitude, UTC time of position fix and status
#define PX_NMEA_GSA_STR "GSA" ///< GPS receiver operating mode, satellites used in the position solution and DOP values
#define PX_NMEA_GSV_STR "GSV" ///< The number of GPS satellites in view, satellite ID numbers, elevation, azimuth, and SNR values
#define PX_NMEA_MSS_STR "MSS" ///< Signal-to-noise ratio, signal strength, frequency, and bit rate from a radio-beacon receiver
#define PX_NMEA_RMC_STR "RMC" ///< Time, date, position, course and speed data
#define PX_NMEA_VTG_STR "VTG" ///< Course and speed information relative to the ground
#define PX_NMEA_ZDA_STR "ZDA" ///< Date and time

/* _____TYPE DEFINITIONS_____________________________________________________ */
/**
 *  Definition for a pointer to a function that will be called to 
 *  send a character.
 */
typedef void (*px_nmea_tx_byte_t)(uint8_t data);

/**
 *  Definition for a pointer to a function that when a valid NMEA string is 
 *  received.
 */
typedef void (*px_nmea_on_valid_str_t)(const char* data);

/**
 *  Definition for a pointer to a function that will be called when GPS data 
 *  is valid.
 */
typedef void (*px_nmea_on_valid_gps_data_t)(void);

/// Parsed time, position, quality data
typedef struct
{
   uint32_t utc_time;
   uint16_t utc_time_fraction;
   uint8_t  delay_from_PPS;
   int16_t  latitude;
   uint16_t latitude_fraction;
   int16_t  longitude;
   uint16_t longitude_fraction;
   int16_t  altitude;
   uint8_t  altitude_fraction;
   uint16_t heading;
   uint8_t  heading_fraction;
   uint8_t  speed;
   uint8_t  speed_fraction;
   uint8_t  sattelites_used;
   uint8_t  hdop;
   uint8_t  hdop_fraction;
   bool     gga_valid_flag;
   bool     vtg_valid_flag;
} px_nmea_data_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */
px_nmea_data_t px_nmea_data;

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 * Initialise NMEA parser module
 *  
 * @param tx_byte           Pointer to a function that will be called to 
 *                          transmit a byte.
 * @param on_valid_str      Pointer to a function that will be called when a 
 *                          valid NMEA string has been received.
 * @param on_valid_gps_data Pointer to a function that will be called when the 
 *                          data structure has been completely polulated with
 *                          valid data.
 */
void px_nmea_init(px_nmea_tx_byte_t           tx_byte,
                  px_nmea_on_valid_str_t      on_valid_str,
                  px_nmea_on_valid_gps_data_t on_valid_gps_data);

/**
 *  Function handler that is fed all raw received data.
 *  
 *  @param[in] data     received 8-bit data
 *  
 */
void px_nmea_on_rx_byte(uint8_t data);

/**
 * Function that is called to send an NMEA frame with the checksum appended.
 *  
 * @param frame     Pointer to zero terminated string.
 */
void px_nmea_tx_frame(char* frame);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
