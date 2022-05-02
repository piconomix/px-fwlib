#ifndef __PX_LPUART_H__
#define __PX_LPUART_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2022 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
   
    Title:          px_lpuart.h : LPUART peripheral driver
    Author(s):      Pieter Conradie
    Creation Date:  2022-04-13

============================================================================= */

/** 
 *  @ingroup STM32
 *  @defgroup STM32_LPUART px_lpuart.h : LPUART peripheral driver
 *  
 *  Driver for the LPUART peripheral for serial communication.
 *  
 *  File(s):
 *  - arch/arm/stm32/inc/px_lpuart.h
 *  - arch/arm/stm32/inc/px_lpuart_cfg_template.h
 *  - arch/arm/stm32/src/px_lpuart.c
 *  
 *  The driver must be configured by supplying a project specific 
 *  "px_lpuart_cfg.h". "px_lpuart_cfg_template.h" can be copied, renamed and
 *  modified to supply compile time options.
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"

// Include project specific configuration. See "px_lpuart_cfg_template.h"
#include "px_lpuart_cfg.h"

// Check that all project specific options have been specified in "px_lpuart_cfg.h"
#if (   !defined(PX_LPUART_CFG_DEFAULT_BAUD      ) \
     || !defined(PX_LPUART_CFG_LPUART_TX_BUF_SIZE) \
     || !defined(PX_LPUART_CFG_LPUART_RX_BUF_SIZE) \
     || !defined(PX_LPUART_CFG_CLK_SRC           ) \
     || !defined(PX_LPUART_CFG_CLK_HZ            )  )
#error "One or more options not defined in 'px_lpuart_cfg.h'"
#endif

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Specify LPUART parity (none, odd or even)
typedef enum
{
    PX_LPUART_PARITY_NONE = 0,
    PX_LPUART_PARITY_ODD,
    PX_LPUART_PARITY_EVEN,
} px_lpuart_parity_t;

/// Specify number of data bits used (7..9)
typedef enum
{
    PX_LPUART_DATA_BITS_7 = 7,
    PX_LPUART_DATA_BITS_8 = 8,
    PX_LPUART_DATA_BITS_9 = 9,
} px_lpuart_data_bits_t;

/// Specify number of stop bits (1/2)
typedef enum
{
    PX_LPUART_STOP_BITS_1 = 1,
    PX_LPUART_STOP_BITS_2 = 2,
} px_lpuart_stop_bits_t;

/// Define LPUART handle
typedef struct
{
    struct px_lpuart_per_s * lpuart_per;      ///< USART peripheral data
} px_lpuart_handle_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/** 
 *  Initialise LPUART driver.
 */
void px_lpuart_init(void);

/**
 *  Open LPUART peripheral using predefined (default) parameters
 *
 *  @param handle        Pointer to handle data structure
 *
 *  @retval false        Error: peripheral was not opened
 *  @retval true         Success: peripheral was opened
 */
bool px_lpuart_open(px_lpuart_handle_t * handle);

/**
 *  Open LPUART peripheral using specified parameters
 *
 *  @param handle        Pointer to handle data structure
 *  @param baud          Baud rate in bits/s
 *  @param data_bits     Data bits. @see px_lpuart_data_bits_t
 *  @param parity        Parity. @see px_lpuart_parity_t
 *  @param stop_bits     Stop bits. @see px_lpuart_stop_bits_t
 *
 *  @retval false        Error: peripheral was not opened
 *  @retval true         Success: peripheral was opened
 */
bool px_lpuart_open2(px_lpuart_handle_t *  handle,
                     uint32_t              baud,
                     px_lpuart_data_bits_t data_bits,
                     px_lpuart_parity_t    parity,
                     px_lpuart_stop_bits_t stop_bits);

/**
 *  Close specified peripheral.
 *  
 *  @param handle           Pointer to handle data structure
 *  
 *  @retval true            Success
 *  @retval false           Specified peripheral was already closed (or not opened)
 */
bool px_lpuart_close(px_lpuart_handle_t * handle);

/**
 *  Write one byte.
 *  
 *  This function blocks until space is available in the write buffer.
 *  
 *  @param handle           Pointer to handle data structure
 *  @param[in] data         Byte to be written
 */
void px_lpuart_putchar(px_lpuart_handle_t * handle, char data);

/**
 *  Buffer one byte for transmission.
 *  
 *  @param handle           Pointer to handle data structure
 *  @param[in] data         Byte to be written
 *  
 *  @retval true            Byte has been buffered
 *  @retval false           Byte has not been buffered, because write buffer is full
 */
bool px_lpuart_wr_u8(px_lpuart_handle_t * handle, uint8_t data);

/**
 *  Buffer byte(s) for transmission.
 *  
 *  @note The write buffer may not be able to hold all of the specified
 *        data.
 *  
 *  @param handle           Pointer to handle data structure
 *  @param[in] data         Buffer containing data for transmission
 *  @param[in] nr_of_bytes  Number of bytes in buffer to be written
 *  
 *  @return size_t          The actual number of bytes buffered for transmission.
 */
size_t px_lpuart_wr(px_lpuart_handle_t * handle,
                    const void *       data,
                    size_t             nr_of_bytes);

/**
 *  Read one byte.
 *  
 *  This function blocks until a byte is received.
 *  
 *  @param handle           Pointer to handle data structure
 *  
 *  @return     Received byte
 */
char px_lpuart_getchar(px_lpuart_handle_t * handle);

/**
 *  See if a received byte is available and store it in the specified location.
 *  
 *  @param handle           Pointer to handle data structure
 *  @param[out] data        Pointer to location where data byte must be stored
 *  
 *  @retval true            Received byte is stored in specified location
 *  @retval false           No received data available (receive buffer empty)
 */
bool px_lpuart_rd_u8(px_lpuart_handle_t * handle, uint8_t * data);

/**
 *  Copy received data from receive buffer into specified buffer.
 *  
 *  @param handle           Pointer to handle data structure
 *  @param[out] buf         Buffer to copy received data into
 *  @param[in] nr_of_bytes  Maximum number of received bytes to copy into buffer
 *  
 *  @return size_t          Number of received bytes copied into buffer
 */
size_t px_lpuart_rd(px_lpuart_handle_t * handle,
                    void *             buf,
                    size_t             nr_of_bytes);

/** 
 *  See if transmit buffer can accept more data.
 *  
 *  @param handle       Pointer to handle data structure
 *  
 *  @retval true        Transmit buffer is full
 *  @retval false       Transmit buffer has space for at least one byte
 */
bool px_lpuart_wr_buf_is_full(px_lpuart_handle_t * handle);

/** 
 *  See if transmit buffer is empty.
 *  
 *  @note Buffer may be empty, but LPUART peripheral may still be busy
 *        with the transmission of the last byte in the buffer. 
 *        @see px_lpuart_tx_finished.
 *  
 *  @param handle       Pointer to handle data structure
 *  
 *  @retval true        Transmit buffer is empty
 *  @retval false       Transmit buffer has space for at least one byte
 */
bool px_lpuart_wr_buf_is_empty(px_lpuart_handle_t * handle);

/** 
 *  See if all transmission has finished, including last byte.
 *  
 *  This functions is usefull for communication standards like RS-485 
 *  where the mode must be changed manually from TX to RX after transmission.
 *  
 *  @param handle       Pointer to handle data structure
 *  
 *  @retval true        Transmision completely finished
 *  @retval false       Busy with transmission
 */
bool px_lpuart_wr_is_done(px_lpuart_handle_t * handle);

/**
 *  See if there is received data in the receive buffer.
 *  
 *  @param handle       Pointer to handle data structure
 *  
 *  @retval true        There is received data in the receive buffer
 *  @retval false       The receive buffer is empty
 */
bool px_lpuart_rd_buf_is_empty(px_lpuart_handle_t * handle);

/**
 *  Change LPUART peripheral baud rate.
 *  
 *  @param handle        Pointer to handle data structure
 *  @param baud          Baud rate in bits/s
 *
 *  @retval true         Success. Baud was changed
 *  @retval false        Error. Requested baud invalid
 */
bool px_lpuart_change_baud(px_lpuart_handle_t * handle, uint32_t baud);

/**
 *  Change LPUART peripheral data format.
 *
 *  @param handle        Pointer to handle data structure
 *  @param data_bits     Data bits (7,8 or 9)
 *  @param parity        Parity(NONE, ODD or EVEN)
 *  @param stop_bits     Stop bits (1 or 2)
 *
 *  @retval true         Success. data format was changed
 *  @retval false        Error. Requested data format invalid
 */
bool px_lpuart_change_data_format(px_lpuart_handle_t *  handle,
                                  px_lpuart_data_bits_t data_bits,
                                  px_lpuart_parity_t    parity,
                                  px_lpuart_stop_bits_t stop_bits);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
