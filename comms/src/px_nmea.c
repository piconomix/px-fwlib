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

/* _____STANDARD INCLUDES____________________________________________________ */
#include <string.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_nmea.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("px_nmea");

// Receive and transmit buffer size
#define PX_NMEA_BUFFER_SIZE    128

typedef enum
{
   PX_NMEA_RX_STATE_START = 0,
   PX_NMEA_RX_STATE_PAYLOAD,
   PX_NMEA_RX_STATE_CHECKSUM1,
   PX_NMEA_RX_STATE_CHECKSUM2,
   PX_NMEA_RX_STATE_END_CR,
   PX_NMEA_RX_STATE_END_LF,
} px_nmea_rx_state_t;

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
px_nmea_data_t px_nmea_data;

/* _____LOCAL VARIABLES______________________________________________________ */
static px_nmea_tx_byte_t           px_nmea_tx_byte_fn;
static px_nmea_on_valid_str_t      px_nmea_on_valid_str_fn;
static px_nmea_on_valid_gps_data_t px_nmea_on_valid_gps_data_fn;

static uint8_t                     px_nmea_rx_buf[PX_NMEA_BUFFER_SIZE];
static uint16_t                    px_nmea_rx_index;
static uint8_t                     px_nmea_rx_checksum;
static px_nmea_rx_state_t          px_nmea_rx_state;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */
static void   px_nmea_tx_byte                  (uint8_t data);

static bool   px_nmea_cmp_nibble_with_hex_ascii(uint8_t nibble, char ascii);
static char * px_nmea_parse_str_to_u32         (char * buf, uint32_t * value);
static char * px_nmea_parse_str_to_s16         (char * buf, int16_t *  value);
static char * px_nmea_parse_str_to_u16         (char * buf, uint16_t * value);
static char * px_nmea_parse_str_fraction_to_u16(char * buf, uint16_t * value, uint8_t precision);
static char * px_nmea_parse_str_to_u8          (char * buf, uint8_t *  value);
static char * px_nmea_parse_str_fraction_to_u8 (char * buf, uint8_t *  value, uint8_t precision);
static char * px_nmea_find_next_param          (char * buf);

static void   px_nmea_on_rx_frame              (char * buf);

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void px_nmea_tx_byte(uint8_t data)
{
    if(px_nmea_tx_byte_fn == NULL)
    {
        return;
    }
    (*px_nmea_tx_byte_fn)(data);
}

static bool  px_nmea_cmp_nibble_with_hex_ascii(uint8_t nibble, char ascii)
{
   if(nibble <= 9)
   {
       nibble += '0';
   }
   else
   {
       nibble += ('A' - 10);
   }

   if(nibble == ascii)
   {
       return true;
   }
   else
   {
       return false;
   }
}

static char * px_nmea_parse_str_to_u32(char * buf, uint32_t * value)
{
   uint32_t u32Value = 0;
   while((*buf != ',') && (*buf != '.') && (*buf != '\0'))
   {
      if(*buf == ' ')
      {
         // Ignore whitespace
         buf++;
         continue;
      }
      u32Value *= 10;
      u32Value += (*buf++) - '0';
   }
   *value = u32Value;
   return buf;
}

static char * px_nmea_parse_str_to_s16(char * buf, int16_t * value)
{
   bool  bNeg     = false;
   int16_t s16Value = 0;

   while((*buf != ',') && (*buf != '.') && (*buf != '\0'))
   {
      if(*buf == ' ')
      {
         // Ignore whitespace
         buf++;
         continue;
      }
      else if(*buf == '-')
      {
         // Remove sign
         bNeg = true;
         buf++;
         continue;
      }
      s16Value *= 10;
      s16Value += (*buf++) - '0';
   }
   // Add sign if necessary
   if(bNeg) s16Value *= -1;

   *value = s16Value;
   return buf;
}

static char * px_nmea_parse_str_to_u16(char * buf, uint16_t * value)
{
   uint16_t u16Value = 0;
   while((*buf != ',') && (*buf != '.') && (*buf != '\0'))
   {
      if(*buf == ' ')
      {
         // Ignore whitespace
         buf++;
         continue;
      }
      u16Value *= 10;
      u16Value += (*buf++) - '0';
   }
   *value = u16Value;
   return buf;
}

static char * px_nmea_parse_str_fraction_to_u16(char * buf, uint16_t * value, uint8_t precision)
{
   uint16_t u16Value = 0;
   while((*buf != ',') && (*buf != '.') && (*buf != '\0') && (precision != 0))
   {
      if(*buf == ' ')
      {
         // Ignore whitespace
         buf++;
         continue;
      }
      u16Value *= 10;
      u16Value += (*buf++) - '0';
      precision--;
   }
   while(precision != 0)
   {
      precision--;
      u16Value *= 10;
   }
   *value = u16Value;
   return buf;
}

static char * px_nmea_parse_str_to_u8(char * buf, uint8_t * value)
{
   uint8_t u8Value = 0;
   while((*buf != ',') && (*buf != '.') && (*buf != '\0'))
   {
      if(*buf == ' ')
      {
         // Ignore whitespace
         buf++;
         continue;
      }
      u8Value *= 10;
      u8Value += (*buf++) - '0';
   }
   *value = u8Value;
   return buf;
}

static char * px_nmea_parse_str_fraction_to_u8(char * buf, uint8_t * value, uint8_t precision)
{
   uint8_t u8Value = 0;
   while((*buf != ',') && (*buf != '.') && (*buf != '\0') && (precision != 0))
   {
      if(*buf == ' ')
      {
         // Ignore whitespace
         buf++;
         continue;
      }
      u8Value *= 10;
      u8Value += (*buf++) - '0';
      precision--;
   }
   while(precision != 0)
   {
      precision--;
      u8Value *= 10;      
   }
   *value = u8Value;
   return buf;
}

static char * px_nmea_find_next_param(char * buf)
{
   while(*buf != ',')
   {
      if(*buf == '\0') return buf;
      buf++;
   }
   return (++buf);
}

static void px_nmea_on_rx_frame(char * buf)
{  
    // Notify handler with valid NMEA string
   (*px_nmea_on_valid_str_fn)(buf);

   // Make sure that message ID starts with "GP"
   if(*buf++ != 'G')
   {
       return;
   }
   if(*buf++ != 'P')
   {
       return;
   }

   // Parse GGA (Global Positioning System Fixed Data) string
   if(strncmp(buf,PX_NMEA_GGA_STR,3) == 0)
   {
#if 0
      if(px_nmea_data.gga_valid_flag)
      {
         // Enable VTG message @ 1Hz update rate
         px_nmea_tx_frame("$PSRF103,05,00,01,01");
      }
#endif
      buf += 4;
      px_nmea_data.gga_valid_flag     = false;
      px_nmea_data.latitude_fraction  = 0;
      px_nmea_data.longitude_fraction = 0;
      // UTC Time
      buf = px_nmea_parse_str_to_u32(buf, &px_nmea_data.utc_time);
      if(*buf == '.')
      {
         buf++;
         buf = px_nmea_parse_str_fraction_to_u16(buf, &px_nmea_data.utc_time_fraction, 3);
      }
      buf = px_nmea_find_next_param(buf);

      // Latitude
      buf = px_nmea_parse_str_to_u16(buf, (uint16_t *)&px_nmea_data.latitude);
      if(*buf == '.')
      {
         buf++;
         buf = px_nmea_parse_str_fraction_to_u16(buf, &px_nmea_data.latitude_fraction, 4);
      }
      buf = px_nmea_find_next_param(buf);
      if(*buf == 'S')
      {
          px_nmea_data.latitude *= -1;
      }
      buf = px_nmea_find_next_param(buf);

      // Longitude
      buf = px_nmea_parse_str_to_u16(buf, (uint16_t *)&px_nmea_data.longitude);
      if(*buf == '.')
      {
         buf++;
         buf = px_nmea_parse_str_fraction_to_u16(buf, &px_nmea_data.longitude_fraction, 4);
      }
      buf = px_nmea_find_next_param(buf);
      if(*buf == 'W')
      {
          px_nmea_data.longitude *= -1;
      }
      buf = px_nmea_find_next_param(buf);
      // Fix valid
      if(*buf == '1')
      {
          px_nmea_data.gga_valid_flag  = true;
      }
      buf = px_nmea_find_next_param(buf);
      // Number of satelites
      buf = px_nmea_parse_str_to_u8(buf, &px_nmea_data.sattelites_used);
      buf = px_nmea_find_next_param(buf);
      // HDOP
      buf = px_nmea_parse_str_to_u8(buf, &px_nmea_data.hdop);
      if(*buf == '.')
      {
         buf++;
         buf = px_nmea_parse_str_fraction_to_u8(buf, &px_nmea_data.hdop_fraction, 1);
      }
      buf = px_nmea_find_next_param(buf);
      // Altitude
      buf = px_nmea_parse_str_to_s16(buf, &px_nmea_data.altitude);
      if(*buf == '.')
      {
         buf++;
         buf = px_nmea_parse_str_fraction_to_u8(buf, &px_nmea_data.altitude_fraction, 2);
      }      

      px_nmea_data.gga_valid_flag = true;
      
      // See if all the fields were populated
      if(px_nmea_data.gga_valid_flag && px_nmea_data.vtg_valid_flag)
      {
         (*px_nmea_on_valid_gps_data_fn)();
         px_nmea_data.gga_valid_flag = false;
         px_nmea_data.vtg_valid_flag = false;
      }
   }
   // Parse VTG (Course Over Ground and Ground Speed) string
   else if(strncmp(buf, PX_NMEA_VTG_STR, 3) == 0)
   {
      buf += 4;

      // Heading
      buf = px_nmea_parse_str_to_u16(buf,&px_nmea_data.heading);
      if(*buf == '.')
      {
         buf++;
         buf = px_nmea_parse_str_fraction_to_u8(buf,&px_nmea_data.heading_fraction,2);
      }
      buf = px_nmea_find_next_param(buf);
      buf = px_nmea_find_next_param(buf);
      buf = px_nmea_find_next_param(buf);
      buf = px_nmea_find_next_param(buf);
      buf = px_nmea_find_next_param(buf);
      buf = px_nmea_find_next_param(buf);

      // Speed
      buf = px_nmea_parse_str_to_u8(buf,&px_nmea_data.speed);
      if(*buf == '.')
      {
         buf++;
         buf = px_nmea_parse_str_fraction_to_u8(buf,&px_nmea_data.speed_fraction,2);
      }
      
      px_nmea_data.vtg_valid_flag = true;
      
      // See if all the fields were populated
      if(px_nmea_data.gga_valid_flag && px_nmea_data.vtg_valid_flag)
      {
         (*px_nmea_on_valid_gps_data_fn)();
         px_nmea_data.gga_valid_flag = false;
         px_nmea_data.vtg_valid_flag = false;
      }
   }
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_nmea_init(px_nmea_tx_byte_t           tx_byte,
                  px_nmea_on_valid_str_t      on_valid_str,
                  px_nmea_on_valid_gps_data_t on_valid_gps_data)
{
    // Save function pointers
    px_nmea_tx_byte_fn           = tx_byte;
    px_nmea_on_valid_str_fn      = on_valid_str;
    px_nmea_on_valid_gps_data_fn = on_valid_gps_data;

   // Reset state variables
   px_nmea_rx_state            = PX_NMEA_RX_STATE_START;
   px_nmea_rx_index            = 0;
   px_nmea_data.gga_valid_flag = false;
   px_nmea_data.vtg_valid_flag = false;
}

void px_nmea_on_rx_byte(uint8_t data)
{   
   switch(px_nmea_rx_state)
   {
   case PX_NMEA_RX_STATE_START :
      {
         // Check start sequence
         if(data != '$')
         {
             break;
         }
         px_nmea_rx_state = PX_NMEA_RX_STATE_PAYLOAD;
         // Reset index and checksum
         px_nmea_rx_checksum = 0;
         px_nmea_rx_index    = 0;
         return;
      }
   case PX_NMEA_RX_STATE_PAYLOAD :
      {
         // Check for unexpected characters in payload
         if(  (data == '$' )
            ||(data == '\r')
            ||(data == '\n')
            ||(data >= 0x80)  )
         {
             break;
         }
         // Check for checksum marker
         if(data == '*')
         {
            px_nmea_rx_state = PX_NMEA_RX_STATE_CHECKSUM1;
            return;
         }
         // Update checksum of payload
         px_nmea_rx_checksum ^= data;
         // Put received byte into buffer
         px_nmea_rx_buf[px_nmea_rx_index] = data;
         // Check for buffer overflow
         if(++px_nmea_rx_index >= (PX_NMEA_BUFFER_SIZE-1))
         {
             break;
         }
         return;
      }
   case PX_NMEA_RX_STATE_CHECKSUM1 :
      {
         // Check high nibble of checksum
         if(!px_nmea_cmp_nibble_with_hex_ascii(((px_nmea_rx_checksum>>4)&0x0f), data))
         {
             break;
         }
         
         px_nmea_rx_state = PX_NMEA_RX_STATE_CHECKSUM2;
         return;
      }
   case PX_NMEA_RX_STATE_CHECKSUM2 :
      {
         // Check low nibble of checksum
         if(!px_nmea_cmp_nibble_with_hex_ascii((px_nmea_rx_checksum&0x0f),data))
         {
             break;
         }

         px_nmea_rx_state = PX_NMEA_RX_STATE_END_CR;
         return;
      }
   case PX_NMEA_RX_STATE_END_CR :
      {
         // Check Carriage Return
         if(data != '\r')
         {
             break;
         }
         px_nmea_rx_state = PX_NMEA_RX_STATE_END_LF;
         return;
      }
    case PX_NMEA_RX_STATE_END_LF:
      {
         // Check Line Feed
         if(data != '\n')
         {
             break;
         }
         // Append terminating zero
         px_nmea_rx_buf[px_nmea_rx_index] = '\0';
         // String successfully received
         px_nmea_on_rx_frame((char *)px_nmea_rx_buf);
         break;
      }   
   }
   // Error detected... reset receiver
   px_nmea_rx_state = PX_NMEA_RX_STATE_START;
}

void px_nmea_tx_frame(char * frame)
{
   uint8_t data;
   uint8_t checksum = '$';

   // Send data and calculate checksum
   while(*frame)
   {
       data      = *frame++;
       checksum ^= data;
       px_nmea_tx_byte(data);
   }

   // Add checksum
   px_nmea_tx_byte('*');
   // Send high nibble
   if(checksum < 0xA0)
   {
       px_nmea_tx_byte(((checksum >> 4) & 0x0f) + '0');
   }
   else
   {
       px_nmea_tx_byte(((checksum >> 4) & 0x0f) + ('A' - 10));
   }
   // Send low nibble
   checksum &= 0x0f;
   if(checksum < 0x0A)
   {
       px_nmea_tx_byte(checksum + '0');
   }
   else
   {
       px_nmea_tx_byte(checksum + ('A' - 10));
   }

   // Add end sequence
   px_nmea_tx_byte('\r');
   px_nmea_tx_byte('\n');
}

