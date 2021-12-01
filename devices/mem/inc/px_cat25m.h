#ifndef __PX_CAT25M_H__
#define __PX_CAT25M_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2014 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          px_cat25m.h : On Semiconductor CAT25M SPI EEPROM Driver
    Author(s):      Pieter Conradie
    Creation Date:  2014-10-16

============================================================================= */
/** 
 *  @ingroup DEVICES_MEM
 *  @defgroup PX_CAT25M px_cat25m.h : On Semiconductor CAT25M SPI EEPROM Driver
 *  
 *  Driver that communicates with an On Semiconductor CAT25M EEPROM using SPI.
 *  
 *  File(s):
 *  - devices/mem/inc/px_cat25m.h
 *  - devices/mem/inc/px_cat25m_cfg_template.h
 *  - devices/mem/src/px_cat25m.c
 *  
 *  Reference:
 *  - [On Semiconductor CAT25M01](http://www.onsemi.com/pub/Collateral/CAT25M01-D.PDF) 1 Mbit SPI Serial CMOS EEPROM datasheet
 *
 *  @{
 */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"
#include "px_spi.h"

// Include project specific configuration. See "px_cat25m_cfg_template.h"
#include "px_cat25m_cfg.h"

// Check that all project specific options have been specified in "px_cat25m_cfg.h"
#ifndef PX_CAT25M_CFG_DEVICE
#error "PX_CAT25M_CFG_DEVICE not specified"
#endif

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
/// @name List CAT25M devices
/// @{
#define PX_CAT25M01 0   /// 1 Mbit SPI Serial CMOS EEPROM
#define PX_CAT25M02 1   /// 2 Mbit SPI Serial CMOS EEPROM
/// @}

// Determine number of pages and page size according to device specified
#if   (PX_CAT25M_CFG_DEVICE == PX_CAT25M01)
#define PX_CAT25M_PAGES         512ul
#define PX_CAT25M_PAGE_SIZE     256ul
#elif (PX_CAT25M_CFG_DEVICE == PX_CAT25M02)
#define PX_CAT25M_PAGES         1024ul
#define PX_CAT25M_PAGE_SIZE     256ul
#else
#error "Invalid CAT25M device specified"
#endif

/// EEPROM size (in bytes)
#define PX_CAT25M_EEPROM_SIZE_BYTES    (PX_CAT25M_PAGES * PX_CAT25M_PAGE_SIZE)

/// Maximum adress
#define PX_CAT25M_ADR_MAX              (PX_CAT25M_EEPROM_SIZE_BYTES - 1)

/// @name Status register
/// @{
#define PX_CAT25M_STATUS_WPEN  7   ///< Write Protect Enable
#define PX_CAT25M_STATUS_IPL   6   ///< Identification Page Latch
#define PX_CAT25M_STATUS_LIP   4   ///< Lock Identification Page
#define PX_CAT25M_STATUS_BP1   3   ///< Block Protect bit 1
#define PX_CAT25M_STATUS_BP0   2   ///< Block Protect bit 0
#define PX_CAT25M_STATUS_WEL   1   ///< Write Enable Latch
#define PX_CAT25M_STATUS_RDY   0   ///< Ready (inverted)
/// @}

/// Maximum SPI Clock rate
#define PX_CAT25M_MAX_SPI_CLOCK_HZ  10000000
/// SPI Clock / Data phase
#define PX_CAT25M_SPI_MODE          PX_SPI_MODE0
/// SPI Data order
#define PX_CAT25M_SPI_DATA_ORDER    PX_SPI_DATA_ORDER_MSB

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Address size
typedef uint32_t px_cat25m_adr_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise driver.
 *  
 *  @param handle    SPI handle to use for SPI slave device
 */
void px_cat25m_init(px_spi_handle_t * handle);

/**
 *  Read data from EEPROM.
 *  
 *  This function reads data from EEPROM and stores it in the specified buffer.
 *  
 *  @param[out] buf             Buffer to store read data
 *  @param[in]  adr             0 to PX_CAT25M_ADR_MAX
 *  @param[in]  nr_of_bytes     Number of bytes to read
 */
void px_cat25m_rd(void * buf, px_cat25m_adr_t adr, size_t nr_of_bytes);

/**
 *  Read a page from EEPROM.
 *  
 *  This function reads a page of data from EEPROM and stores it in the 
 *  specified buffer. The buffer must be at least PX_CAT25M_PAGE_SIZE bytes in size
 *  to accomodate a full page.
 *  
 *  The EEPROM has PX_CAT25M_PAGES pages.
 *  
 *  @param[out] buf             Buffer to store read data
 *  @param[in]  page            0 to (PX_CAT25M_PAGES - 1)
 *  
 */
void px_cat25m_rd_page(void * buf, uint16_t page);

/**
 *  Partial read of data in a page of EEPROM.
 *  
 *  This function reads part of a page of data from EEPROM and stores it in
 *  the specified buffer. The buffer must be at least @b nr_of_bytes in size
 *  to the read data.
 *  
 *  @note
 *  
 *  Only read up to the end of the specified page. If the page boundary is
 *  exceeded, the index will wrap to the start of the page, i.e. only the
 *  content of the specified page will be read.
 *  
 *  @param[out] buf                 Buffer to store read data
 *  @param[in]  page                0 to (PX_CAT25M_PAGES - 1)
 *  @param[in]  start_byte_in_page  Index of first byte to read (0 to PX_CAT25M_PAGE_SIZE - 1)
 *  @param[in]  nr_of_bytes         Number of bytes to read
 *  
 */
void px_cat25m_rd_page_offset(void *   buf,
                              uint16_t page,
                              uint8_t  start_byte_in_page,
                              size_t   nr_of_bytes);

/**
 *  Write a page from EEPROM.
 *  
 *  This function writes a page of data to EEPROM using the specified 
 *  buffer as the source. The buffer must contain at least PX_CAT25M_PAGE_SIZE bytes
 *  of data.
 *  
 *  The EEPROM has PX_CAT25M_PAGES pages.
 *  
 *  @param[in] buf      Buffer containing data to be written
 *  @param[in]  page    0 to (PX_CAT25M_PAGES - 1)
 *  
 */
void px_cat25m_wr_page(const void * buf, uint16_t page);

/**
 *  Partial write of data in a page of EEPROM.
 *  
 *  This function writes a part of a page of data to EEPROM using the 
 *  specified buffer as source. The buffer must contain at least @b nr_of_bytes
 *  of data.
 *  
 *  @note
 *  
 *  Only write up to the end of the specified page. If the page boundary is
 *  exceeded, the index will wrap to the start of the page, i.e. only the
 *  content of the specified page will be written.
 *  
 *  @param[in]  buf                 Buffer containing data to be written
 *  @param[in]  page                0 to (PX_CAT25M_PAGES - 1)
 *  @param[in]  start_byte_in_page  Index of first byte to write (0 to PX_CAT25M_PAGE_SIZE - 1)
 *  @param[in]  nr_of_bytes         Number of bytes to write
 *  
 */
void px_cat25m_wr_page_offset(const void * buf,
                              uint16_t     page,   
                              uint8_t      start_byte_in_page,                                    
                              size_t       nr_of_bytes);

/**
 *  Check if EEPROM is ready for the next read or write access.
 *  
 *  When data is written to EEPROM, the microcontroller must wait until the
 *  operation is finished, before the next one is attempted. This function
 *  allows the microcontroller to do something else while waiting for the
 *  operation to finish.
 *  
 * @retval true     EEPROM is ready for next read or write access.
 * @retval false    EEPROM is busy writing.
 */
bool px_cat25m_ready(void);

/**
 *  Read the status register of the EEPROM.
 *  
 *  The status register contains flags (e.g. PX_CAT25M_STATUS_RDY). This function
 *  can be used to check that a valid and working EEPROM device is connected.
 *  
 *  @return uint8_t Status register value
 */
uint8_t px_cat25m_status_rd(void);

/**
 * Write to the status register of the EEPROM
 *  
 * @param status    Value to write to the status register
 */
void px_cat25m_status_wr(uint8_t status);

/** 
 *  Enable writing.
 */
void px_cat25m_wr_en(void);

/** 
 *  Disable writing.
 */
void px_cat25m_wr_dis(void);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
