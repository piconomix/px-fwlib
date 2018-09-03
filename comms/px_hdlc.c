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

    Title:          px_hdlc.h : HDLC encapsulation layer
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31

============================================================================= */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_hdlc.h"
#include "px_crc.h"

#include "px_dbg.h"
PX_DBG_DECL_NAME("px_hdlc")

/* _____LOCAL DEFINITIONS____________________________________________________ */
// Significant octet values
#define PX_HDLC_FLAG_SEQUENCE  0x7e   // Flag Sequence
#define PX_HDLC_CONTROL_ESCAPE 0x7d   // Asynchronous Control Escape
#define PX_HDLC_ESCAPE_BIT     0x20   // Asynchronous transparency modifier

/* _____MACROS_______________________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static uint8_t   px_hdlc_rx_frame[PX_HDLC_MRU];
static uint8_t   px_hdlc_rx_frame_index;
static px_crc_t  px_hdlc_rx_frame_fcs;
static bool px_hdlc_rx_esc_flag;

/// Pointer to the function that will be called to send a character
static px_hdlc_tx_u8_fn_t px_hdlc_tx_u8_fn;

/// Pointer to the function that will be called to process a received HDLC frame
static px_hdlc_on_rx_frame_fn_t px_hdlc_on_rx_frame_fn;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
/// Function to send a byte
static void px_hdlc_tx_u8(uint8_t data)
{
    // Call provided function
    (*px_hdlc_tx_u8_fn)(data);
}

/// Function to send an escaped byte
static void px_hdlc_esc_tx_u8(uint8_t data)
{
    // See if data should be escaped
    if((data == PX_HDLC_CONTROL_ESCAPE) || (data == PX_HDLC_FLAG_SEQUENCE))
    {
        // Send control escape byte
        px_hdlc_tx_u8(PX_HDLC_CONTROL_ESCAPE);
        // Toggle escape bit
        data ^= PX_HDLC_ESCAPE_BIT;
    }
    
    // Send data
    px_hdlc_tx_u8(data);
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_hdlc_init(px_hdlc_tx_u8_fn_t       tx_u8_fn,
                  px_hdlc_on_rx_frame_fn_t on_rx_frame_fn)
{
    px_hdlc_rx_frame_index = 0;
    px_hdlc_rx_frame_fcs   = PX_CRC_INIT_VAL;
    px_hdlc_rx_esc_flag    = false;
    px_hdlc_tx_u8_fn       = tx_u8_fn;
    px_hdlc_on_rx_frame_fn = on_rx_frame_fn;
}

void px_hdlc_on_rx_u8(uint8_t data)
{
    // Start/End sequence?
    if(data == PX_HDLC_FLAG_SEQUENCE)
    {
        // If Escape sequence + End sequence is received then this packet must 
        // be silently discarded.
        if(px_hdlc_rx_esc_flag == true)
        {
            // Reset flag
            px_hdlc_rx_esc_flag = false;
        }
        // Minimum requirement for a valid frame is reception of good FCS
        else if(  (px_hdlc_rx_frame_index >= sizeof(px_hdlc_rx_frame_fcs)) 
                &&(px_hdlc_rx_frame_fcs   == PX_CRC_MAGIC_VAL            )  )
        {
            // Pass on frame with FCS field removed
            (*px_hdlc_on_rx_frame_fn)(px_hdlc_rx_frame,
                                   px_hdlc_rx_frame_index - sizeof(px_hdlc_rx_frame_fcs));
        }
        // Reset for next packet
        px_hdlc_rx_frame_index = 0;
        px_hdlc_rx_frame_fcs   = PX_CRC_INIT_VAL;
        return;
    }

    // Escape sequence processing?
    if(px_hdlc_rx_esc_flag)
    {
        // Reset flag
        px_hdlc_rx_esc_flag  = false;
        // Toggle escape bit to restore data to correct state
        data             ^= PX_HDLC_ESCAPE_BIT;
    }
    else if(data == PX_HDLC_CONTROL_ESCAPE)
    {
        // Set flag to indicate that the next byte's escape bit must be toggled
        px_hdlc_rx_esc_flag = true;
        // Discard control escape byte (do not buffer it)
        return;
    }

    // Store received data
    px_hdlc_rx_frame[px_hdlc_rx_frame_index] = data;

    // Update checksum
    px_hdlc_rx_frame_fcs = px_crc_update_u8(px_hdlc_rx_frame_fcs, data);

    // Advance to next position in buffer
    px_hdlc_rx_frame_index++;

    // Check for buffer overflow
    if(px_hdlc_rx_frame_index >= PX_HDLC_MRU)
    {
        // Wrap index
        px_hdlc_rx_frame_index  = 0;

        // Invalidate FCS so that packet will be rejected
        px_hdlc_rx_frame_fcs  ^= 0xFFFF;
    }
}

void px_hdlc_tx_frame(const uint8_t * data, size_t nr_of_bytes)
{
    uint8_t  data_u8;
    px_crc_t fcs = PX_CRC_INIT_VAL;    

    // Send start marker
    px_hdlc_tx_u8(PX_HDLC_FLAG_SEQUENCE);

    // Send escaped data
    while(nr_of_bytes)
    {
        // Get next byte
        data_u8 = *data++;
        
        // Update checksum
        fcs = px_crc_update_u8(fcs, data_u8);
        
        // ESC send data
        px_hdlc_esc_tx_u8(data_u8);
                
        // decrement counter
        nr_of_bytes--;
    }

    // Invert checksum
    fcs ^= 0xffff;

    // Low byte of inverted FCS
    data_u8 = PX_U16_LO8(fcs);
    // ESC send data
    px_hdlc_esc_tx_u8(data_u8);

    // High byte of inverted FCS
    data_u8 = PX_U16_HI8(fcs);
    // ESC send data
    px_hdlc_esc_tx_u8(data_u8);

    // Send end marker
    px_hdlc_tx_u8(PX_HDLC_FLAG_SEQUENCE);    
}

