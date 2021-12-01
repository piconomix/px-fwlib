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

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_nrf905.h"
#include "px_board.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("px_nrf905");

#define PX_NRF905_INSTR_W_CONFIG       0x00    ///< Write Configuration register
#define PX_NRF905_INSTR_R_CONFIG       0x10    ///< Read Configuration register
#define PX_NRF905_INSTR_W_TX_PAYLOAD   0x20    ///< Write TX-payload
#define PX_NRF905_INSTR_R_TX_PAYLOAD   0x21    ///< Read TX-payload
#define PX_NRF905_INSTR_W_TX_ADDRESS   0x22    ///< Write TX-address
#define PX_NRF905_INSTR_R_TX_ADDRESS   0x23    ///< Read TX-address
#define PX_NRF905_INSTR_R_RX_PAYLOAD   0x24    ///< Read RX-payload
#define PX_NRF905_INSTR_CHANNEL_CONFIG 0x80    ///< set CH_NO, HFREQ_PLL and PA_PWR

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static px_spi_handle_t * px_nrf905_spi_handle;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_nrf905_init(px_spi_handle_t * handle)
{
    px_nrf905_spi_handle = handle;
}

uint8_t px_nrf905_cfg_wr(const px_nrf905_cfg_reg_t * cfg, uint8_t start_byte)
{
    uint8_t spi_data[1];

    PX_LOG_ASSERT(start_byte < (sizeof(px_nrf905_cfg_reg_t) - 1));

    // Send instruction
    spi_data[0] = PX_NRF905_INSTR_W_CONFIG | start_byte;
    px_spi_xc(px_nrf905_spi_handle, spi_data, spi_data, 1, PX_SPI_FLAG_START);

    // Write data
    px_spi_wr(px_nrf905_spi_handle,
              &((const uint8_t *)cfg)[start_byte],
              sizeof(px_nrf905_cfg_reg_t) - start_byte,
              PX_SPI_FLAG_STOP);

    // Return status register value
    return spi_data[0];
}

uint8_t px_nrf905_cfg_rd(px_nrf905_cfg_reg_t * cfg, uint8_t start_byte)
{
    uint8_t spi_data[1];

    PX_LOG_ASSERT(start_byte < (sizeof(px_nrf905_cfg_reg_t) - 1));

    // Send instruction
    spi_data[0] = PX_NRF905_INSTR_R_CONFIG | start_byte;
    px_spi_xc(px_nrf905_spi_handle, spi_data, spi_data, 1, PX_SPI_FLAG_START);

    // Read data
    px_spi_rd(px_nrf905_spi_handle,
              &((uint8_t *)cfg)[start_byte],
              sizeof(px_nrf905_cfg_reg_t) - start_byte,
              PX_SPI_FLAG_STOP);    

    // Return status register
    return spi_data[0];
}

uint8_t px_nrf905_tx_payload_wr(const void * data, uint8_t nr_of_bytes)
{
    uint8_t spi_data[1];

    PX_LOG_ASSERT( ((nr_of_bytes >= 1) && (nr_of_bytes <= 32)) );

    // Send instruction
    spi_data[0] = PX_NRF905_INSTR_W_TX_PAYLOAD;
    px_spi_xc(px_nrf905_spi_handle, spi_data, spi_data, 1, PX_SPI_FLAG_START);

    // Write data
    px_spi_wr(px_nrf905_spi_handle,
              data,
              nr_of_bytes,
              PX_SPI_FLAG_STOP);

    // Return status register value
    return spi_data[0];
}

uint8_t px_nrf905_tx_payload_rd(void * data, uint8_t nr_of_bytes)
{
    uint8_t spi_data[1];

    PX_LOG_ASSERT( ((nr_of_bytes >= 1) && (nr_of_bytes <= 32)) );

    // Send instruction
    spi_data[0] = PX_NRF905_INSTR_R_TX_PAYLOAD;
    px_spi_xc(px_nrf905_spi_handle, spi_data, spi_data, 1, PX_SPI_FLAG_START);

    // Read data
    px_spi_rd(px_nrf905_spi_handle,
              data,
              nr_of_bytes,
              PX_SPI_FLAG_STOP);    

    // Return status register
    return spi_data[0];
}

uint8_t px_nrf905_rx_payload_rd(void * data, uint8_t nr_of_bytes)
{
    uint8_t spi_data[1];

    PX_LOG_ASSERT( ((nr_of_bytes >= 1) && (nr_of_bytes <= 32)) );

    // Send instruction
    spi_data[0] = PX_NRF905_INSTR_R_RX_PAYLOAD;
    px_spi_xc(px_nrf905_spi_handle, spi_data, spi_data, 1, PX_SPI_FLAG_START);

    // Read data
    px_spi_rd(px_nrf905_spi_handle,
              data,
              nr_of_bytes,
              PX_SPI_FLAG_STOP);    

    // Return status register
    return spi_data[0];
}

uint8_t px_nrf905_channel_cfg(uint16_t channel_no, uint8_t hfreq_pll, uint8_t pa_pwr)
{
    uint8_t spi_data[2];

    PX_LOG_ASSERT(start_byte < (sizeof(px_nrf905_cfg_reg_t)-1));

    // Send instruction
    spi_data[0] =   PX_NRF905_INSTR_CHANNEL_CONFIG
                  | (pa_pwr     << 2)
                  | (hfreq_pll  << 1)
                  | (channel_no >> 8);
    spi_data[1] = channel_no & 0xff;
    px_spi_xc(px_nrf905_spi_handle, spi_data, spi_data, 2, PX_SPI_FLAG_START);

    // Return status register value
    return spi_data[0];
}
