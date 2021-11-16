#ifndef __PX_UART_H__
#define __PX_UART_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2012 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
   
    Title:          px_uart.h : UART peripheral driver
    Author(s):      Pieter Conradie
    Creation Date:  2012-06-10

============================================================================= */

/** 
 *  @ingroup AVR
 *  @defgroup AVR_UART px_uart.h : UART peripheral driver
 *  
 *  Driver for the UART peripheral for serial communication.
 *  
 *  File(s):
 *  - arch/avr/inc/px_uart.h
 *  - arch/avr/inc/px_uart_cfg_template.h
 *  - arch/avr/src/px_uart.c
 *  
 *  The driver must be configured by supplying a project specific 
 *  "px_uart_cfg.h". "px_uart_cfg_template.h" can be copied, renamed and 
 *  modified to supply compile time options.
 *  
 *  @include "avr/test/px_uart_test.c"
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

// Include project specific configuration. See "px_uart_cfg_template.h"
#include "px_uart_cfg.h"

// Check that all project specific options have been specified in "px_uart_cfg.h"
#if (   !defined(PX_UART_CFG_UART0_EN     ) \
     || !defined(PX_UART_CFG_UART1_EN     ) \
     || !defined(PX_UART_CFG_DEFAULT_BAUD ) \
     || !defined(PX_UART_CFG_DEFAULT_UCSRC) \
     || !defined(PX_UART_CFG_TX_BUF_SIZE  ) \
     || !defined(PX_UART_CFG_RX_BUF_SIZE  )  )
#error "One or more options not defined in 'px_uart_cfg.h'"
#endif
#if (   (PX_UART_CFG_UART0_EN > 1) \
     || (PX_UART_CFG_UART1_EN > 1)  )
#error "PX_UART_CFG_UARTx_EN must be 0 or 1"
#endif

/// Number of enabled peripherals
#define PX_UART_CFG_PER_COUNT   (   PX_UART_CFG_UART0_EN \
                                  + PX_UART_CFG_UART1_EN )
#if (PX_UART_CFG_PER_COUNT == 0)
#error "No peripherals enabled"
#endif

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Specify UART peripheral
typedef enum
{
    PX_UART_NR_0 = 0,
    PX_UART_NR_1 = 1,
} px_uart_nr_t;

/// Specify UART parity (none, odd or even)
typedef enum
{
    PX_UART_PARITY_NONE = 0,
    PX_UART_PARITY_ODD,
    PX_UART_PARITY_EVEN,
} px_uart_parity_t;

// Specify number of data bits used (5..8)
typedef enum
{
    PX_UART_DATA_BITS_5 = 5,
    PX_UART_DATA_BITS_6 = 6,
    PX_UART_DATA_BITS_7 = 7,
    PX_UART_DATA_BITS_8 = 8,
} px_uart_data_bits_t;

/// Specify number of stop bits (1/2)
typedef enum
{
    PX_UART_STOP_BITS_1 = 1,
    PX_UART_STOP_BITS_2 = 2,
} px_uart_stop_bits_t;

/// Define UART handle
typedef struct
{
    struct px_uart_per_s * uart_per;      ///< USART peripheral data
} px_uart_handle_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/** 
 *  Initialise UART driver.
 */
void px_uart_init(void);

/** 
 *  Open UART peripheral using predefined (default) parameters
 *  
 *  @param handle        Pointer to handle data structure
 *  @param uart_nr       UART peripheral number. @see px_uart_nr_t
 *  
 *  @retval false        Error: peripheral was not opened
 *  @retval true         Success: peripheral was opened
 */
bool px_uart_open(px_uart_handle_t * handle, 
                  px_uart_nr_t       uart_nr);

/** 
 *  Open UART peripheral using specified parameters
 *  
 *  @param handle        Pointer to handle data structure
 *  @param uart_nr       UART peripheral number. @see px_uart_nr_t
 *  @param baud          Baud rate in bits/s
 *  @param data_bits     Data bits. @see px_uart_data_bits_t
 *  @param parity        Parity. @see px_uart_parity_t
 *  @param stop_bits     Stop bits. @see px_uart_stop_bits_t
 *  
 *  @retval false        Error: peripheral was not opened
 *  @retval true         Success: peripheral was opened
 */
bool px_uart_open2(px_uart_handle_t *  handle,
                   px_uart_nr_t        uart_nr,
                   uint32_t            baud, 
                   px_uart_data_bits_t data_bits, 
                   px_uart_parity_t    parity, 
                   px_uart_stop_bits_t stop_bits);

/**
 *  Close specified peripheral.
 *  
 *  @param handle    Pointer to handle data structure
 *  
 *  @retval true     Success 
 *  @retval false    Specified peripheral was already closed (or not opened)
 */
bool px_uart_close(px_uart_handle_t * handle);

/**
 *  Write one byte.
 *  
 *  This function blocks until space is available in the write buffer.
 *  
 *  @param handle     Pointer to handle data structure
 *  @param[in] data   Byte to be written
 */
void px_uart_putchar(px_uart_handle_t * handle, char data);

/**
 *  Buffer one byte for transmission.
 *  
 *  @param handle     Pointer to handle data structure
 *  @param[in] data   Byte to be written
 *  
 *  @retval true    Byte has been buffered
 *  @retval false   Byte has not been buffered, because write buffer is full
 */
bool px_uart_wr_u8(px_uart_handle_t * handle, uint8_t data);

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
size_t px_uart_wr(px_uart_handle_t * handle, 
                  const void *       data, 
                  size_t             nr_of_bytes);

/**
 *  Read one byte.
 *  
 *  This function blocks until a byte is received.
 *  
 *  @param handle        Pointer to handle data structure
 *  
 *  @return     Received byte
 */
char px_uart_getchar(px_uart_handle_t * handle);

/**
 *  See if a received byte is available and store it in the specified location.
 *  
 *  @param handle        Pointer to handle data structure
 *  @param[out] data    Pointer to location where data byte must be stored
 *  
 *  @retval true        Received byte is stored in specified location
 *  @retval false       No received data available (receive buffer empty)
 */
bool px_uart_rd_u8(px_uart_handle_t * handle, uint8_t * data);

/**
 *  Copy received data from receive buffer into specified buffer.
 *  
 *  @param handle           Pointer to handle data structure
 *  @param[out] buffer      Buffer to copy received data into
 *  @param[in] nr_of_bytes  Maximum number of received bytes to copy into buffer
 *  
 *  @return size_t          Number of received bytes copied into buffer
 */
size_t px_uart_rd(px_uart_handle_t * handle, 
                  void *             buffer,
                  size_t             nr_of_bytes);

/** 
 *  See if transmit buffer can accept more data.
 *  
 *  @param handle        Pointer to handle data structure
 *  
 *  @retval true    Transmit buffer is full
 *  @retval false   Transmit buffer has space for at least one byte
 */
bool px_uart_wr_buf_is_full(px_uart_handle_t * handle);

/** 
 *  See if transmit buffer is empty.
 *  
 *  @note Buffer may be empty, but UART peripheral may still be busy 
 *        with the transmission of the last byte in the buffer. 
 *        @see px_uart_tx_finished.
 *  
 *  @param handle        Pointer to handle data structure
 *  
 *  @retval true    Transmit buffer is empty
 *  @retval false   Transmit buffer has space for at least one byte
 */
bool px_uart_wr_buf_is_empty(px_uart_handle_t * handle);

/** 
 *  See if all transmission has finished, including last byte.
 *  
 *  This functions is usefull for communication standards like RS-485 
 *  where the mode must be changed manually from TX to RX after transmission.
 *  
 *  @param handle        Pointer to handle data structure
 *  
 *  @retval true    Transmision completely finished
 *  @retval false   Busy with transmission
 */
bool px_uart_wr_is_done(px_uart_handle_t * handle);

/**
 *  See if there is received data in the receive buffer.
 *  
 *  @param handle        Pointer to handle data structure
 *  
 *  @retval true    There is received data in the receive buffer
 *  @retval false   The receive buffer is empty
 */
bool px_uart_rd_buf_is_empty(px_uart_handle_t * handle);

/**
 *  Change UART peripheral baud rate.
 *
 *  @param handle        Pointer to handle data structure
 *  @param baud          Baud rate in bits/s
 *
 *  @retval true         Success. Baud was changed
 *  @retval false        Error. Requested baud invalid
 */
bool px_uart_change_baud(px_uart_handle_t * handle, uint32_t baud);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

/// @}
#endif
