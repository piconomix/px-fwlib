#ifndef __PX_AT25S_H__
#define __PX_AT25S_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md

    Title:          px_at25s.h : Adesto AT25S Serial Flash Driver
    Author(s):      Pieter Conradie
    Creation Date:  2018-10-12

============================================================================= */
/** 
 *  @ingroup DEVICES_MEM
 *  @defgroup PX_AT25S px_at25s.h : Adesto AT25S Serial Flash Driver
 *  
 *  Driver that communicates with an Adesto AT25S Serial Flash using SPI.
 *  
 *  File(s):
 *  - devices/mem/inc/px_at25s.h
 *  - devices/mem/inc/px_at25s_cfg_template.h
 *  - devices/mem/src/px_at25s.c
 *  
 *  Reference:
 *  - [Adesto AT25S041](https://www.adestotech.com/wp-content/uploads/DS-AT25S041_044.pdf) 4-Mbit SPI Serial Flash Memory datasheet
 *  
 *  Example:
 *  
 *  @include devices/mem/test/px_at25s_test.c
 *  
 */
/// @{

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_spi.h"

// Include project specific configuration. See "px_at25s_cfg_template.h"
#include "px_at25s_cfg.h"

// Check that all project specific options have been specified in "px_at25s_cfg.h"
#ifndef PX_AT25S_CFG_DEVICE
#error "PX_AT25S_CFG_DEVICE not specified"
#endif

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
/// @name List of supported AT25S devices
//@{
#define PX_CFG_DEV_AT25SF041 0		///< 4 Mbit, 2.5-3.6V 104 MHz Standard Serial Flash
#define PX_CFG_DEV_AT25SF081 1      ///< 8 Mbit, 2.5-3.6V 104 MHz Standard Serial Flash
#define PX_CFG_DEV_AT25SF161 2      ///< 16 Mbit, 2.5-3.6V 104 MHz Standard Serial Flash
#define PX_CFG_DEV_AT25SF321 3      ///< 32 Mbit, 2.5-3.6V 104 MHz Standard Serial Flash
#define PX_CFG_DEV_AT25SF641 4      ///< 64 Mbit, 2.5-3.6V 104 MHz Standard Serial Flash
//@}

/// AT25S page size
#define PX_AT25S_PAGE_SIZE  256

/// AT25S block selection
typedef enum
{
    PX_AT25S_BLOCK_4KB  = 0,
    PX_AT25S_BLOCK_32KB = 1,
    PX_AT25S_BLOCK_64KB = 2,
} px_at25s_block_t;

/// @name AT25S block sizes
//@{
#define PX_AT25S_BLOCK_SIZE_4KB     (4 * 1024)
#define PX_AT25S_BLOCK_SIZE_32KB    (32 * 1024)
#define PX_AT25S_BLOCK_SIZE_64KB    (64 * 1024)
//@}
    
/// @name Pages per block
//@{
#define PX_AT25S_PAGES_PER_BLOCK_4KB    (PX_AT25S_BLOCK_SIZE_4KB / PX_AT25S_PAGE_SIZE)
#define PX_AT25S_PAGES_PER_BLOCK_32KB   (PX_AT25S_BLOCK_SIZE_32KB / PX_AT25S_PAGE_SIZE)
#define PX_AT25S_PAGES_PER_BLOCK_64KB   (PX_AT25S_BLOCK_SIZE_64KB / PX_AT25S_PAGE_SIZE)
//@}

// Determine number of pages and page size according to device specified
#if   (PX_AT25S_CFG_DEVICE == PX_CFG_DEV_AT25SF041)
#define PX_AT25S_PAGES        2048ul
#elif (PX_AT25S_CFG_DEVICE == PX_CFG_DEV_AT25SF081)
#define PX_AT25S_PAGES        4096ul
#elif (PX_AT25S_CFG_DEVICE == PX_CFG_DEV_AT25SF161)
#define PX_AT25S_PAGES        8192ul
#elif (PX_AT25S_CFG_DEVICE == PX_CFG_DEV_AT25SF321)
#define PX_AT25S_PAGES        16384ul
#elif (PX_AT25S_CFG_DEVICE == PX_CFG_DEV_AT25SF641)
#define PX_AT25S_PAGES        32768ul
#else
#error "Unsupported AT25S device specified"
#endif

/// Flash size (in bytes)
#define PX_AT25S_FLASH_SIZE_BYTES   (PX_AT25S_PAGES * PX_AT25S_PAGE_SIZE)

/// Maximum adress
#define PX_AT25S_ADR_MAX            (PX_AT25S_FLASH_SIZE_BYTES - 1)

/// @name Status register 1
//@{
#define PX_AT25S_STATUS_REG1_SRP0       7   ///< Status Register Protection bit 0
#define PX_AT25S_STATUS_REG1_SEC        6   ///< Block Protection
#define PX_AT25S_STATUS_REG1_TB         5   ///< Top or Bottom Protection
#define PX_AT25S_STATUS_REG1_BP2        4   ///< Block Protection bit 2
#define PX_AT25S_STATUS_REG1_BP1        3   ///< Block Protection bit 1
#define PX_AT25S_STATUS_REG1_BP0        2   ///< Block Protection bit 0
#define PX_AT25S_STATUS_REG1_WEL        1   ///< Write Enable Latch Status
#define PX_AT25S_STATUS_REG1_BSY        0   ///< Ready/Busy Status
//@}

/// Maximum SPI Clock rate
#define PX_AT25S_MAX_SPI_CLOCK_HZ  33000000
/// SPI Clock / Data phase
#define PX_AT25S_SPI_MODE          PX_SPI_MODE0
/// SPI Data order
#define PX_AT25S_SPI_DATA_ORDER    PX_SPI_DATA_ORDER_MSB

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise driver
 *  
 *  @param handle    SPI handle to use for SPI slave device
 */
void px_at25s_init(px_spi_handle_t * handle);

/** 
 *  Power down device to minimise power consumption.
 */
void px_at25s_deep_power_down(void);

/** 
 *  Power up device to resume communication.
 */
void px_at25s_resume_from_deep_power_down(void);

/**
 *  Read data from Serial Flash.
 *  
 *  This function reads data from Serial Flash and stores it in the specified
 *  buffer.
 *  
 *  @param buffer           Buffer to store read data
 *  @param address          0 to PX_AT25S_ADR_MAX
 *  @param nr_of_bytes      Number of bytes to read
 *  
 */
void px_at25s_rd(void *   buffer,
                 uint32_t address,
                 uint16_t nr_of_bytes);

/**
 *  Read a page from Serial Flash.
 *  
 *  This function reads a page of data from Serial Flash and stores it in the 
 *  specified buffer. The buffer must be at least PX_AT25S_PAGE_SIZE bytes in size
 *  to accomodate a full page.
 *  
 *  The Serial Flash has PX_AT25S_PAGES pages.
 *  
 *  @param[out] buffer          Buffer to store read data
 *  @param[in]  page            0 to (PX_AT25S_PAGES-1)
 *  
 */
void px_at25s_rd_page(void *   buffer,
                      uint16_t page);

/**
 *  Partial read of data in a page of Serial Flash.
 *  
 *  This function reads part of a page of data from Serial Flash and stores it in
 *  the specified buffer. The buffer must be at least @b nr_of_bytes in size
 *  to the read data.
 *  
 *  Only read up to the end of the specified page. If the page boundary is
 *  exceeded, the index will wrap to the start of the page, i.e. only the
 *  content of the specified page will be read.
 *  
 *  @param[out] buffer              Buffer to store read data
 *  @param[in]  page                0 to (PX_AT25S_PAGES-1)
 *  @param[in]  start_byte_in_page  Index of first byte to read (0 to
 *                                  PX_AT25S_PAGE_SIZE - 1)
 *  @param[in]  nr_of_bytes         Number of bytes to read
 *  
 */
void px_at25s_rd_page_offset(void *   buffer,
                             uint16_t page,
                             uint8_t  start_byte_in_page,
                             uint8_t  nr_of_bytes);

/**
 *  Write a page from Serial Flash.
 *  
 *  This function writes a page of data to Serial Flash using the specified 
 *  buffer as the source. The buffer must contain at least PX_AT25S_PAGE_SIZE bytes
 *  of data.
 *  
 *  The Serial Flash has PX_AT25S_PAGES pages.
 *  
 *  @param[in] buffer  Buffer containing data to be written
 *  @param[in]  page   0 to (PX_AT25S_PAGES-1)
 *  
 */
void px_at25s_wr_page(const void * buffer,
                      uint16_t     page);

/**
 *  Partial write of data in a page of Serial Flash.
 *  
 *  This function writes a part of a page of data to Serial Flash using the 
 *  specified buffer as source. The buffer must contain at least @b nr_of_bytes
 *  of data.
 *  
 *  @note
 *  
 *  Only write up to the end of the specified page. If the page boundary is
 *  exceeded, the index will wrap to the start of the page, i.e. only the
 *  content of the specified page will be written.
 *  
 *  @param[in]  buffer              Buffer containing data to be written
 *  @param[in]  page                0 to (PX_AT25S_PAGES-1)
 *  @param[in]  start_byte_in_page  Index of first byte to write (0 to
 *                                  PX_AT25S_PAGE_SIZE - 1)
 *  @param[in]  nr_of_bytes         Number of bytes to write
 *  
 */
void px_at25s_wr_page_offset(const void * buffer,
                             uint16_t     page,   
                             uint8_t      start_byte_in_page,                                    
                             uint8_t      nr_of_bytes);

/**
 *  Erase a block of Serial Flash.
 *  
 *  This function erases a block of Serial Flash. The Serial Flash has
 *  PX_AT25S_PAGES pages.
 *  
 *  @param  block       4KB, 32KB or 64KB
 *  @param  page        0 to (PX_AT25S_PAGES-1); must be an integer multiple of
 *                      block size
 */
void px_at25s_erase(px_at25s_block_t block,
                    uint16_t         page);

/**
 *  Check if Serial Flash is ready for the next read or write access.
 *  
 *  When data is written to Serial Flash, the microcontroller must wait until the
 *  operation is finished, before the next one is attempted. This function
 *  allows the microcontroller to do something else while waiting for the
 *  operation to finish.
 *  
 * @retval true     Serial Flash is ready for next read or write access.
 * @retval false    Serial Flash is busy writing.
 */
bool px_at25s_ready(void);

/**
 *  Read status register 1 of the Serial Flash.
 *  
 *  The status register contains flags, e.g. PX_AT25S_STATUS_REG1_BSY to see
 *  if the device is ready or busy with an internal operation.
 *  
 *  @return uint8_t Status register 1 value
 */
uint8_t px_at25s_rd_status_reg1(void);

/**
 *  Read Manufacturer and Device ID.
 *  
 *  @param buffer   Buffer with a size of 3 bytes to hold manufacturer and
 *                  device ID
 */
void px_at25s_rd_man_and_dev_id(uint8_t * buffer);

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif // #ifndef __PX_AT25S_H__
