#ifndef __PX_SD_H__
#define __PX_SD_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2013 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          px_sd.h : SD Card Driver
    Author(s):      Pieter Conradie
    Creation Date:  2013-06-21

============================================================================= */
/** 
 *  @ingroup DEVICES_MEM
 *  @defgroup PX_SD px_sd.h : SD Card Driver
 *  
 *  Driver that communicates with an SD (Secure Digital) Card using SPI.
 *  
 *  File(s):
 *  - devices/mem/inc/px_sd.h
 *  - devices/mem/src/px_sd_spi.c
 *  
 *  Reference:
 *  - 1. [SD Specifications Part 1 Physical Layer Simplified Specification 4.10](https://www.sdcard.org/downloads/pls/simplified_specs)
 *  - 2. [ChaN's How to Use MMC/SDC](http://elm-chan.org/docs/mmc/mmc_e.html)
 *  - 3. [Maxim - SD Media Format Expands the MAXQ2000's Space for Nonvolatile Data Storage](http://www.maximintegrated.com/app-notes/index.mvp/id/3969)
 *
 *  @{
 */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"
#include "px_spi.h"

#ifdef __cplusplus
extern "C" {
#endif
/* __DEFINITIONS ____________________________________________________________ */
/// Maximum SPI Clock rate
#define PX_SD_MAX_SPI_CLOCK_HZ  25000000ul
/// SPI Clock / Data phase
#define PX_SD_SPI_MODE          PX_SPI_MODE0
/// SPI Data order
#define PX_SD_SPI_DATA_ORDER    PX_SPI_DATA_ORDER_MSB
/// SPI Master Out dummy byte
#define PX_SD_SPI_MO_DUMMY_BYTE 0xff

/// Size of each data block
#define PX_SD_BLOCK_SIZE        512ul

/* _____TYPE DEFINITIONS_____________________________________________________ */
typedef enum
{
    PX_SD_CARD_TYPE_INVALID          = 0,
    PX_SD_CARD_TYPE_VER_1_SD         = 1,
    PX_SD_CARD_TYPE_VER_2_SCSD       = 2,
    PX_SD_CARD_TYPE_VER_2_HCSD_XCSD  = 3,
} px_sd_card_type_t;

/// CID - Card ID register; Ref 1. Paragraph "5.2 CID Register", page 113
typedef struct PX_ATTR_PACKED
{
    uint8_t  mid;           ///< Manufacturer ID
    char     oid[2];        ///< OEM/Application ID [2 ASCII chars]
    char     pnm[5];        ///< Product name [5 ASCII chars]
    uint8_t  prv;           ///< BCD Product revision [Major.Minor]
    uint32_t psn;           ///< Product serial number    
    uint8_t  year_hi : 4;   ///< Manufacturing date [xYYM]
    uint8_t  res1 : 4;
    uint8_t  year_lo : 4;   ///< Manufacturing date [xYYM]
    uint8_t  month : 4;     ///< Manufacturing date [xYYM]
    uint8_t  crc7_end_bit;
} px_sd_cid_t;

/// CSD - Card Specific Data register; Paragraph "5.3 CSD Register", page 114
typedef struct PX_ATTR_PACKED
{
    // Byte 0
    uint8_t reserved1 : 6;
    uint8_t csd_structure : 2;         ///< CSD structure version 1.0 or 2.0
    // Byte 1
    uint8_t taac;                      ///< data read access-time-1
    // Byte 2
    uint8_t nsac;                      ///< data read access-time-2 in CLK cycles (NSAC*100)
    // Byte 3
    uint8_t tran_speed;                ///< max. data transfer rate
    // Byte 4
    uint8_t ccc_hi;                    ///< card command classes
    // Byte 5
    uint8_t read_bl_len : 4;           ///< max. read data block length
    uint8_t ccc_lo : 4;                ///< card command classes

    union
    {
        struct
        {
            // Byte 6
            uint8_t c_size_hi : 2;             ///< device size
            uint8_t reserved2 : 2;
            uint8_t dsr_imp : 1;               ///< DSR implemented
            uint8_t read_blk_misalign : 1;     ///< read block misalignment
            uint8_t write_blk_misalign : 1;    ///< write block misalignment
            uint8_t read_bl_partial : 1;       ///< partial blocks for read allowed
            // Byte 7
            uint8_t c_size_mid;                ///< device size
            // Byte 8
            uint8_t vdd_r_curr_max : 3;        ///< max. read current at VDD max
            uint8_t vdd_r_curr_min : 3;        ///< max. read current at VDD min
            uint8_t c_size_lo : 2;             ///< device size
            // Byte 9
            uint8_t c_size_mult_hi : 2;        ///< device size multiplier
            uint8_t vdd_w_cur_max : 3;         ///< max. write current at VDD max
            uint8_t vdd_w_curr_min : 3;        ///< max. write current at VDD min
            // Byte 10
            uint8_t sector_size_hi : 6;        ///< erase sector size
            uint8_t erase_blk_en : 1;          ///< erase single block enable
            uint8_t c_size_mult_lo : 1;        ///< device size multiplier
            // Byte 11
            uint8_t wp_grp_size : 7;           ///< write protect group size
            uint8_t sector_size_lo : 1;        ///< erase sector size
        } csd_1_0;

        struct
        {
            // Byte 6
            uint8_t reserved2 : 4;
            uint8_t dsr_imp : 1;               ///< DSR implemented
            uint8_t read_blk_misalign : 1;     ///< read block misalignment
            uint8_t write_blk_misalign : 1;    ///< write block misalignment
            uint8_t read_bl_partial : 1;       ///< partial blocks for read allowed
            // Byte 7
            uint8_t c_size_hi : 6;             ///< device size
            uint8_t reserved3 : 2;
            // Byte 8
            uint8_t c_size_mid : 8;            ///< device size
            // Byte 9
            uint8_t c_size_lo : 8;             ///< device size
            // Byte 10
            uint8_t sector_size_hi : 6;        ///< erase sector size
            uint8_t erase_blk_en : 1;          ///< erase single block enable
            uint8_t reserved4 : 1;
            // Byte 11
            uint8_t wp_grp_size : 7;           ///< write protect group size
            uint8_t sector_size_lo : 1;        ///< erase sector size
        } csd_2_0;
    } ver;

    // Byte 12
    uint8_t write_bl_len_hi : 2;       ///< max. write data block length
    uint8_t r2w_factor : 3;            ///< write speed factor
    uint8_t reserved3 : 2;
    uint8_t wp_grp_enable : 1;         ///< write protect group enable
    // Byte 13
    uint8_t reserved4 : 5;
    uint8_t write_partial : 1;         ///< partial blocks for write allowed
    uint8_t write_bl_len_low : 2;      ///< max. write data block length
    // Byte 14
    uint8_t reserved5: 2;
    uint8_t file_format : 2;           ///< File format
    uint8_t tmp_write_protect : 1;     ///< temporary write protection
    uint8_t perm_write_protect : 1;    ///< permanent write protection
    uint8_t copy : 1;                  ///< copy flag
    uint8_t file_format_grp : 1;       ///< File format
    // Byte 15
    uint8_t crc7_end_bit;              ///< CRC
} px_sd_csd_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise driver.
 *  
 *  @param handle    SPI handle to use for SPI slave device
 */
void px_sd_init(px_spi_handle_t * handle);

/**
 *  Detect and reset SD card.
 *  
 *  @retval true    SD card detected and reset to known state
 *  @retval false   SD card not detected or not supported
 */
bool px_sd_reset(void);

/**
 *  Read Card ID Register.
 *   
 *  @param cid       Structure to store data
 *   
 *  @retval true     Card ID register succesfully read 
 *  @retval false    Error while trying to read Card ID register 
 */
bool px_sd_rd_cid(px_sd_cid_t * cid);

/**
 *  Read Card Specific Data.
 *   
 *  @param csd       Structure to store data
 *   
 *  @retval true     Card Specific Data succesfully read 
 *  @retval false    Error while trying to read Card Specific Data 
 */
bool px_sd_rd_csd(px_sd_csd_t * csd);

/**
 *  Read Card Status.
 *   
 *  @param status    Variable to store card status in
 *   
 *  @retval true     Card Status succesfully read 
 *  @retval false    Error while trying to read Card Status 
 */
bool px_sd_get_status(uint16_t * status);

/**
 *  Extract and calculate card capacity from Card Specific Data (CSD).
 *   
 *  @param csd       Card Specific Data
 *   
 *  @return uint32_t Card capacity in number of blocks (see #PX_SD_BLOCK_SIZE)
 */
uint32_t px_sd_get_capacity_in_blocks(const px_sd_csd_t * csd);

/**
 *  Read a data block from the SD card. 
 *   
 *  The data block size is #PX_SD_BLOCK_SIZE and the specified array must be big 
 *  enough to accept the whole data block. 
 *   
 *  @param data      Pointer to array where data block must be stored
 *  @param block_adr Address of block to read.
 *   
 *  @retval true     Data block succcesfully read 
 *  @retval false    Error while trying to read data block 
 */
bool px_sd_rd_block(uint8_t * data, uint32_t block_adr);

/**
 *  Read a number of data blocks from the SD card. 
 *   
 *  The data block size is #PX_SD_BLOCK_SIZE and the specified array must be big 
 *  enough to accept (nr_of_blocks x PX_SD_BLOCK_SIZE). 
 *   
 *  @param data          Pointer to array where data blocks must be stored
 *  @param block_adr     Address of start block to read
 *  @param nr_of_blocks  Number of blocks to read   
 *   
 *  @return uint8_t      Number of blocks succesfully read
 */
uint8_t px_sd_rd_blocks(uint8_t * data, uint32_t block_adr, uint8_t nr_of_blocks);

/**
 *  Write a data block to the SD card. 
 *   
 *  The data block size is #PX_SD_BLOCK_SIZE and the specified array must contain 
 *  the whole data block. Poll px_sd_wait_write_finished() to find out when write 
 *  operation has finished, e.g. before powering down SD card.
 *   
 *  @param data      Pointer to array containing data block content to be written
 *  @param block_adr Address of block to write.
 *   
 *  @retval true     Data block succcesfully written 
 *  @retval false    Error while trying to write data block 
 */
bool px_sd_wr_block(const uint8_t * data, uint32_t block_adr);

/**
 *  Write a number of data blocks to the SD card. 
 *   
 *  The data block size is #PX_SD_BLOCK_SIZE and the specified array must contain 
 *  (nr_of_blocks x PX_SD_BLOCK_SIZE).  Poll px_sd_wait_write_finished() to find out 
 *  when write operation has finished, e.g. before powering down SD card.
 *   
 *  @param data          Pointer to array containing content to be written
 *  @param block_adr     Address of start block to write.
 *  @param nr_of_blocks  Number of blocks to write 
 *   
 *  @return uint8_t      Number of blocks succesfully written
 */
uint8_t px_sd_wr_blocks(const uint8_t * data, uint32_t block_adr, uint8_t nr_of_blocks);

/**
 *  Wait up to 500 ms for write block transaction to finish.
 *  
 *  @retval true        Write operation finished
 *  @retval false       Timed-out waiting for SD card to be ready
 */
bool px_sd_wait_wr_is_finished(void);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
