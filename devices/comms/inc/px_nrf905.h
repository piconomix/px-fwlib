#ifndef __PX_NRF905_H__
#define __PX_NRF905_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2013 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
 
    Title:          px_nrf905.h : Nordic Semiconductor nRF905 Transceiver driver
    Author(s):      Pieter Conradie
    Creation Date:  2013-07-28

============================================================================= */

/** 
 *  @ingroup DEVICES_COMMS
 *  @defgroup PX_NRF905 px_nrf905.h : Nordic Semiconductor nRF905 RF Transceiver driver
 *  
 *  File(s):
 *  - devices/comms/inc/px_nrf905.h
 *  - devices/comms/src/px_nrf905.c 
 *   
 *  Reference: 
 *  -  [Nordic nRF905] (https://www.nordicsemi.com/eng/Products/Sub-1-GHz-RF/nRF905) datasheet
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_spi.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
/// Maximum SPI Clock rate
#define PX_NRF905_MAX_SPI_CLOCK_HZ     10000000ul
/// SPI Clock / Data phase
#define PX_NRF905_SPI_MODE             PX_SPI_MODE0
/// SPI Data order
#define PX_NRF905_SPI_DATA_ORDER       PX_SPI_DATA_ORDER_MSB

/// Size of each data block
#define PX_NRF905_MIN_PAYLOAD_SIZE     1
#define PX_NRF905_MAX_PAYLOAD_SIZE     32

/// @name RF - Configuration register settings
/// @{
#define PX_NRF905_HFREQ_PLL_433MHZ     0   ///< 433 MHz
#define PX_NRF905_HFREQ_PLL_868_915MHZ 1   ///< 868 / 915 MHz

#define PX_NRF905_PA_PWR_M10DBM        0   ///< -10 dBm
#define PX_NRF905_PA_PWR_M2DBM         1   ///< -2  dBm
#define PX_NRF905_PA_PWR_P6DBM         2   ///< +6  dBm
#define PX_NRF905_PA_PWR_P10DBM        3   ///< +10 dBm

#define PX_NRF905_RX_RED_PWR_NORMAL    0   ///< Normal operation
#define PX_NRF905_RX_RED_PWR_ACTIVE    1   ///< Reduced power

#define PX_NRF905_AUTO_RETRAN_NO       0   ///< No retransmission
#define PX_NRF905_AUTO_RETRAN_YES      1   ///< Retransmission of data packet

#define PX_NRF905_UP_CLK_FREQ_4MHZ     0   ///< 4 MHz
#define PX_NRF905_UP_CLK_FREQ_2MHZ     1   ///< 2 MHz
#define PX_NRF905_UP_CLK_FREQ_1MHZ     2   ///< 1 MHz
#define PX_NRF905_UP_CLK_FREQ_500KHZ   3   ///< 500 kHz

#define PX_NRF905_UP_CLK_EN_NO         0   ///< No external clock signal available
#define PX_NRF905_UP_CLK_EN_YES        1   ///< External clock signal enabled

#define PX_NRF905_XOF_4MHZ             0   ///< 4 MHz
#define PX_NRF905_XOF_8MHZ             1   ///< 8 MHz
#define PX_NRF905_XOF_12MHZ            2   ///< 12 MHz
#define PX_NRF905_XOF_16MHZ            3   ///< 16 MHz
#define PX_NRF905_XOF_20MHZ            4   ///< 20 MHz

#define PX_NRF905_CRC_EN_NO            0   ///< Disable CRC check
#define PX_NRF905_CRC_EN_YES           1   ///< Enable CRC check

#define PX_NRF905_CRC_MODE_8BITS       0   ///< 8-bit CRC check
#define PX_NRF905_CRC_MODE_16BITS      1   ///< 16-bit CRC check
/// @}

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// NRF905 Configuration register map
typedef struct
{
    // Byte 0
    uint8_t ch_no_7_0;       ///< Set center frequency (together with HFREQ_PLL)

    // Byte 1
    uint8_t ch_no_8     : 1; ///< Set center frequency (together with HFREQ_PLL)
    uint8_t hfreq_pll   : 1; ///< Set PLL in 433 or 868/915MHz mode
    uint8_t pa_pwr      : 2; ///< Output power
    uint8_t rx_red_pwr  : 1; ///< Reduce current in RX mode by 1.6mA (sensitivity also is reduced)
    uint8_t auto_retran : 1; ///< Retransmit contents in TX register if TRX_CE and TXEN are high
    uint8_t reserved1   : 2;

    // Byte 2
    uint8_t rx_afw      : 3; ///< RX-address width
    uint8_t reserved2   : 1;
    uint8_t tx_afw      : 3; ///< TX-address width
    uint8_t reserved3   : 1;

    // Byte 3
    uint8_t rx_pw       : 6; ///< RX-payload width
    uint8_t reserved4   : 2;

    // Byte 4
    uint8_t tx_pw       : 6; ///< TX-payload width
    uint8_t reserved5   : 2;

    // Byte 5
    uint8_t rx_adr_0;        ///< RX address identity byte 0
    uint8_t rx_adr_1;        ///< RX address identity byte 1
    uint8_t rx_adr_2;        ///< RX address identity byte 2
    uint8_t rx_adr_3;        ///< RX address identity byte 3

    // Byte 9
    uint8_t up_clk_freq : 2; ///< Output clock frequency
    uint8_t up_clk_en   : 1; ///< Output clock enable
    uint8_t xof         : 3; ///< Crystal oscillator frequency
    uint8_t px_crc_en   : 1; ///< CRC – check enable
    uint8_t px_crc_mode : 1; ///< CRC – mode
} px_nrf905_cfg_reg_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
void   px_nrf905_init             (px_spi_handle_t * handle);
uint8_t px_nrf905_config_write    (const px_nrf905_cfg_reg_t * cfg, uint8_t start_byte);
uint8_t px_nrf905_config_read     (px_nrf905_cfg_reg_t * cfg, uint8_t start_byte);
uint8_t px_nrf905_tx_payload_write(const void * data, uint8_t nr_of_bytes);
uint8_t px_nrf905_tx_payload_read (void * data, uint8_t nr_of_bytes);
uint8_t px_nrf905_rx_payload_read (void * data, uint8_t nr_of_bytes);
uint8_t px_nrf905_channel_config  (uint16_t channel_no, uint8_t hfreq_pll, uint8_t pa_pwr);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
