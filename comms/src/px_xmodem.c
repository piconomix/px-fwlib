/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2006 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          XMODEM-CRC receive module
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <string.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_xmodem.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("px_xmodem");

/// @name XMODEM protocol definitions
//@{
#define PX_XMODEM_DATA_SIZE         128
//@}

/// @name XMODEM flow control characters
//@{
#define PX_XMODEM_SOH               0x01 ///< Start of Header
#define PX_XMODEM_EOT               0x04 ///< End of Transmission 
#define PX_XMODEM_ACK               0x06 ///< Acknowledge 
#define PX_XMODEM_NAK               0x15 ///< Not Acknowledge 
#define PX_XMODEM_C                 0x43 ///< ASCII 'C'
//@}

/// XMODEM packet structure definition
typedef struct
{
    uint8_t  start;
    uint8_t  packet_nr;
    uint8_t  packet_nr_inv;
    uint8_t  data[PX_XMODEM_DATA_SIZE];
    uint8_t  crc16_hi8;
    uint8_t  crc16_lo8;
} px_xmodem_packet_t;

/* _____LOCAL VARIABLES______________________________________________________ */
// Variable to keep track of current packet number
static uint8_t px_xmodem_packet_nr;

// Packet buffer
static union
{
    px_xmodem_packet_t packet;
    uint8_t            data[sizeof(px_xmodem_packet_t)];
} px_xmodem_packet;

/* _____LOCAL FUNCTIONS______________________________________________________ */
const char * px_modem_flow_char_to_str(uint8_t data)
{
    switch(data)
    {
    case PX_XMODEM_SOH : return "SOH";
    case PX_XMODEM_EOT : return "EOT";
    case PX_XMODEM_ACK : return "ACK";
    case PX_XMODEM_NAK : return "NAK";
    case PX_XMODEM_C :   return "C";
    default:             return "???";
    }
}

static bool px_xmodem_wait_rx_char(uint8_t * data)
{
    // See if character has been received
    while(!PX_XMODEM_CFG_RD_U8(data))
    {
        if(PX_XMODEM_CFG_TMR_HAS_EXPIRED())
        {
            // Timeout
            return false;
        }
    }
    return true;
}

static uint16_t px_xmodem_calc_checksum(void)
{
    uint8_t  i;
    uint8_t  j;
    uint8_t  data;
    uint16_t crc = 0x0000;

    // Repeat until all the data has been processed...
    for(i=0; i<PX_XMODEM_DATA_SIZE; i++)
    {
        data = px_xmodem_packet.packet.data[i];

        // XOR high byte of CRC with 8-bit data
        crc = crc ^ (((uint16_t)data)<<8);

        // Repeat 8 times (for each bit)
        for(j=8; j!=0; j--)
        {
            // Is highest bit set?
            if((crc & (1<<15)) != 0)
            {
                // Shift left and XOR with polynomial CRC16-CCITT (x^16 + x^12 + x^5 + x^0)
                crc = (crc << 1) ^ 0x1021;
            }
            else
            {
                // Shift left
                crc = (crc << 1);
            }
        }
    }
    return crc;
}

static bool px_xmodem_verify_checksum(uint16_t crc)
{
    // Compare received CRC with calculated value
    if(  (px_xmodem_packet.packet.crc16_hi8 != PX_U16_HI8(crc))
       ||(px_xmodem_packet.packet.crc16_lo8 != PX_U16_LO8(crc))  )
    {    
        PX_LOG_E("CRC Error");
        return false;
    }
    return true;
}

/**
 *  Blocking function with a timeout that tries to receive an XMODEM packet
 *  
 *  @retval true    Packet correctly received
 *  @retval false   Packet error
 */
static bool px_xmodem_rx_packet(void)
{
    uint8_t  i = 0; 
    uint8_t  data;

    // Start packet timeout
    PX_XMODEM_CFG_TMR_START(PX_XMODEM_CFG_TIMEOUT_MS);

    // Repeat until whole packet has been received
    for(i=0; i<sizeof(px_xmodem_packet.data); i++)
    {
        // See if character has been received
        if(!px_xmodem_wait_rx_char(&data))
        {
            // Timeout
            PX_LOG_E("Timeout");
            return false;
        }
        // Store received data in buffer
        px_xmodem_packet.data[i] = data;
        // Restart timer
        PX_XMODEM_CFG_TMR_START(PX_XMODEM_CFG_TIMEOUT_MS);
        // See if this is the first byte of a packet received (px_xmodem_packet.packet.start)
        if(i == 0)
        {
            // See if End Of Transmission has been received
            if(data == PX_XMODEM_EOT)
            {
                PX_LOG_D("EOT");
                return true;
            }
        }
    }
    // See if whole packet was received
    if(i != sizeof(px_xmodem_packet.data))
    {
        PX_LOG_E("Packet incomplete");
        return false;
    }
    // See if correct header was received
    if(px_xmodem_packet.packet.start != PX_XMODEM_SOH)
    {
        PX_LOG_E("Did not receive SOH");
        return false;
    }
    // Check packet number checksum
    if((px_xmodem_packet.packet.packet_nr + px_xmodem_packet.packet.packet_nr_inv) != 255)
    {
        PX_LOG_E("Packet number checksum error");
        return false;
    }
    // Verify Checksum
    return px_xmodem_verify_checksum(px_xmodem_calc_checksum());
}

static void px_xmodem_tx_packet(void)
{
    uint8_t  i; 
    uint16_t crc;

    // Start Of Header
    px_xmodem_packet.packet.start = PX_XMODEM_SOH;
    // Packet number
    px_xmodem_packet.packet.packet_nr = px_xmodem_packet_nr;
    // Inverse packet number
    px_xmodem_packet.packet.packet_nr_inv = 255 - px_xmodem_packet_nr;
    // Data already filled in...
    // Checksum
    crc = px_xmodem_calc_checksum();
    px_xmodem_packet.packet.crc16_hi8 = PX_U16_HI8(crc);
    px_xmodem_packet.packet.crc16_lo8 = PX_U16_LO8(crc);

    // Send whole packet
    for(i=0; i<sizeof(px_xmodem_packet.data); i++)
    {
        PX_XMODEM_CFG_WR_U8(px_xmodem_packet.data[i]);
    }
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
bool px_xmodem_receive_file(px_xmodem_on_rx_data_t on_rx_data)
{
    uint8_t retry          = PX_XMODEM_CFG_MAX_RETRIES_START;
    bool    first_ack_sent = false;

    // Reset packet number
    px_xmodem_packet_nr = 1;

    // Repeat until transfer is finished or error count is exceeded
    while(retry != 0)
    {
        // Decrement retry count
        retry--;

        if(!first_ack_sent)
        {
            // Send initial start character to start transfer (with CRC checking)
            PX_XMODEM_CFG_WR_U8(PX_XMODEM_C);
            PX_LOG_D("Sending 'C'");
        }

        // Try to receive a packet
        if(!px_xmodem_rx_packet())
        {
            if(first_ack_sent)
            {
                PX_XMODEM_CFG_WR_U8(PX_XMODEM_NAK);
                PX_LOG_D("Sending NAK");
            }
            continue;
        }
        // End Of Transfer received?
        if(px_xmodem_packet.packet.start == PX_XMODEM_EOT)
        {
            // Acknowledge EOT
            PX_LOG_D("Received EOT");
            PX_XMODEM_CFG_WR_U8(PX_XMODEM_ACK);
            PX_LOG_D("Sending ACK");
            break;
        }
        // Duplicate packet received?
        if(px_xmodem_packet.packet.packet_nr == (px_xmodem_packet_nr - 1))
        {
            // Acknowledge packet
            PX_LOG_W("Duplicate packet received");
            PX_XMODEM_CFG_WR_U8(PX_XMODEM_ACK);
            PX_LOG_D("Sending ACK");
            continue;
        }
        // Expected packet received?
        if(px_xmodem_packet.packet.packet_nr != px_xmodem_packet_nr)
        {
            // NAK packet
            PX_LOG_E("Packet number not expected");
            PX_XMODEM_CFG_WR_U8(PX_XMODEM_NAK);
            PX_LOG_D("Sending NAK");
            continue;
        }
        PX_LOG_D("Received packet %u", px_xmodem_packet_nr);
        // Pass received data on to handler
        (*on_rx_data)(&px_xmodem_packet.packet.data[0], 
                      sizeof(px_xmodem_packet.packet.data));
        // Acknowledge packet
        PX_XMODEM_CFG_WR_U8(PX_XMODEM_ACK);
        // Next packet
        px_xmodem_packet_nr++;
        // Reset retry count
        retry = PX_XMODEM_CFG_MAX_RETRIES;
        // First ACK sent
        first_ack_sent = true;
    }

    // Too many errors?
    if(retry == 0)
    {
        PX_LOG_E("Retry count exceeded");
        return false;
    }

    // See if more EOTs are received...
    while(retry--)
    {
        // Wait for a packet
        if(!px_xmodem_rx_packet())
        {
            break;
        }
        // End Of Transfer received?
        if(px_xmodem_packet.packet.start == PX_XMODEM_EOT)
        {
            // Acknowledge EOT
            PX_LOG_D("Received EOT");
            PX_XMODEM_CFG_WR_U8(PX_XMODEM_ACK);
            PX_LOG_D("Sending ACK");
        }
    }

    return true;
}

bool px_xmodem_send_file(px_xmodem_on_tx_data_t on_tx_data)
{
    uint8_t retry;
    uint8_t data;

    // Reset packet number
    px_xmodem_packet_nr = 1;

    // Wait for initial start character to start transfer (with CRC checking)
    PX_XMODEM_CFG_TMR_START(10000);
    if(!px_xmodem_wait_rx_char(&data))
    {
        PX_LOG_E("Timeout waiting for start character");
        return false;
    }
    if(data != PX_XMODEM_C)
    {
        PX_LOG_E("Did not receive 'C''");
        return false;
    }

    // Get next data block to send
    while((*on_tx_data)(&px_xmodem_packet.packet.data[0], sizeof(px_xmodem_packet.packet.data)))
    {
        // Try sending error packet until error count is exceeded
        retry = PX_XMODEM_CFG_MAX_RETRIES;
        while(retry != 0)
        {
            // Send packet
            PX_LOG_D("Sending packet %u", px_xmodem_packet_nr);
            px_xmodem_tx_packet();

            // Wait for a response (ACK, NAK or C)
            PX_XMODEM_CFG_TMR_START(PX_XMODEM_CFG_TIMEOUT_MS);
            if(px_xmodem_wait_rx_char(&data))
            {
                PX_LOG_D("Received %s", px_modem_flow_char_to_str(data));
                // Received an ACK?
                if(data == PX_XMODEM_ACK)
                {
                    // Packet has been correctly received
                    break;
                }
            }

            // Decrement retry count
            retry--;            
        }

        // retry exceeded?
        if(retry == 0)
        {
            PX_LOG_E("Retry count exceeded");
            // Abort transfer
            return false;
        }
        // Next packet number
        px_xmodem_packet_nr++;
    }

    // Finish transfer by sending EOT ("End Of Transfer")
    retry = PX_XMODEM_CFG_MAX_RETRIES;
    while(retry != 0)
    {
        // Send "End Of Transfer"
        PX_LOG_D("Sending EOT");
        PX_XMODEM_CFG_WR_U8(PX_XMODEM_EOT);        
        // Wait for response
        PX_XMODEM_CFG_TMR_START(PX_XMODEM_CFG_TIMEOUT_MS);
        if(px_xmodem_wait_rx_char(&data))
        {
            PX_LOG_D("Received %s", px_modem_flow_char_to_str(data));
            if(data == PX_XMODEM_ACK)
            {
                // File successfully transferred
                PX_LOG_D("Received ACK. Success");
                return true;
            }
        }
        retry--;
    }
    return false;
}
