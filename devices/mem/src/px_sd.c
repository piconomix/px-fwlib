/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2013 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md

    Title:          px_sd.h : SD Card Driver
    Author(s):      Pieter Conradie
    Creation Date:  2013-06-21

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_sd.h"
#include "px_spi.h"
#include "px_board.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("px_sd");

// SD SPI commands
#define PX_SD_CMD0_GO_IDLE_STATE           0   // Reset all cards to idle state
#define PX_SD_CMD1_SEND_OP_COND            1   // Send host capacitity support info and activate init
#define PX_SD_CMD6_SWITCH_FUNC             6   // Check switchable function and switches card function
#define PX_SD_CMD8_SEND_IF_COND            8   // Send interface condition
#define PX_SD_CMD9_SEND_CSD                9   // Request Card Specific Data
#define PX_SD_CMD10_SEND_CID               10  // Request Card Identification
#define PX_SD_CMD12_STOP_TRANSMISSION      12  // Stop transmission in Multiple Block Read operation
#define PX_SD_CMD13_SEND_STATUS            13  // Request status register
#define PX_SD_CMD16_SET_BLOCKLEN           16  // Set block length for SDSC cards; fixed for SDHC and SDXC cards
#define PX_SD_CMD17_READ_SINGLE_BLOCK      17  // Read a block of size selected with PX_SD_CMD16_SET_BLOCKLEN
#define PX_SD_CMD18_READ_MULT_BLOCK        18  // Continuously transfers data blocks until stopped with PX_SD_CMD12_STOP_TRANSMISSION
#define PX_SD_CMD24_WRITE_BLOCK            24  // Write a block of size selected with PX_SD_CMD16_SET_BLOCKLEN
#define PX_SD_CMD25_WRITE_MULT_BLOCK       25  // Continuously writes data blocks until stopped with 'Stop Tran' token
#define PX_SD_CMD27_PROGRAM_CSD            27  // Program programmable bits of the CSD
#define PX_SD_CMD28_SET_WRITE_PROT         28  // Sets write protection bits if supported by card
#define PX_SD_CMD29_CLR_WRITE_PROT         29  // Clears write protection bits if supported by card
#define PX_SD_CMD30_SEND_WRITE_PROT        30  // Requests status of write protection bits if supported by card
#define PX_SD_CMD32_ERASE_WR_BLK_START_ADR 32  // Sets the address of the first write block to be erased
#define PX_SD_CMD33_ERASE_WR_BLK_END_ADR   33  // Sets the address of the last write block to be erased
#define PX_SD_CMD38_ERASE                  38  // Erases all previously selected write blocks
#define PX_SD_CMD42_LOCK_UNLOCK            42  // Used to Set/Reset the password or lock/unlock the card
#define PX_SD_CMD48_READ_EXTR_SINGLE       48  // Used to read xtension register
#define PX_SD_CMD49_WRITE_EXTR_SINGLE      49  // Used to write extension register
#define PX_SD_CMD55_APP_CMD                55  // Next command is an application specific command
#define PX_SD_CMD56_GEN_CMD                56  // Used either to transfer a Data Block to the card or to get a Data Block from the card for general purpose/application specific commands
#define PX_SD_CMD58_READ_OCR               58  // Read the OCR register
#define PX_SD_CMD59_CRC_ON_OFF             59  // Turn the CRC option on or off


// SD SPI application specific command marker bit
#define PX_SD_ACMD_MARKER_BIT              7

// SD SPI application specific commands (after PX_SD_CMD55_APP_CMD)
#define PX_SD_ACMD13_SD_STATUS             ((1<<PX_SD_ACMD_MARKER_BIT) + 13) // Request the SD Status
#define PX_SD_ACMD22_SEND_NUM_WR_BLOCKS    ((1<<PX_SD_ACMD_MARKER_BIT) + 22) // Send the number of the well written (without errors) blocks
#define PX_SD_ACMD23_SET_WR_BLOCK_COUNT    ((1<<PX_SD_ACMD_MARKER_BIT) + 23) // Set the number of write blocks to be pre-erased before writing
#define PX_SD_ACMD41_SD_SEND_OP_COND       ((1<<PX_SD_ACMD_MARKER_BIT) + 41) // Send host capacity support information and activate init
#define PX_SD_ACMD42_SET_CLR_CARD_DETECT   ((1<<PX_SD_ACMD_MARKER_BIT) + 42) // Connect / Disconnect the 50 KOhm pull-up resistor on CS
#define PX_SD_ACMD51_SEND_SCR              ((1<<PX_SD_ACMD_MARKER_BIT) + 51) // Read the SD Configuration Register

// SD R1 response bits
#define PX_SD_RESP_R1_START_BIT            7   // Start bit of response is always '0'
#define PX_SD_RESP_R1_ERR_PARAM            6   // The command's argument (e.g. address, block length) was outside the allowed range for this card
#define PX_SD_RESP_R1_ERR_ADR              5   // A misaligned address that did not match the block length was used in the command
#define PX_SD_RESP_R1_ERR_ERASE_SEQ        4   // An error in the sequence of erase commands occurred
#define PX_SD_RESP_R1_ERR_COM_CRC          3   // The CRC check of the last command failed
#define PX_SD_RESP_R1_ILLEGAL_CMD          2   // An illegal command code was detected
#define PX_SD_RESP_R1_ERASE_RESET          1   // An erase sequence was cleared before executing because an out of erase sequence command was received
#define PX_SD_RESP_R1_IDLE_STATE           0   // The card is in idle state and running the initializing process

// Data block tokens
#define PX_SD_TOKEN_DATA_BLOCK_START           0xfe    // Start block token for Single Block Read (CMD17), Multiple Block Read (CMD18) and Single Block Write (CMD24
#define PX_SD_TOKEN_DATA_BLOCK_START_MULT_WR   0xfc    // Start block token for Multiple Block Write (CMD25)
#define PX_SD_TOKEN_DATA_BLOCK_STOP_MULT_WR    0xfd    // Stop transaction token for Multiple Block Write (CMD25)

// Data response token
#define PX_SD_TOKEN_DATA_RESP_MASK         0x1f
#define PX_SD_TOKEN_DATA_RESP_DATA_OK      0x05    // Data accepted
#define PX_SD_TOKEN_DATA_RESP_CRC_ERROR    0x0b    // Data rejected due to a CRC error
#define PX_SD_TOKEN_DATA_RESP_WRITE_ERROR  0x0d    // Data Rejected due to a Write Error

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static px_spi_handle_t * px_spi_handle_sd;
static uint8_t           px_sd_rx_data[4];
static px_sd_card_type_t px_sd_card_type;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static uint8_t px_sd_spi_exchange_u8(uint8_t data)
{
    uint8_t data_tx[1];
    uint8_t data_rx[1];

    data_tx[0] = data;
    px_spi_xc(px_spi_handle_sd, data_tx, data_rx, 1, 0);

    return data_rx[0];
}

static uint8_t px_sd_spi_rd_u8(void)
{
    return px_sd_spi_exchange_u8(0xff);
}

static void px_sd_spi_rd_data(void *data, size_t nr_of_bytes)
{
    uint8_t * data_u8 = (uint8_t *)data;
    while(nr_of_bytes != 0)
    {
        *data_u8++ = px_sd_spi_rd_u8();
        nr_of_bytes--;
    }
}

static void px_sd_spi_wr_u8(uint8_t data)
{
    px_sd_spi_exchange_u8(data);
}

static void px_sd_spi_wr_data(const void * data, size_t nr_of_bytes)
{
    const uint8_t * data_u8 = (uint8_t * )data;
    while(nr_of_bytes != 0)
    {
        px_sd_spi_exchange_u8(*data_u8++);
        nr_of_bytes--;
    }
}

static void px_sd_spi_cs_lo(void)
{
    px_spi_wr(px_spi_handle_sd, NULL, 0, PX_SPI_FLAG_START);
}

static void px_sd_spi_cs_end(void)
{
    // Deselect SD card to end previous transaction
    px_spi_wr(px_spi_handle_sd, NULL, 0, PX_SPI_FLAG_STOP);
    // Provide 8 clocks for SD Card state machine
    px_sd_spi_rd_u8();
}

static bool px_sd_wait_ready(void)
{
    uint16_t retry;

    // Wait up to 500 ms for SD card to be ready (not busy)
    for(retry = 5000; retry != 0; retry--)
    {
        if(px_sd_spi_rd_u8() == 0xff)
        {
            return true;
        }
        // Wait 100 us
        px_board_delay_us(100);
    }
    // Timed-out
    PX_LOG_E("Timed-out waiting for SD card to be ready");
    return false;
}

static uint8_t px_sd_tx_cmd_rx_resp_r1(uint8_t cmd, uint32_t arg)
{
    uint8_t r1;
    uint8_t retry;

    // application specific command?
    if(PX_BIT_IS_HI(cmd, PX_SD_ACMD_MARKER_BIT))
    {
        // Signal to the card that the next command is an application specific
        // command rather than a standard command.
        r1 = px_sd_tx_cmd_rx_resp_r1(PX_SD_CMD55_APP_CMD, 0);
    
        // start bit = 1 or any error bit set? (ignore idle state bit)
        if((r1 & ~(1<<PX_SD_RESP_R1_IDLE_STATE)) != 0)
        {
            PX_LOG_E("CMD55 received R1 = 0x%02x", r1);
            return r1;
        }
    
        // Remove marker
        PX_BIT_SET_LO(cmd, PX_SD_ACMD_MARKER_BIT);
    }

    // End previous transaction
    px_sd_spi_cs_end();

    PX_LOG_D("CMD%d(%08lX)", cmd, arg);

    // Select SD card to start next transaction
    px_sd_spi_cs_lo();
    // Dummy clock until card is ready (not busy).
    if(!px_sd_wait_ready())
    {
        // Card not ready
        return 0xff;
    }

    // Send command index with start bit = 0 (bit 47); transmission bit = 1 (bit 46)
    px_sd_spi_wr_u8(0x40 | cmd);

    // Send 32-bit argument
    px_sd_spi_wr_u8(PX_U32_HI8(arg));
    px_sd_spi_wr_u8(PX_U32_MH8(arg));
	px_sd_spi_wr_u8(PX_U32_ML8(arg));
    px_sd_spi_wr_u8(PX_U32_LO8(arg));

    // Send CRC and end bit = 1 (bit 0)
    switch(cmd)
    {
    case PX_SD_CMD0_GO_IDLE_STATE:
        // Valid 7-bit CRC for CMD0 (arg = 0x00000000)
        px_sd_spi_wr_u8(0x95);
        break;
    case PX_SD_CMD8_SEND_IF_COND:
        // Valid 7-bit CRC for CMD8 (arg = 0x000001aa)
        px_sd_spi_wr_u8(0x87);
        break;
    default:
        // Invalid 7-bit CRC for other commands
        px_sd_spi_rd_u8();
        break;
    }

    // Try 8 times to receive valid R1 response (start bit = 0)
    for(retry = 8; retry != 0; retry--)
    {
        r1 = px_sd_spi_rd_u8();

        // Start bit = 0?
        if(PX_BIT_IS_LO(r1, PX_SD_RESP_R1_START_BIT))
        {
            break;
        }
    }
    // Timeout?
    if(retry == 0)
    {
        PX_LOG_E("Timed out waiting for R1 response (received 0x%02x)", r1);
    }
    else
    {
        PX_LOG_D("R1 = 0x%02X", r1);
    }

    // Return response
    return r1;
}

static uint8_t px_sd_rx_data_block(uint8_t * data, size_t nr_of_bytes)
{
    uint16_t retry;
    uint8_t  data_token;
    uint8_t  px_crc_hi, px_crc_lo;

    // Receive data token with 100 ms timeout
    for(retry = 1000; retry != 0; retry--)
    {
        data_token = px_sd_spi_rd_u8();
        if(data_token != 0xff)
        {
            PX_LOG_D("Data token = %02hX", data_token);
            break;
        }
        // Wait 100 us
        px_board_delay_us(100);
    }
    // Timeout?
    if(retry == 0)
    {
        PX_LOG_E("Timed-out waiting for data token");
        return 0xff;
    }

    // Receive data block
    px_sd_spi_rd_data(data, nr_of_bytes);    

    // Receive CRC
    px_crc_hi = px_sd_spi_rd_u8();
    px_crc_lo = px_sd_spi_rd_u8();
    if(PX_LOG_LEVEL_I())
    {
        PX_LOG_D("CRC = %02hX %02hX", px_crc_hi, px_crc_lo);
    }
    else
    {
		// Eliminate compiler about unused variables
        (void)px_crc_hi;
        (void)px_crc_lo;
    }

    return data_token;
}

static uint8_t px_sd_tx_data_block(const uint8_t * data,
                                   size_t          nr_of_bytes,
                                   uint8_t         data_token_start)
{
    uint8_t data_token;

    // Wait for SD card to be ready (not busy)
    if(!px_sd_wait_ready())
    {
        return 0xff;
    }

    // Send start data token
    px_sd_spi_wr_u8(data_token_start);

    // Send data block
    px_sd_spi_wr_data(data, nr_of_bytes);    

    // Send dummy CRC
    px_sd_spi_rd_u8();
    px_sd_spi_rd_u8();
    
    // Return data response token
    data_token = px_sd_spi_rd_u8();
    PX_LOG_D("Data response token = 0x%02X", data_token);
    return data_token;
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_sd_init(px_spi_handle_t * handle)
{
    // Save SPI slave device handle
    px_spi_handle_sd = handle;    
}

bool px_sd_reset(void)
{
    uint8_t  r1;
    uint8_t  retry;
    uint16_t wait;

    // Card type not determined yet
    px_sd_card_type = PX_SD_CARD_TYPE_INVALID;

    // Change to bit rate equal or below 400 kHz
    px_spi_ioctl_change_baud(px_spi_handle_sd,
                             px_spi_util_baud_hz_to_clk_div(400000));

    // Wait 1 ms
    px_board_delay_us(1000);
    
    // Card requires at least 74 clocks to start up
    PX_LOG_D("80 Clocks");
    for(retry=10; retry!=0 ; retry--)
    {
        px_sd_spi_rd_u8();
    }

    // Execute CMD0+ (CMD0 with CS asserted) to put SD card into SPI operation mode
    for(retry = 100; retry != 0; retry--)
    {
        // Send CMD0+ (CMD0 with CS asserted)
        r1 = px_sd_tx_cmd_rx_resp_r1(PX_SD_CMD0_GO_IDLE_STATE, 0);
        // SD Card in idle state?
        if(r1 == (1<<PX_SD_RESP_R1_IDLE_STATE))
        {
            break;
        }
    }
    // Timeout?
    if(retry == 0)
    {
        PX_LOG_E("Unable to put SD Card in idle state (R1 = 0x%02X)", r1);
        px_sd_spi_cs_end();
        return false;
    }

    /*
     *  Execute CMD8 to initialise SD cards compliant with Physical Layer 
     *  Specification Version 2.00 or later.
     *  
     *  Voltage supplied (VHS) = 0001b (2.7-3.6V)
     *  Check pattern          = 0xaa
     */
    r1 = px_sd_tx_cmd_rx_resp_r1(PX_SD_CMD8_SEND_IF_COND, 0x000001aa);
    // Did card respond correctly?
    if(r1 == (1<<PX_SD_RESP_R1_IDLE_STATE))
    {
        // Ver 2.00 or later SD Card
        PX_LOG_D("Ver 2.00 or later SD Card");
        // Receive rest of R7 response
        px_sd_spi_rd_data(px_sd_rx_data, 4);        
        if(  (px_sd_rx_data[2] != 0x01)    // Voltage accepted 2.7-3.6V
           ||(px_sd_rx_data[3] != 0xaa)  ) // Verify check pattern
        {
            // Report error
            if(px_sd_rx_data[2] != 0x01)
            {
                PX_LOG_E("Voltage not accepted (received 0x%02X)", px_sd_rx_data[2]);
            }
            if(px_sd_rx_data[3] != 0xaa)
            {
                PX_LOG_E("Expected 0xaa, but received 0x%02X echo", px_sd_rx_data[3]);
            }
            px_sd_spi_cs_end();
            return false;
        }

        // Explicitely disable CRC, even though spec says that it is disabled by default
        r1 = px_sd_tx_cmd_rx_resp_r1(PX_SD_CMD59_CRC_ON_OFF, 0);
        if(r1 != (1<<PX_SD_RESP_R1_IDLE_STATE))
        {
            PX_LOG_E("Incorrect response to CMD59 (R1 = 0x%02X)", r1);
            px_sd_spi_cs_end();
            return false;
        }
        
        for(wait=1000; wait!=0; wait--)
        {
            /* 
             *  Send host capacity support information and activate the
             *  card's initialization process.
             *
             *  HCS = 1 (indicate that host supports SDHC or SDXC Card)
             */
            if(px_sd_tx_cmd_rx_resp_r1(PX_SD_ACMD41_SD_SEND_OP_COND, (1ul<<30)) == 0)
            {
                // Read the Operation Conditions Register (OCR) of the card
                r1 = px_sd_tx_cmd_rx_resp_r1(PX_SD_CMD58_READ_OCR, 0);
                // Init finished (not in idle state)?
                if(r1 == 0)
                {
                    // Receive rest of R3 response
                    px_sd_spi_rd_data(px_sd_rx_data, 4); 
                    PX_LOG_D("OCR = 0x%02x%02x%02x%02x",
                             px_sd_rx_data[0], px_sd_rx_data[1],
                             px_sd_rx_data[2], px_sd_rx_data[3]);
                    // Is Card Capacity Status (CCS) bit 30 set?
                    if(px_sd_rx_data[0] & (1<<6))
                    {
                        PX_LOG_D("Ver 2.00 HCSD or XCSD Card");
                        px_sd_card_type = PX_SD_CARD_TYPE_VER_2_HCSD_XCSD;
                    }
                    else
                    {
                        PX_LOG_D("Ver 2.00 SCSD Card");
                        px_sd_card_type = PX_SD_CARD_TYPE_VER_2_SCSD;
                    }
                }
                else
                {
                    PX_LOG_E("Incorrect response to CMD58 (R1 = 0x%02X)", r1);
                    px_sd_spi_cs_end();
                    return false;
                }
                break;
            }
            // Wait 1 ms
            px_board_delay_us(1000);
        }
        // Timed-out?
        if(wait == 0)
        {
            PX_LOG_E("Timed out waiting for ACMD41 init to finish");
            px_sd_spi_cs_end();
            return false;
        }
    }
    else
    {
        // Ver 1.xx SD Card (or not?)
        PX_LOG_D("Ver 1.xx SD Card (or not?)");

        // Explicitely disable CRC, even though spec says that it is disabled by default
        r1 = px_sd_tx_cmd_rx_resp_r1(PX_SD_CMD59_CRC_ON_OFF, 0);
        if(r1 != (1<<PX_SD_RESP_R1_IDLE_STATE))
        {
            PX_LOG_E("Incorrect response to CMD59 (R1 = 0x%02X)", r1);
            px_sd_spi_cs_end();
            return false;
        }

        for(wait=1000; wait!=0; wait--)
        {
            /* 
             *  Send host capacity support information and activate the
             *  card's initialization process.
             *
             *  HCS = 0
             */
            r1 = px_sd_tx_cmd_rx_resp_r1(PX_SD_ACMD41_SD_SEND_OP_COND, 0);
            // Init finished?
            if(r1 == 0)
            {
                break;
            }
            // Wait 1 ms
            px_board_delay_us(1000);
        }
        // Timed-out?
        if(wait == 0)
        {
            PX_LOG_E("Timed out waiting for ACMD41 init to finish (R1 = 0x%02X)", r1);
            px_sd_spi_cs_end();
            return false;
        }

        // Set R/W block length to 512 bytes
        r1 = px_sd_tx_cmd_rx_resp_r1(PX_SD_CMD16_SET_BLOCKLEN, PX_SD_BLOCK_SIZE);
        if (r1 != 0)
        {
            PX_LOG_E("Incorrect response to CMD16 (R1 = 0x%02X)", r1);
            px_sd_spi_cs_end();
            return false;
        }
        PX_LOG_D("Ver 1.xx SD Card");
        px_sd_card_type = PX_SD_CARD_TYPE_VER_1_SD;
    }

    // Change to maximum bit rate
    px_spi_ioctl_change_baud(px_spi_handle_sd, 
                             px_spi_util_baud_hz_to_clk_div(PX_SD_MAX_SPI_CLOCK_HZ));

    px_sd_spi_cs_end();
    return true;
}

bool px_sd_read_cid(px_sd_cid_t * cid)
{
    uint8_t r1;
    uint8_t data_token;

    // Send CMD10 to read CID
    r1 = px_sd_tx_cmd_rx_resp_r1(PX_SD_CMD10_SEND_CID, 0);

    // any error bit set?
    if(r1 != 0)
    {
        PX_LOG_E("Incorrect response to CMD10 (R1 = 0x%02X)", r1);
        px_sd_spi_cs_end();
        return false;
    }

    // Receive data block
    data_token = px_sd_rx_data_block((uint8_t *) cid, sizeof(*cid));
    if(data_token != PX_SD_TOKEN_DATA_BLOCK_START)
    {
        px_sd_spi_cs_end();
        return false;
    }

    px_sd_spi_cs_end();
    return true;
}

bool px_sd_read_csd(px_sd_csd_t * csd)
{
    uint8_t r1;
    uint8_t data_token;

    // Send CMD9 to read CSD
    r1 = px_sd_tx_cmd_rx_resp_r1(PX_SD_CMD9_SEND_CSD, 0);

    // any error bit set?
    if(r1 != 0)
    {
        PX_LOG_E("Incorrect response to CMD9 (R1 = 0x%02X)", r1);
        px_sd_spi_cs_end();
        return false;
    }

    // Receive data block
    data_token = px_sd_rx_data_block((uint8_t *) csd, sizeof(*csd));
    if(data_token != PX_SD_TOKEN_DATA_BLOCK_START)
    {
        PX_LOG_E("Incorrect data token (received 0x%02X)", data_token);
        px_sd_spi_cs_end();
        return false;
    }

    px_sd_spi_cs_end();

    if(PX_LOG_LEVEL_D())
    {
        PX_LOG_D("CSD:");
        PX_LOG_TRACE_DATA(csd, sizeof(px_sd_csd_t));
        PX_LOG_TRACE("\n");
    }
    return true;
}

bool px_sd_get_status(uint16_t * status)
{
    uint8_t  r1;
    uint8_t  r2;

    // Send CMD13 to get status
    r1 = px_sd_tx_cmd_rx_resp_r1(PX_SD_CMD13_SEND_STATUS, 0);

    // any error bit set?
    if(PX_BIT_IS_HI(r1, PX_SD_RESP_R1_START_BIT))
    {
        PX_LOG_E("Incorrect response to CMD13 (R1 = 0x%02X)", r1);
        px_sd_spi_cs_end();
        return false;
    }

    // Read second part of response
    r2 = px_sd_spi_rd_u8();

    // Combine R1 and R2
    *status = ((uint16_t)r1<<8) + r2;

    // End operation
    px_sd_spi_cs_end();
    return true;
}

uint32_t px_sd_get_capacity_in_blocks(const px_sd_csd_t * csd)
{
    uint32_t capacity = 0;
    uint8_t  n;

    PX_LOG_D("csd_structure = %02X", csd->csd_structure);
    if(csd->csd_structure == 0) // CSD Structure version 1.0?
    {
        PX_LOG_D("read_bl_len    = %02hX", csd->read_bl_len);
        PX_LOG_D("c_size_mult_hi = %02hX", csd->ver.csd_1_0.c_size_mult_hi);
        PX_LOG_D("c_size_mult_lo = %02hX", csd->ver.csd_1_0.c_size_mult_lo);

        // READ_BL_LEN + C_SIZE_MULT + 2
        n =   csd->read_bl_len 
            + (csd->ver.csd_1_0.c_size_mult_hi << 1)
            + csd->ver.csd_1_0.c_size_mult_lo + 2;

        PX_LOG_D("n = %02hX", n);

        PX_LOG_D("c_size_hi  = %02hX", csd->ver.csd_1_0.c_size_hi);
        PX_LOG_D("c_size_mid = %02hX", csd->ver.csd_1_0.c_size_mid);
        PX_LOG_D("c_size_lo  = %02hX", csd->ver.csd_1_0.c_size_lo);

        // C_SIZE + 1
        capacity = (   ((uint16_t)csd->ver.csd_1_0.c_size_hi << 10)
                     | ((uint16_t)csd->ver.csd_1_0.c_size_mid << 2)
                     | (csd->ver.csd_1_0.c_size_lo                )  )
                   + 1;

        PX_LOG_D("capacity = %08lX", capacity);

        // (C_SIZE + 1) * 2^(READ_BL_LEN + C_SIZE_MULT + 2) / 512
        capacity = capacity << (n - 9);

        PX_LOG_D("capacity = %08lX", capacity);
    }
    else if(csd->csd_structure == 1) // CSD Structure version 2.0?
    {
        PX_LOG_D("c_size_hi  = %02hX", csd->ver.csd_2_0.c_size_hi);
        PX_LOG_D("c_size_mid = %02hX", csd->ver.csd_2_0.c_size_mid);
        PX_LOG_D("c_size_lo  = %02hX", csd->ver.csd_2_0.c_size_lo);

        // C_SIZE + 1
        capacity =   ((uint32_t)csd->ver.csd_2_0.c_size_hi << 16)
                    | ((uint16_t)csd->ver.csd_2_0.c_size_mid << 8)
                    | (csd->ver.csd_2_0.c_size_lo + 1);

        PX_LOG_D("capacity = %08lX", capacity);

        // Memory capacity = (C_SIZE + 1) * 512 KByte
        // Size in blocks  = (C_SIZE + 1) * 512 KByte / 0.5 KByte
        capacity <<= 10;

        PX_LOG_D("capacity = %08lX", capacity);
    }

    return capacity;
}

bool px_sd_read_block(uint8_t * data, uint32_t block_adr)
{
    uint8_t r1;
    uint8_t data_token;

    PX_LOG_ASSERT(px_sd_card_type != PX_SD_CARD_TYPE_INVALID);

    // Standard Capacity card?
    if(px_sd_card_type != PX_SD_CARD_TYPE_VER_2_HCSD_XCSD)
    {
        // Multiply with block size of 512
        block_adr *= PX_SD_BLOCK_SIZE;
    }

    // Send CMD17 for single block read
    r1 = px_sd_tx_cmd_rx_resp_r1(PX_SD_CMD17_READ_SINGLE_BLOCK, block_adr);

    // any error bit set?
    if(r1 != 0)
    {
        PX_LOG_E("Incorrect response to CMD17 (R1 = 0x%02X)", r1);
        px_sd_spi_cs_end();
        return false;
    }

    // Receive data block
    data_token = px_sd_rx_data_block(data, PX_SD_BLOCK_SIZE);
    if(data_token != PX_SD_TOKEN_DATA_BLOCK_START)
    {
        px_sd_spi_cs_end();
        return false;
    }

    // Success
    px_sd_spi_cs_end();
    return true;
}

uint8_t px_sd_read_blocks(uint8_t * data, uint32_t block_adr, uint8_t nr_of_blocks)
{
    uint8_t r1;
    uint8_t data_token;
    uint8_t blocks_read;

    PX_LOG_ASSERT(px_sd_card_type != PX_SD_CARD_TYPE_INVALID);

    if(nr_of_blocks == 0)
    {
        return 0;
    }
    else if(nr_of_blocks == 1)
    {
        // Read single block
        if(px_sd_read_block(data, block_adr))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }

    // Standard Capacity card?
    if(px_sd_card_type != PX_SD_CARD_TYPE_VER_2_HCSD_XCSD)
    {
        // Multiply with block size of 512
        block_adr *= PX_SD_BLOCK_SIZE;
    }
    // Send CMD18 for multiple block read
    r1 = px_sd_tx_cmd_rx_resp_r1(PX_SD_CMD18_READ_MULT_BLOCK, block_adr);

    // any error bit set?
    if(r1 != 0)
    {
        PX_LOG_E("Incorrect response to CMD18 (R1 = 0x%02X)", r1);
        px_sd_spi_cs_end();
        return 0;
    }

    for(blocks_read = 0; blocks_read < nr_of_blocks; blocks_read++)
    {
        // Receive data block
        data_token = px_sd_rx_data_block(data, PX_SD_BLOCK_SIZE);
        if(data_token != PX_SD_TOKEN_DATA_BLOCK_START)
        {
            break;
        }
        // Next block
        data += PX_SD_BLOCK_SIZE;
    }

    // Send CMD12 to stop multiple block read operation
    px_sd_tx_cmd_rx_resp_r1(PX_SD_CMD12_STOP_TRANSMISSION, 0);

    px_sd_spi_cs_end();
    return blocks_read;
}

bool px_sd_write_block(const uint8_t * data, uint32_t block_adr)
{
    uint8_t r1;
    uint8_t data_resp_token;

    PX_LOG_ASSERT(px_sd_card_type != PX_SD_CARD_TYPE_INVALID);

    // Standard Capacity card?
    if(px_sd_card_type != PX_SD_CARD_TYPE_VER_2_HCSD_XCSD)
    {
        // Multiply with block size of 512
        block_adr *= PX_SD_BLOCK_SIZE;
    }

    // Send CMD24 for single block write
    r1 = px_sd_tx_cmd_rx_resp_r1(PX_SD_CMD24_WRITE_BLOCK, block_adr);

    // any error bit set?
    if(r1 != 0)
    {
        PX_LOG_E("Incorrect response to CMD24 (R1 = 0x%02X)", r1);
        px_sd_spi_cs_end();
        return false;
    }

    // Send data block
    data_resp_token = px_sd_tx_data_block(data, PX_SD_BLOCK_SIZE, PX_SD_TOKEN_DATA_BLOCK_START);    

    // data token correct?
    if((data_resp_token & PX_SD_TOKEN_DATA_RESP_MASK) != PX_SD_TOKEN_DATA_RESP_DATA_OK)
    {
        PX_LOG_E("Data response token = 0x%02X", data_resp_token);
        px_sd_spi_cs_end();
        return false;
    }    

    // Success
    px_sd_spi_cs_end();
    return true;
}

uint8_t px_sd_write_blocks(const uint8_t * data, uint32_t block_adr, uint8_t nr_of_blocks)
{
    uint8_t r1;
    uint8_t data_resp_token;
    uint8_t blocks_written;

    PX_LOG_D("px_sd_write_blocks(%04X, %08lX, %hd)", data, block_adr, nr_of_blocks);
    PX_LOG_ASSERT(px_sd_card_type != PX_SD_CARD_TYPE_INVALID);


    if(nr_of_blocks == 0)
    {
        return 0;
    }
    else if(nr_of_blocks == 1)
    {
        // Write single block
        if(px_sd_write_block(data, block_adr))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    // Standard Capacity card?
    if(px_sd_card_type != PX_SD_CARD_TYPE_VER_2_HCSD_XCSD)
    {
        // Multiply with block size of 512
        block_adr *= PX_SD_BLOCK_SIZE;
    }
    // Send CMD25 for multiple block write
    r1 = px_sd_tx_cmd_rx_resp_r1(PX_SD_CMD25_WRITE_MULT_BLOCK, block_adr);

    // any error bit set?
    if(r1 != 0)
    {
        PX_LOG_E("Incorrect response to CMD25 (R1 = 0x%02X)", r1);
        px_sd_spi_cs_end();
        return 0;
    }

    // Send ACMD23 to set block erase count to number of block about to be written
    r1 = px_sd_tx_cmd_rx_resp_r1(PX_SD_ACMD23_SET_WR_BLOCK_COUNT, nr_of_blocks);

    // any error bit set?
    if(r1 != 0)
    {
        PX_LOG_E("Incorrect response to ACMD23 (R1 = 0x%02X)", r1);
        px_sd_spi_cs_end();
        return 0;
    }

    for(blocks_written = 0; blocks_written < nr_of_blocks; blocks_written++)
    {
        // Send data block
        data_resp_token = px_sd_tx_data_block(data, PX_SD_BLOCK_SIZE, PX_SD_TOKEN_DATA_BLOCK_START_MULT_WR);
    
        // data token correct?
        if((data_resp_token & PX_SD_TOKEN_DATA_RESP_MASK) != PX_SD_TOKEN_DATA_RESP_DATA_OK)
        {
            PX_LOG_E("Data response token = 0x%02X", data_resp_token);

            // Send CMD12 to stop multiple block write operation
            px_sd_tx_cmd_rx_resp_r1(PX_SD_CMD12_STOP_TRANSMISSION, 0);

            px_sd_spi_cs_end();
            return blocks_written;
        }

        // Next block
        data += PX_SD_BLOCK_SIZE;
    }

    // Wait for SD card to be ready (not busy)
    px_sd_wait_ready();

    // Send stop data token
    px_sd_spi_exchange_u8(PX_SD_TOKEN_DATA_BLOCK_STOP_MULT_WR);
    px_sd_spi_cs_end();

    PX_LOG_D("%hd block(s) written", blocks_written);
    return blocks_written;
}

bool px_sd_wait_write_finished(void)
{
    bool flag;

    // Select SD card
    px_spi_wr(px_spi_handle_sd, NULL, 0, PX_SPI_FLAG_START);
    // Dummy clock until card is ready (not busy).
    flag = px_sd_wait_ready();
    px_spi_wr(px_spi_handle_sd, NULL, 0, PX_SPI_FLAG_STOP);

    return flag;
}
