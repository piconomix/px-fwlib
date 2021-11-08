#ifndef __PX_SPI_H__
#define __PX_SPI_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_spi.h : SPI Peripheral Driver
    Author(s):      Pieter Conradie
    Creation Date:  2018-03-01

============================================================================= */

/** 
 *  @ingroup STM32
 *  @defgroup STM32_SPI px_spi.h : SPI Peripheral Driver
 *  
 *  Driver for the SPI peripheral to communicate with SPI slaves.
 *  
 *  File(s):
 *  - arch/arm/stm32/inc/px_spi.h
 *  - arch/arm/stm32/inc/px_spi_cfg_template.h
 *  - arch/arm/stm32/src/px_spi.c 
 *   
 *  The driver must be configured by supplying a project specific "px_spi_cfg.h".
 *  "px_spi_cfg_template.h" can be copied, renamed and modified to supply 
 *  compile time options.
 *  
 *  @par Example:
 *  @include arch/arm/stm32/test/px_spi_test.c
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

// Include project specific configuration. See "px_spi_cfg_template.h"
#include "px_spi_cfg.h"

// Check that all project specific options have been specified in "px_spi_cfg.h"
#if (   !defined(PX_SPI_CFG_SPI1_EN           ) \
     || !defined(PX_SPI_CFG_SPI2_EN           ) \
     || !defined(PX_SPI_CFG_DEFAULT_BAUD      ) \
     || !defined(PX_SPI_CFG_DEFAULT_MODE      ) \
     || !defined(PX_SPI_CFG_DEFAULT_DATA_ORDER) \
     || !defined(PX_SPI_CFG_CS_LO             ) \
     || !defined(PX_SPI_CFG_CS_HI             )  )
#error "One or more options not defined in 'px_spi_cfg.h'"
#endif
#if (  (PX_SPI_CFG_SPI1_EN > 1) \
     ||(PX_SPI_CFG_SPI2_EN > 1)  )
#error "PX_SPI_CFG_SPIx_EN must be 0 or 1"
#endif

/// Number of enabled peripherals
#define PX_SPI_CFG_PER_COUNT    (PX_SPI_CFG_SPI1_EN + PX_SPI_CFG_SPI2_EN )
#if (PX_SPI_CFG_PER_COUNT == 0)
#error "No peripherals enabled"
#endif

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Specify SPI peripheral number
typedef enum
{
    PX_SPI_NR_1 = 1,
    PX_SPI_NR_2 = 2,
} px_spi_nr_t;

/// Specify SPI Clock polarity / Clock phase
typedef enum
{
    PX_SPI_MODE0 = 0, ///< CPOL=0, CPHA=0; SCLK idle polarity low; data sampled on first (rising) edge
    PX_SPI_MODE1 = 1, ///< CPOL=0, CPHA=1; SCLK idle polarity low; data sampled on second (falling) edge
    PX_SPI_MODE2 = 2, ///< CPOL=1, CPHA=0; SCLK idle polarity high; data sampled on first (falling) edge
    PX_SPI_MODE3 = 3, ///< CPOL=1, CPHA=1; SCLK idle polarity high; data sampled on second (rising) edge
} px_spi_mode_t;

/// Specify SPI Data order
typedef enum
{
    PX_SPI_DATA_ORDER_MSB = 0, ///< Data order is Most Significant Bit first  (D7, D6, ..., D0) 
    PX_SPI_DATA_ORDER_LSB = 1, ///< Data order is Least Significant Bit first (D0, D1, ..., D7)     
} px_spi_dord_t;

/// Specify SPI baud rate as a ratio of the peripheral clock
typedef enum
{
    PX_SPI_BAUD_CLK_DIV_2 = 0,       ///< F_PCLK / 2
    PX_SPI_BAUD_CLK_DIV_4,           ///< F_PCLK / 4
    PX_SPI_BAUD_CLK_DIV_8,           ///< F_PCLK / 8
    PX_SPI_BAUD_CLK_DIV_16,          ///< F_PCLK / 16
    PX_SPI_BAUD_CLK_DIV_32,          ///< F_PCLK / 32
    PX_SPI_BAUD_CLK_DIV_64,          ///< F_PCLK / 64
    PX_SPI_BAUD_CLK_DIV_128,         ///< F_PCLK / 128
    PX_SPI_BAUD_CLK_DIV_256,         ///< F_PCLK / 256
} px_spi_baud_t;

/// @name SPI bit flags to demarcate the start and end of a transaction
/// @{

/// Begin SPI transaction (take SPI slave's Chip Select line low)
#define PX_SPI_FLAG_START           (1 << 0)
/// Finish SPI transaction (take SPI slave's Chip Select line high)
#define PX_SPI_FLAG_STOP            (1 << 1)
/// Begin and finish SPI transaction
#define PX_SPI_FLAG_START_AND_STOP  (PX_SPI_FLAG_START + PX_SPI_FLAG_STOP)
/// @}

/// Define SPI handle
typedef struct
{
    struct px_spi_per_s * spi_per;          ///< SPI peripheral
    uint8_t               cs_id;            ///< Chip Select GPIO ID
    uint32_t              spi_cr1_val;      ///< SPI Control Register 1 value (SPI_CR1)
    uint8_t               mo_dummy_byte;    ///< Master Out dummy byte when data is read from Master In
} px_spi_handle_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise SPI driver.
 */
void px_spi_init(void);

/** 
 *  Open SPI peripheral using predefined (default) parameters.
 *  
 *  @param handle       Pointer to handle data structure
 *  @param spi_nr       SPI peripheral number
 *  @param cs_id        ID to use when calling PX_SPI_CS_xx() macro
 *  
 *  @retval false       Error: peripheral was not opened
 *  @retval true        Success: peripheral was opened
 */
bool px_spi_open(px_spi_handle_t * handle,
                 px_spi_nr_t       spi_nr,
                 uint8_t           cs_id);

/**
 *  Open SPI peripheral using specified parameters. 
 *  
 *  @param handle           Pointer to handle data structure
 *  @param spi_nr           SPI peripheral number
 *  @param cs_id            ID to use when calling PX_SPI_CS() macro
 *  @param baud             Desired BAUD rate as a division of peripheral clock
 *  @param mode             Clock mode 0,1,2 or 3 @see px_spi_mode_t
 *  @param data_order       Data order (MSB first of LSB first)
 *  @param mo_dummy_byte    Master Out dummy byte used when reading data from
 *                          SPI slave
 *  
 *  @retval false           Error: peripheral was not opened
 *  @retval true            Success: peripheral was opened
 */
bool px_spi_open2(px_spi_handle_t * handle,
                  px_spi_nr_t       spi_nr,
                  uint8_t           cs_id,
                  px_spi_baud_t     baud,
                  px_spi_mode_t     mode, 
                  px_spi_dord_t     data_order,
                  uint8_t           mo_dummy_byte);

/**
 *  Close specified device.
 *  
 *  @param handle       Pointer to handle data structure
 *  
 *  @retval true        Success 
 *  @retval false       Specified device was already closed (or not opened)
 */
bool px_spi_close(px_spi_handle_t * handle);

/**
 *  Perform an SPI write transaction with an SPI slave.
 *  
 *  With the use of flags this function allows concatenation of one or more
 *  writes / reads / exchanges with an SPI slave to form one big transaction.
 *  
 *  If the PX_SPI_FLAG_START flag is specified, the SPI slave is first selected
 *  by taking it's Chip Select line low.
 *  
 *  The specified number of bytes is then written to the SPI slave.
 *  
 *  Finally, if the PX_SPI_FLAG_STOP flag is specified, the SPI slave's Chip
 *  Select line is taken high to complete the transaction.
 *  
 *  @param handle       Pointer to handle data structure
 *  @param data         Buffer containing data to write
 *  @param nr_of_bytes  Number of bytes to write to the slave
 *  @param flags        Bit combination of PX_SPI_FLAG_START and
 *                      PX_SPI_FLAG_STOP or nothing (0)
 */
void px_spi_wr(px_spi_handle_t * handle,
               const void *      data,
               size_t            nr_of_bytes,
               uint8_t           flags);

/**
 *  Perform an SPI read transaction with an SPI slave.
 *  
 *  With the use of flags this function allows concatenation of one or more
 *  writes / reads / exchanges with an SPI slave to form one big transaction.
 *  
 *  If the PX_SPI_FLAG_START flag is specified, the SPI slave is first selected
 *  by taking it's Chip Select line low.
 *  
 *  The specified number of bytes is then read from the SPI slave.
 *  
 *  Finally, if the PX_SPI_FLAG_STOP flag is specified, the SPI slave's Chip
 *  Select line is taken high to complete the transaction.
 *  
 *  @param handle           Pointer to handle data structure
 *  @param data             Pointer to a buffer where the received data must be
 *                          stored
 *  @param nr_of_bytes      Number of bytes to read from slave
 *  @param flags            Bit combination of PX_SPI_FLAG_START and
 *                          PX_SPI_FLAG_STOP or nothing (0)
 */
void px_spi_rd(px_spi_handle_t * handle,
               void *            data,
               size_t            nr_of_bytes,
               uint8_t           flags);

/**
 *  Perform an SPI exchange (write and read) transaction with an SPI
 *  slave.
 *  
 *  With the use of flags this function allows concatenation of one or more
 *  writes / reads / exchanges with an SPI slave to form one big transaction.
 *  
 *  If the PX_SPI_FLAG_START flag is specified, the SPI slave is first selected
 *  by taking it's Chip Select line low.
 *  
 *  The specified number of bytes is then written to the slave, while
 *  simultaneously reading the same number of bytes from the slave.
 *  
 *  The write buffer and read buffer must both be at least "nr_of_bytes" bytes
 *  in size. It is acceptable to have the write buffer and read buffer point to
 *  the same location in which case the data to write will be overwritten by the
 *  received data.
 *  
 *  Finally, if the PX_SPI_FLAG_STOP flag is specified, the SPI slave's Chip
 *  Select line is taken high to complete the transaction.
 *  
 *  @param handle       Pointer to handle data structure
 *  @param data_wr      Pointer to a buffer containing data to be written
 *  @param data_rd      Pointer to a buffer where the read data must be stored
 *  @param nr_of_bytes  Number of bytes to exchange
 *  @param flags        Bit combination of PX_SPI_FLAG_START and
 *                      PX_SPI_FLAG_STOP or nothing (0)
 */
void px_spi_xc(px_spi_handle_t * handle,
               const void *      data_wr,
               void *            data_rd,
               size_t            nr_of_bytes,
               uint8_t           flags);

/**
 *  Change SPI peripheral baud. 
 *  
 *  @param handle       Pointer to handle data structure
 *  @param baud         Desired BAUD rate as a division of peripheral clock
 */
void px_spi_ioctl_change_baud(px_spi_handle_t * handle,
                              px_spi_baud_t     baud);

/**
 *  Calculate clock divisor that will yield closest frequency equal to or
 *  less than specified baud rate (in Hz).
 *  
 *  @param baud_hz          Desired BAUD rate in Hz that SCK must be clocked
 *  
 *  @return px_spi_baud_t   Calculated divisor
 */
px_spi_baud_t px_spi_util_baud_hz_to_clk_div(uint32_t baud_hz);

/**
 *  Calculate the actual BAUD (in Hz) from the specified division.
 *  
 *  @param baud         Desired BAUD rate as a division of peripheral clock
 *  
 *  @return uint32_t    Actual BAUD (in Hz)
 */
uint32_t px_spi_util_clk_div_to_baud_hz(px_spi_baud_t baud);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
