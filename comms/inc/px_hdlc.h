#ifndef __PX_HDLC_H__
#define __PX_HDLC_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2006 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md

    Title:          px_hdlc.h : HDLC encapsulation layer
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31

============================================================================= */
/** 
 *  @ingroup COMMS
 *  @defgroup PX_HDLC px_hdlc.h : HDLC encapsulation layer
 *  
 *  This component encapsulates packets in HDLC-like frames.
 *  
 *  File(s):
 *  - comms/inc/px_hdlc.h
 *  - comms/src/px_hdlc.c
 *  
 *  The purpose of this module is detect the start and end of a data packet and
 *  if any errors occured during transmission.
 *  
 *  This is accomplished by appending a 16-bit CRC to the data and by adding a
 *  unique start and end marker to the packet (byte 0x7E or ASCII character '~').
 *  This allows a receiver to detect the start of a new packet, without using a
 *  timeout mechanism (no timers). This is very useful when communicating with
 *  a non real-time operating system, e.g. Windows.
 *  
 *  To make sure that 0x7E only occurs to mark the valid start and end of a 
 *  packet, any 0x7E bytes in the data is replaced with a [0x7D, 0x5E] sequence
 *  (known as "escaping"). 0x5E = 0x7E xor 0x20. Any data byte 0x7D must also 
 *  be "escaped", meaning it must be replaced with a [0x7D, 0x5D] sequence. 
 *  0x5D = 0x7D xor 0x20.
 *  
 *  For example, to transmit the following data:
 *  
 *      [0x01] [0x02] [0x7e] [0x03]
 *  
 *  The following packet will be generated:
 *  
 *      [0x7e] [0x01] [0x02] [0x7d] [0x5e] [0x03] [CRC-LO] [CRC-HI] [0x7e]
 *  
 *  In general any data byte can be escaped by replacing it with 0x7D and the 
 *  original data byte xor'd with 0x20 (toggling bit 5). This is useful if 
 *  software flow control is used with XON and XOFF characters and these 
 *  characters must not occur accidently in the data.
 *  
 *  @warn_s
 *  The overhead with escaping may mean that a packet may be up to 
 *  double in size if all of the data bytes are 0x7D or 0x7E, but the 
 *  probability of this sequence occuring is low.
 *  @warn_e
 *  
 *  Linking dependency to the higher communication layer is avoided by 
 *  passing a pointer to the function px_hdlc_init(). The pointer function 
 *  call overhead can be avoided by replacing a direct call to the 
 *  function handler if it is known at compile time. This means that 
 *  px_hdlc.c must be modifed to avoid a small processing overhead.
 *  
 *  Reference:
 *  - [RFC 1662 "PPP in HDLC-like Framing"](http://tools.ietf.org/html/rfc1662)
 *  
 *  Example:
 *  
 *  @include comms/test/px_hdlc_test.c
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stddef.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
#ifndef PX_HDLC_MRU
/// Receive Packet size (Maximum Receive Unit)
#define PX_HDLC_MRU    64
#endif

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Definition for a pointer to a function that will be called to send a byte
typedef void (*px_hdlc_tx_u8_fn_t)(uint8_t data);

/**
 *  Definition for a pointer to a function that will be called once a frame has
 *  been received.
 */
typedef void (*px_hdlc_on_rx_frame_fn_t)(const uint8_t * data, size_t nr_of_bytes);

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise HDLC encapsulation layer.
 *  
 * @param[in] tx_u8_fn          Pointer to a function that will be called to 
 *                              send a byte.
 * @param[in] on_rx_frame_fn    Pointer to function that is called when a
 *                              correct frame is received.
 */
void px_hdlc_init(px_hdlc_tx_u8_fn_t       tx_u8_fn,
                  px_hdlc_on_rx_frame_fn_t on_rx_frame_fn);

/**
 *  Function handler that is fed all raw received data.
 *  
 *  @param[in] data     received 8-bit data
 *  
 *  @retval true        Valid HDLC frame has been received
 *  @retval false       Valid HDLC frame has not been received yet
 *  
 */
bool px_hdlc_on_rx_u8(uint8_t data);

/**
 *  Encapsulate and send an HDLC frame.
 *  
 *  @param[in] data         Buffer containing data for transmission
 *  @param[in] nr_of_bytes  Number of bytes in buffer to be transmitted
 *  
 */
void px_hdlc_tx_frame(const uint8_t * data, size_t nr_of_bytes);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif
/// @}

#endif
