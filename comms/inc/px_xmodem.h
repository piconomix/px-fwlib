#ifndef __PX_XMODEM_H__
#define __PX_XMODEM_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2007 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_xmodem.h : XMODEM-CRC receive module
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31

============================================================================= */

/** 
 *  @ingroup COMMS
 *  @defgroup PX_XMODEM px_xmodem.h : XMODEM-CRC receive module
 *  
 *  Receive or send a file via the XMODEM-CRC protocol. 
 *  
 *  File(s):
 *  - comms/inc/px_xmodem.h
 *  - comms/inc/px_xmodem_cfg_template.h
 *  - comms/src/px_xmodem.c
 *  
 *  XMODEM-CRC is a simple file transfer protocol. The file is broken up into a 
 *  series of packets that are sent to the receiver.
 *  
 *  Each packet consists of:
 *  - a SOH character
 *  - a "block number" from 0-255
 *  - the "inverse" block number
 *  - 128 bytes of data
 *  - 16-bit CRC (in big endian order: CRC16-HI then CRC16-LO)
 *  
 *  A single character response is sent by the receiver to control the flow of
 *  packets:
 *  - 'C' [0x43] : ASCII 'C' to start an XMODEM transfer using CRC
 *  - ACK [0x06] : Acknowledge 
 *  - NAK [0x15] : Not Acknowledge
 *  
 *  To indicate the end of transfer, the transmitter sends:
 *  - EOT [0x04] : End of Transfer
 *  
 *  @see http://en.wikipedia.org/wiki/XMODEM
 *  
 *  This module requires a few functions that must be defined externally.
 *  Macros are used to bind to these functions (for flexibility and optimisation)
 *  and must be defined in 'px_xmodem_cfg.h'.
 *
 *  @{
 */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"

// Include project specific configuration. See "px_xmodem_cfg_template.h"
#include "px_xmodem_cfg.h"

// Check that all project specific options have been correctly specified
#if (   !defined(PX_XMODEM_CFG_MAX_RETRIES      ) \
     || !defined(PX_XMODEM_CFG_MAX_RETRIES_START) \
     || !defined(PX_XMODEM_CFG_TIMEOUT_MS       ) \
     || !defined(PX_XMODEM_CFG_WR_U8            ) \
     || !defined(PX_XMODEM_CFG_RD_U8            ) \
     || !defined(PX_XMODEM_CFG_TMR_START        ) \
     || !defined(PX_XMODEM_CFG_TMR_HAS_EXPIRED  )  )
#error "One or more options not defined in 'px_xmodem_cfg.h'"
#endif

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */
/** 
 *  Definition for a pointer to a function that will be called once a block of
 *  data has been received.
 */
typedef void (*px_xmodem_on_rx_data_t)(const uint8_t *data, uint8_t bytes_received);

/**
 *  Definition for a pointer to a function that will be called to supply
 *  data to send.
 */
typedef bool (*px_xmodem_on_tx_data_t)(uint8_t *data, uint8_t bytes_to_send);

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Blocking function that receives a file using the XMODEM-CRC protocol.
 *  
 *  @param on_rx_data   Pointer to a function that will be called once a block of
 *                      data has been received.
 *  
 *  @retval true        File succesfully received
 *  @retval false       Timed out while trying to receive a file
 */
bool px_xmodem_receive_file(px_xmodem_on_rx_data_t on_rx_data);

/**
 *  Blocking function that sends a file using the XMODEM-CRC protocol.
 *  
 *  @param on_tx_data   pointer to a function that will be called to supply
 *                      data to send. 
 *  
 *  @retval true        File succesfully sent
 *  @retval false       Timed out while trying to send a file
 */
bool px_xmodem_send_file(px_xmodem_on_tx_data_t on_tx_data);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
