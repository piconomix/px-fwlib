/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.
   
    Title:          px_uart.h : UART peripheral driver
    Author(s):      Pieter Conradie
    Creation Date:  2018-02-01

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdlib.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_uart.h"
#include "px_board.h"
#include "px_circ_buffer.h"
#include "px_lib_stm32cube.h"
#include "px_dbg.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_DBG_DECL_NAME("uart");

/// Definition of data for each UART peripheral
typedef struct px_uart_data_s
{
    /// USART peripheral base register address
    USART_TypeDef * usart_base_adr; 
    /// Peripheral
    px_uart_per_t peripheral;
    /// Number of open handles referencing peripheral
    uint8_t open_counter;
    /// Transmit finished flag
    volatile bool tx_finished;
    /// Transmit circular buffer
    px_circ_buf_t tx_circ_buf;
    /// Transmit circular buffer data
    uint8_t tx_circ_buf_data[PX_UART_CFG_TX_BUF_SIZE];
    /// Receive circular buffer
    px_circ_buf_t rx_circ_buf;
    /// Receive circular buffer data
    uint8_t rx_circ_buf_data[PX_UART_CFG_RX_BUF_SIZE];
} px_uart_data_t;

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
/// Allocate data for each enabled UART peripheral
#if PX_UART_CFG_UART1_EN
static px_uart_data_t px_uart1_data;
#endif

#if PX_UART_CFG_UART2_EN
static px_uart_data_t px_uart2_data;
#endif

#if PX_UART_CFG_UART4_EN
static px_uart_data_t px_uart4_data;
#endif

#if PX_UART_CFG_UART5_EN
static px_uart_data_t px_uart5_data;
#endif

/* _____LOCAL FUNCTIONS______________________________________________________ */
/// Generic interrupt handler
static void uart_irq_handler(px_uart_data_t * uart_data)
{
    USART_TypeDef * usart_base_adr = uart_data->usart_base_adr;
    uint8_t         data;

    // Received a byte?
    if(LL_USART_IsActiveFlag_RXNE(usart_base_adr))
    {
        // Read receive data register
        data = LL_USART_ReceiveData8(usart_base_adr);
        // Parity Error?
        if(LL_USART_IsActiveFlag_PE(usart_base_adr))
        {
            // Clear error flag
            LL_USART_ClearFlag_PE(usart_base_adr);
        }
        // Framing Error?
        else if(LL_USART_IsActiveFlag_FE(usart_base_adr))
        {
            // Clear error flag
            LL_USART_ClearFlag_FE(usart_base_adr);
        }
        else
        {
            // Add received byte to circular buffer
            // (byte is discarded if buffer is full)
            px_circ_buf_wr_u8(&uart_data->rx_circ_buf, data);
        }
    }

    // Transmit data register interrupt enabled?
    if(LL_USART_IsEnabledIT_TXE(usart_base_adr))
    {
        // Transmit data register empty?
        if(LL_USART_IsActiveFlag_TXE(usart_base_adr))
        {
            // Data to transmit?
            if(px_circ_buf_rd_u8(&uart_data->tx_circ_buf, &data))
            {
                // Load transmit register with data
                LL_USART_TransmitData8(usart_base_adr, data);
                // Clear flag to indicate that transmission is busy
                uart_data->tx_finished = false;
            }
            else
            {
                // Disable Transmit data register empty interrupt
                LL_USART_DisableIT_TXE(usart_base_adr);
                // Enable Transmit complete interrupt
                LL_USART_EnableIT_TC(usart_base_adr);
            }
        }
    }

    // Transmit complete interrupt enabled?
    if(LL_USART_IsEnabledIT_TC(usart_base_adr))
    {
        // Transmit complete?
        if(LL_USART_IsActiveFlag_TC(usart_base_adr))
        {
            // Set flag to indicate that transmission has finished
            uart_data->tx_finished = true;
            // Disable Transmit complete interrupt
            LL_USART_DisableIT_TC(usart_base_adr);
        }
    }
}

#if PX_UART_CFG_UART1_EN
/// USART1 interrupt handler
void USART1_IRQHandler(void)
{
    uart_irq_handler(&px_uart1_data);
}
#endif

#if PX_UART_CFG_UART2_EN
/// USART2 interrupt handler
void USART2_IRQHandler(void)
{
    uart_irq_handler(&px_uart2_data);
}
#endif

#if PX_UART_CFG_UART4_EN || PX_UART_CFG_UART5_EN
/// USART4 & USART5 interrupt handler
void USART4_5_IRQHandler(void)
{
#if PX_UART_CFG_UART4_EN
    uart_irq_handler(&px_uart4_data);
#endif

#if PX_UART_CFG_UART5_EN
    uart_irq_handler(&px_uart5_data);
#endif
}
#endif

static inline void px_uart_start_tx(USART_TypeDef * usart_base_adr)
{
    // Sanity check
    PX_DBG_ASSERT(usart_base_adr != NULL);

    // Enable Transmit data register empty interrupt
    LL_USART_EnableIT_TXE(usart_base_adr);
}

static bool px_uart_init_peripheral(USART_TypeDef *     usart_base_adr,
                                    px_uart_per_t       peripheral,
                                    uint32_t            baud, 
                                    px_uart_data_bits_t data_bits, 
                                    px_uart_parity_t    parity, 
                                    px_uart_stop_bits_t stop_bits)
{
    // Sanity check
    PX_DBG_ASSERT(usart_base_adr != NULL);

    // USART_CR1 register calculated value
    uint32_t usart_cr1_val;

    // Enable peripheral clock
    switch(peripheral)
    {
#if PX_UART_CFG_UART1_EN
    case PX_UART_PER_1:
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
        break;
#endif
#if PX_UART_CFG_UART2_EN
    case PX_UART_PER_2:
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);
        break;
#endif
#if PX_UART_CFG_UART4_EN
    case PX_UART_PER_4:
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART4);
        break;
#endif
#if PX_UART_CFG_UART5_EN
    case PX_UART_PER_5:
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART5);
        break;
#endif
    default:
        PX_DBG_ERR("Invalid peripheral");
        return false;
    }

    // Set baud rate
    LL_USART_SetBaudRate(usart_base_adr, 
                         PX_BOARD_PER_CLK_HZ, 
                         LL_USART_OVERSAMPLING_16,
                         baud);

    // Set transmitter and receiver and receive interrupt
    usart_cr1_val = USART_CR1_RXNEIE | USART_CR1_TE | USART_CR1_RE;

    // Set number of data bits
    switch(data_bits)
    {
    case PX_UART_DATA_BITS_7:
        usart_cr1_val |= USART_CR1_M1;
        break;
    case PX_UART_DATA_BITS_8:
        break;
    case PX_UART_DATA_BITS_9:
        usart_cr1_val |= USART_CR1_M0;
        break;
    default:
        PX_DBG_ERR("Invalid number of data bits");
        return false;
    }

    // Set parity
    switch(parity)
    {
    case PX_UART_PARITY_NONE:
        break;
    case PX_UART_PARITY_ODD:
        usart_cr1_val |= USART_CR1_PCE | USART_CR1_PS;
        break;
    case PX_UART_PARITY_EVEN:
        usart_cr1_val |= USART_CR1_PCE;
        break;
    default:
        PX_DBG_ERR("Invalid parity specified");
        return false;
    }

    // Set stop bits
    switch(stop_bits)
    {
    case PX_UART_STOP_BITS_1:
        LL_USART_SetStopBitsLength(usart_base_adr, LL_USART_STOPBITS_1);
        break;
    case PX_UART_STOP_BITS_2:
        LL_USART_SetStopBitsLength(usart_base_adr, LL_USART_STOPBITS_2);
        break;
    default:
        PX_DBG_ERR("Invalid number of stop bits specified");
        return false;
    }

    // Set Control register 1
    usart_base_adr->CR1 = usart_cr1_val;

    // Enable UART
    LL_USART_Enable(usart_base_adr);

    // Enable interrupt handler
    switch(peripheral)
    {
#if PX_UART_CFG_UART1_EN
    case PX_UART_PER_1:
        NVIC_EnableIRQ(USART1_IRQn);
        break;
#endif
#if PX_UART_CFG_UART2_EN
    case PX_UART_PER_2:
        NVIC_EnableIRQ(USART2_IRQn);
        break;
#endif
#if PX_UART_CFG_UART4_EN
    case PX_UART_PER_4:
        NVIC_EnableIRQ(USART4_5_IRQn);
        break;
#endif
#if PX_UART_CFG_UART5_EN
    case PX_UART_PER_5:
        NVIC_EnableIRQ(USART4_5_IRQn);
        break;
#endif
    default:
        break;
    }

    // Success
    return true;
}

static void px_uart_init_peripheral_data(px_uart_per_t    peripheral, 
                                         px_uart_data_t * uart_data)
{
    // Set peripheral
    uart_data->peripheral = peripheral;

    // Set peripheral base address
    switch(peripheral)
    {
#if PX_UART_CFG_UART1_EN
    case PX_UART_PER_1:
        uart_data->usart_base_adr = USART1;
        break;
#endif
#if PX_UART_CFG_UART2_EN
    case PX_UART_PER_2:
        uart_data->usart_base_adr = USART2;
        break;
#endif
#if PX_UART_CFG_UART4_EN
    case PX_UART_PER_4:
        uart_data->usart_base_adr = USART4;
        break;
#endif
#if PX_UART_CFG_UART5_EN
    case PX_UART_PER_5:
        uart_data->usart_base_adr = USART5;
        break;
#endif
    default:
        break;
    }

    // Clear reference counter
    uart_data->open_counter = 0;

    // Set transmit finished flag
    uart_data->tx_finished = true;

    // Initialise transmit circular buffer
    px_circ_buf_init(&uart_data->tx_circ_buf,
                     uart_data->tx_circ_buf_data,
                     PX_UART_CFG_TX_BUF_SIZE);

    // Initialise receive circular buffer
    px_circ_buf_init(&uart_data->rx_circ_buf,
                     uart_data->rx_circ_buf_data,
                     PX_UART_CFG_RX_BUF_SIZE);
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_uart_init(void)
{   
    // Initialize peripheral data for each enabled peripheral
#if PX_UART_CFG_UART1_EN
    px_uart_init_peripheral_data(PX_UART_PER_1, &px_uart1_data);
#endif
#if PX_UART_CFG_UART2_EN
    px_uart_init_peripheral_data(PX_UART_PER_2, &px_uart2_data);
#endif
#if PX_UART_CFG_UART4_EN
    px_uart_init_peripheral_data(PX_UART_PER_4, &px_uart4_data);
#endif
#if PX_UART_CFG_UART5_EN
    px_uart_init_peripheral_data(PX_UART_PER_5, &px_uart5_data);
#endif
}

bool px_uart_open(px_uart_handle_t * handle, 
                  px_uart_per_t      peripheral)
{
    return px_uart_open2(handle,
                         peripheral,
                         PX_UART_CFG_DEFAULT_BAUD,
                         PX_UART_DATA_BITS_8,
                         PX_UART_PARITY_NONE,
                         PX_UART_STOP_BITS_1);
}

bool px_uart_open2(px_uart_handle_t *  handle, 
                   px_uart_per_t       peripheral,
                   uint32_t            baud, 
                   px_uart_data_bits_t data_bits, 
                   px_uart_parity_t    parity, 
                   px_uart_stop_bits_t stop_bits)
{
    px_uart_data_t * uart_data;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);

    // Handle not initialised
    handle->uart_data = NULL;

    // Set pointer to peripheral data
    switch(peripheral)
    {
#if PX_UART_CFG_UART1_EN
    case PX_UART_PER_1:
        uart_data = &px_uart1_data;
        break;
#endif
#if PX_UART_CFG_UART2_EN
    case PX_UART_PER_2:
        uart_data = &px_uart2_data;
        break;
#endif
#if PX_UART_CFG_UART4_EN
    case PX_UART_PER_4:
        uart_data = &px_uart4_data;
        break;
#endif
#if PX_UART_CFG_UART5_EN
    case PX_UART_PER_5:
        uart_data = &px_uart5_data;
        break;
#endif
    default:
        PX_DBG_ERR("Invalid peripheral specified");
        return false;
    }

    // Already open?
    if(uart_data->open_counter != 0)
    {
        PX_DBG_ERR("Only one handle per UART peripheral can be opened");
        return false;
    }

    // Set transmit finished flag
    uart_data->tx_finished = true;

    // Initialise peripheral
    if(!px_uart_init_peripheral(uart_data->usart_base_adr,
                                peripheral,
                                baud, 
                                data_bits, 
                                parity, 
                                stop_bits))
    {
        // Invalid parameter specified
        return false;
    }

    // Point handle to peripheral
    handle->uart_data = uart_data;

    // Success
    uart_data->open_counter = 1;
    return true;
}

bool px_uart_close(px_uart_handle_t * handle)
{
    px_uart_data_t * uart_data;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    uart_data = handle->uart_data;
    // Check that handle is open
    PX_DBG_ASSERT(uart_data != NULL);

    // Already closed?
    if(uart_data->open_counter == 0)
    {
        PX_DBG_ERR("Peripheral already closed");
        return false;
    }

    // Disable interrupt handler
    switch(uart_data->peripheral)
    {
#if PX_UART_CFG_UART1_EN
    case PX_UART_PER_1:
        NVIC_DisableIRQ(USART1_IRQn);
        break;
#endif

#if PX_UART_CFG_UART2_EN
    case PX_UART_PER_2:
        NVIC_DisableIRQ(USART2_IRQn);
        break;
#endif

#if PX_UART_CFG_UART4_EN
    case PX_UART_PER_4:
#if PX_UART_CFG_UART5_EN
        if(px_uart5_data.open_counter == 0)
#endif
        {
            NVIC_DisableIRQ(USART4_5_IRQn);
        }
        break;
#endif

#if PX_UART_CFG_UART5_EN
    case PX_UART_PER_5:
#if PX_UART_CFG_UART4_EN
        if(px_uart4_data.open_counter == 0)
#endif					
        {
            NVIC_DisableIRQ(USART4_5_IRQn);
        }
        break;
#endif

    default:
        return false;
    }

    // Disable peripheral
    uart_data->usart_base_adr->CR1 = 0;

    // Disable peripheral clock
    switch(uart_data->peripheral)
    {
#if PX_UART_CFG_UART1_EN
    case PX_UART_PER_1:
        LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_USART1);
        break;
#endif
#if PX_UART_CFG_UART2_EN
    case PX_UART_PER_2:
        LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_USART2);
        break;
#endif
#if PX_UART_CFG_UART4_EN
    case PX_UART_PER_4:
        LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_USART4);
        break;
#endif
#if PX_UART_CFG_UART5_EN
    case PX_UART_PER_5:
        LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_USART5);
        break;
#endif
    default:
        PX_DBG_ERR("Invalid peripheral");
        return false;
    }

    // Close handle
    handle->uart_data = NULL;

    // Success
    uart_data->open_counter = 0;
    return true;
}

void px_uart_put_char(px_uart_handle_t * handle, char data)
{
    px_uart_data_t * uart_data;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    uart_data = handle->uart_data;
    // Check that handle is open
    PX_DBG_ASSERT(uart_data != NULL);
    PX_DBG_ASSERT(uart_data->open_counter != 0);
    PX_DBG_ASSERT(uart_data->usart_base_adr != NULL);

    // Wait until transmit buffer has space for one byte and add it
    while(!px_circ_buf_wr_u8(&uart_data->tx_circ_buf, (uint8_t)data))
    {
        ;
    }

    // Make sure transmit process is started by enabling interrupt
    px_uart_start_tx(uart_data->usart_base_adr);
}

bool px_uart_wr_u8(px_uart_handle_t * handle, uint8_t data)
{
    px_uart_data_t * uart_data;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    uart_data = handle->uart_data;
    // Check that handle is open
    PX_DBG_ASSERT(uart_data != NULL);
    PX_DBG_ASSERT(uart_data->open_counter != 0);
    PX_DBG_ASSERT(uart_data->usart_base_adr != NULL);

    // Add byte to transmit buffer
    if(!px_circ_buf_wr_u8(&uart_data->tx_circ_buf, (uint8_t)data))
    {
        // Could not buffer byte for transmission
        return false;
    }

    // Make sure transmit process is started by enabling interrupt
    px_uart_start_tx(uart_data->usart_base_adr);

    // Success
    return true;
}

size_t px_uart_wr(px_uart_handle_t * handle, 
                  const void *       data, 
                  size_t             nr_of_bytes)
{
    px_uart_data_t * uart_data;
    uint8_t  *       data_u8 = (uint8_t *)data;
    size_t           bytes_buffered = 0;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    uart_data = handle->uart_data;
    // Check that handle is open
    PX_DBG_ASSERT(uart_data != NULL);
    PX_DBG_ASSERT(uart_data->open_counter != 0);
    PX_DBG_ASSERT(uart_data->usart_base_adr != NULL);

    // Add bytes to transmit buffer
    bytes_buffered = px_circ_buf_wr(&uart_data->tx_circ_buf, 
                                         data_u8, 
                                         nr_of_bytes);
    
    if(bytes_buffered != 0)
    {
        // Make sure transmit process is started by enabling interrupt
        px_uart_start_tx(uart_data->usart_base_adr);
    }

    // Indicate number of bytes buffered
    return bytes_buffered;
}

char px_uart_get_char(px_uart_handle_t * handle)
{
    px_uart_data_t * uart_data;
    uint8_t          data;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    uart_data = handle->uart_data;
    // Check that handle is open
    PX_DBG_ASSERT(uart_data != NULL);
    PX_DBG_ASSERT(uart_data->open_counter != 0);
    PX_DBG_ASSERT(uart_data->usart_base_adr != NULL);

    // Wait until a byte is in receive buffer and fetch it
    while(!px_circ_buf_rd_u8(&uart_data->rx_circ_buf, &data))
    {
        ;
    }

    return (char)data;
}

bool px_uart_rd_u8(px_uart_handle_t * handle, uint8_t * data)
{
    px_uart_data_t * uart_data;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    uart_data = handle->uart_data;
    // Check that handle is open
    PX_DBG_ASSERT(uart_data != NULL);
    PX_DBG_ASSERT(uart_data->open_counter != 0);
    PX_DBG_ASSERT(uart_data->usart_base_adr != NULL);

    // Return byte from receive buffer (if it is available)
    return px_circ_buf_rd_u8(&uart_data->rx_circ_buf, data);
}

size_t px_uart_rd(px_uart_handle_t * handle, 
                  void *             buffer, 
                  size_t             nr_of_bytes)
{
    px_uart_data_t * uart_data;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    uart_data = handle->uart_data;
    // Check that handle is open
    PX_DBG_ASSERT(uart_data != NULL);
    PX_DBG_ASSERT(uart_data->open_counter != 0);
    PX_DBG_ASSERT(uart_data->usart_base_adr != NULL);

    // Fetch data from receive buffer (up to the specified number of bytes)
    return px_circ_buf_rd(&uart_data->rx_circ_buf, 
                               buffer, 
                               nr_of_bytes);
}

bool px_uart_wr_buf_full(px_uart_handle_t * handle)
{
    px_uart_data_t * uart_data;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    uart_data = handle->uart_data;
    // Check that handle is open
    PX_DBG_ASSERT(uart_data != NULL);
    PX_DBG_ASSERT(uart_data->open_counter != 0);
    PX_DBG_ASSERT(uart_data->usart_base_adr != NULL);

    return px_circ_buf_full(&uart_data->tx_circ_buf);
}

bool px_uart_wr_buf_empty(px_uart_handle_t * handle)
{
    px_uart_data_t * uart_data;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    uart_data = handle->uart_data;
    // Check that handle is open
    PX_DBG_ASSERT(uart_data != NULL);
    PX_DBG_ASSERT(uart_data->open_counter != 0);
    PX_DBG_ASSERT(uart_data->usart_base_adr != NULL);

    return px_circ_buf_empty(&uart_data->tx_circ_buf);
}

bool px_uart_wr_finished(px_uart_handle_t * handle)
{
    px_uart_data_t * uart_data;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    uart_data = handle->uart_data;
    // Check that handle is open
    PX_DBG_ASSERT(uart_data != NULL);
    PX_DBG_ASSERT(uart_data->open_counter != 0);
    PX_DBG_ASSERT(uart_data->usart_base_adr != NULL);

    // Any data to be transmitted in buffer?
    if(!px_circ_buf_empty(&uart_data->tx_circ_buf))
    {
        return false;
    }
    // Return transmission finished flag
    return uart_data->tx_finished;
}

bool px_uart_rd_buf_empty(px_uart_handle_t * handle)
{
    px_uart_data_t * uart_data;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    uart_data = handle->uart_data;
    // Check that handle is open
    PX_DBG_ASSERT(uart_data != NULL);
    PX_DBG_ASSERT(uart_data->open_counter != 0);
    PX_DBG_ASSERT(uart_data->usart_base_adr != NULL);

    return px_circ_buf_empty(&uart_data->rx_circ_buf);
}

void px_uart_ioctl_change_baud(px_uart_handle_t * handle, uint32_t baud)
{
    px_uart_data_t * uart_data;

    // Verify that pointer to handle is not NULL
    PX_DBG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    uart_data = handle->uart_data;
    // Check that handle is open
    PX_DBG_ASSERT(uart_data != NULL);
    PX_DBG_ASSERT(uart_data->open_counter != 0);
    PX_DBG_ASSERT(uart_data->usart_base_adr != NULL);

    // Set baud rate
    LL_USART_SetBaudRate(uart_data->usart_base_adr,
                         PX_BOARD_PER_CLK_HZ, 
                         LL_USART_OVERSAMPLING_16,
                         baud);
}
