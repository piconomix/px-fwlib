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

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdlib.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_lpuart.h"
#include "px_board.h"
#include "px_ring_buf.h"
#include "px_stm32cube.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("lpuart");

/// Internal data for each peripheral
typedef struct px_lpuart_per_s
{
    USART_TypeDef * lpuart_base_adr;    ///< Peripheral base register address
    uint8_t         open_counter;       ///< Number of open handles referencing peripheral
    volatile bool   tx_done;            ///< Transmit done flag
    px_ring_buf_t   tx_ring_buf;        ///< Transmit ring buffer
    px_ring_buf_t   rx_ring_buf;        ///< Receive ring buffer
} px_lpuart_per_t;

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
/// Allocate data for LPUART peripheral
static px_lpuart_per_t px_lpuart_per;
static uint8_t         px_lpuart_tx_buf[PX_LPUART_CFG_LPUART_TX_BUF_SIZE];
static uint8_t         px_lpuart_rx_buf[PX_LPUART_CFG_LPUART_RX_BUF_SIZE];

/* _____LOCAL FUNCTIONS______________________________________________________ */
/// Generic interrupt handler
static void lpuart_irq_handler(px_lpuart_per_t * lpuart_per)
{
    USART_TypeDef * lpuart_base_adr = lpuart_per->lpuart_base_adr;
    uint8_t         data;

    // Received a byte?
    if(LL_LPUART_IsActiveFlag_RXNE(lpuart_base_adr))
    {
        bool discard = false;

        // Read receive data register
        data = LL_LPUART_ReceiveData8(lpuart_base_adr);
        // Overrrun Error?
        if(LL_LPUART_IsActiveFlag_ORE(lpuart_base_adr))
        {
            // Clear flag. Did not service IRQ fast enough and missed byte(s). Ignore
            LL_LPUART_ClearFlag_ORE(lpuart_base_adr);
        }
        // Framing Error?
        if(LL_LPUART_IsActiveFlag_FE(lpuart_base_adr))
        {
            // Clear flag
            LL_LPUART_ClearFlag_FE(lpuart_base_adr);
            // Received byte not correct. Discard
            discard = true;
        }
        // Parity Error?
        if(LL_LPUART_IsActiveFlag_PE(lpuart_base_adr))
        {
            // Clear flag
            LL_LPUART_ClearFlag_PE(lpuart_base_adr);
            // Received byte not correct. Discard
            discard = true;
        }
        // Buffer received byte?
        if(!discard)
        {
            // Parity enabled?
            uint32_t usart_cr1_val = lpuart_base_adr->CR1;
            if(usart_cr1_val & USART_CR1_PCE)
            {
                // 8 data bits?
                if(  ((usart_cr1_val & USART_CR1_M1) == 0) && ((usart_cr1_val & USART_CR1_M0) == 0)  )
                {
                    // Clear parity bit in received byte
                    data &= ~(1 << 7);
                }
                // 7 data bits?
                else if(  ((usart_cr1_val & USART_CR1_M1) != 0) && ((usart_cr1_val & USART_CR1_M0) == 0)  )
                {
                    // Clear parity bit in received byte
                    data &= ~(1 << 6);
                }
            }
            // Add received byte to ring buffer (byte is discarded if buffer is full)
            px_ring_buf_wr_u8(&lpuart_per->rx_ring_buf, data);
        }
    }

    // Transmit data register interrupt enabled?
    if(LL_LPUART_IsEnabledIT_TXE(lpuart_base_adr))
    {
        // Transmit data register empty?
        if(LL_LPUART_IsActiveFlag_TXE(lpuart_base_adr))
        {
            // Data to transmit?
            if(px_ring_buf_rd_u8(&lpuart_per->tx_ring_buf, &data))
            {
                // Load transmit register with data
                LL_LPUART_TransmitData8(lpuart_base_adr, data);
                // Clear flag to indicate that transmission is busy
                lpuart_per->tx_done = false;
            }
            else
            {
                // Disable Transmit data register empty interrupt
                LL_LPUART_DisableIT_TXE(lpuart_base_adr);
                // Enable Transmit complete interrupt
                LL_LPUART_EnableIT_TC(lpuart_base_adr);
            }
        }
    }

    // Transmit complete interrupt enabled?
    if(LL_LPUART_IsEnabledIT_TC(lpuart_base_adr))
    {
        // Transmit complete?
        if(LL_LPUART_IsActiveFlag_TC(lpuart_base_adr))
        {
            // Set flag to indicate that transmission has finished
            lpuart_per->tx_done = true;
            // Disable Transmit complete interrupt
            LL_LPUART_DisableIT_TC(lpuart_base_adr);
        }
    }
}

/// LPUART1 interrupt handler
void LPUART1_IRQHandler(void)
{
    lpuart_irq_handler(&px_lpuart_per);
}

static inline void px_lpuart_start_tx(USART_TypeDef * lpuart_base_adr)
{
    // Sanity check
    PX_LOG_ASSERT(lpuart_base_adr != NULL);

    // Enable Transmit data register empty interrupt
    LL_LPUART_EnableIT_TXE(lpuart_base_adr);
}

static bool px_lpuart_init_baud(USART_TypeDef * lpuart_base_adr,
                                uint32_t        baud)
{
    uint32_t lpuart_clk_hz;

    // Determine peripheral clock (in Hertz)
    lpuart_clk_hz = PX_LPUART_CFG_CLK_HZ;
    // Set BAUD
    LL_LPUART_SetBaudRate(lpuart_base_adr, lpuart_clk_hz, baud);

#if PX_LOG
    {
        uint32_t brr = lpuart_base_adr->BRR;
        PX_LOG_ASSERT((brr >= LPUART_BRR_MIN_VALUE) && (brr <= LPUART_BRR_MASK));
    }
#endif

    // Success
    return true;
}

static bool px_lpuart_init_data_format(USART_TypeDef *       lpuart_base_adr,
                                       px_lpuart_data_bits_t data_bits,
                                       px_lpuart_parity_t    parity,
                                       px_lpuart_stop_bits_t stop_bits)
{
    // Parity specified?
    if(parity != PX_LPUART_PARITY_NONE)
    {
        // Increment data bits to include parity bit
        if(data_bits == PX_LPUART_DATA_BITS_7)
        {
            data_bits = PX_LPUART_DATA_BITS_8;
        }
        else if(data_bits == PX_LPUART_DATA_BITS_8)
        {
            data_bits = PX_LPUART_DATA_BITS_9;
        }
        else
        {
            PX_LOG_E("Parity invalid");
            return false;
        }
    }
    // Set number of data bits
    switch(data_bits)
    {
    case PX_LPUART_DATA_BITS_7: LL_LPUART_SetDataWidth(lpuart_base_adr, LL_LPUART_DATAWIDTH_7B); break;
    case PX_LPUART_DATA_BITS_8: LL_LPUART_SetDataWidth(lpuart_base_adr, LL_LPUART_DATAWIDTH_8B); break;
    case PX_LPUART_DATA_BITS_9: LL_LPUART_SetDataWidth(lpuart_base_adr, LL_LPUART_DATAWIDTH_9B); break;
    default: PX_LOG_E("Invalid data bits"); return false;
    }
    // Set parity
    switch(parity)
    {
    case PX_LPUART_PARITY_NONE: LL_LPUART_SetParity(lpuart_base_adr, LL_LPUART_PARITY_NONE); break;
    case PX_LPUART_PARITY_ODD:  LL_LPUART_SetParity(lpuart_base_adr, LL_LPUART_PARITY_ODD);  break;
    case PX_LPUART_PARITY_EVEN: LL_LPUART_SetParity(lpuart_base_adr, LL_LPUART_PARITY_EVEN); break;
    default: PX_LOG_E("Invalid parity"); return false;
    }
    // Set stop bits
    switch(stop_bits)
    {
    case PX_LPUART_STOP_BITS_1: LL_LPUART_SetStopBitsLength(lpuart_base_adr, LL_LPUART_STOPBITS_1); break;
    case PX_LPUART_STOP_BITS_2: LL_LPUART_SetStopBitsLength(lpuart_base_adr, LL_LPUART_STOPBITS_2); break;
    default: PX_LOG_E("Invalid stop bits"); return false;
    }

    // Success
    return true;
}

static bool px_lpuart_init_per(USART_TypeDef *       lpuart_base_adr,
                               uint32_t              baud,
                               px_lpuart_data_bits_t data_bits,
                               px_lpuart_parity_t    parity,
                               px_lpuart_stop_bits_t stop_bits)
{
    // Sanity check
    PX_LOG_ASSERT(lpuart_base_adr != NULL);

    // Select clock source
#if   (PX_LPUART_CFG_CLK_SRC == 1)
    LL_RCC_SetLPUARTClockSource(LL_RCC_LPUART1_CLKSOURCE_PCLK1);
#elif (PX_LPUART_CFG_CLK_SRC == 2)
    LL_RCC_SetLPUARTClockSource(LL_RCC_LPUART1_CLKSOURCE_SYSCLK);
#elif (PX_LPUART_CFG_CLK_SRC == 3)
    LL_RCC_SetLPUARTClockSource(LL_RCC_LPUART1_CLKSOURCE_HSI);
#elif (PX_LPUART_CFG_CLK_SRC == 4)
    LL_RCC_SetLPUARTClockSource(LL_RCC_LPUART1_CLKSOURCE_LSE);
#endif
    // Enable peripheral clock
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_LPUART1);
    // Set baud rate
    if(!px_lpuart_init_baud(lpuart_base_adr, baud))
    {
        return false;
    }
    // Set data format
    if(!px_lpuart_init_data_format(lpuart_base_adr, data_bits, parity, stop_bits))
    {
        return false;
    }
    // Enable transmitter, receiver and receive interrupt
    LL_LPUART_EnableDirectionTx(lpuart_base_adr);
    LL_LPUART_EnableDirectionRx(lpuart_base_adr);
    LL_LPUART_EnableIT_RXNE(lpuart_base_adr);
    // Enable wake up from STOP mode
    LL_LPUART_EnableInStopMode(lpuart_base_adr);
    // Enable LPUART
    LL_LPUART_Enable(lpuart_base_adr);
    // Enable interrupt handler
    NVIC_EnableIRQ(LPUART1_IRQn);

    // Success
    return true;
}

static void px_lpuart_init_per_data(px_lpuart_per_t * lpuart_per)
{
    // Set peripheral base address and intialise ring buffers
    lpuart_per->lpuart_base_adr = LPUART1;
    px_ring_buf_init(&lpuart_per->tx_ring_buf, px_lpuart_tx_buf, PX_LPUART_CFG_LPUART_TX_BUF_SIZE);
    px_ring_buf_init(&lpuart_per->rx_ring_buf, px_lpuart_rx_buf, PX_LPUART_CFG_LPUART_RX_BUF_SIZE);
    // Clear open counter
    lpuart_per->open_counter = 0;
    // Set transmit done flag
    lpuart_per->tx_done = true;
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_lpuart_init(void)
{
    // Initialize peripheral data
    px_lpuart_init_per_data(&px_lpuart_per);
}

bool px_lpuart_open(px_lpuart_handle_t * handle)
{
    return px_lpuart_open2(handle,
                           PX_LPUART_CFG_DEFAULT_BAUD,
                           PX_LPUART_DATA_BITS_8,
                           PX_LPUART_PARITY_NONE,
                           PX_LPUART_STOP_BITS_1);
}

bool px_lpuart_open2(px_lpuart_handle_t *  handle,
                     uint32_t              baud,
                     px_lpuart_data_bits_t data_bits,
                     px_lpuart_parity_t    parity,
                     px_lpuart_stop_bits_t stop_bits)
{
    px_lpuart_per_t * lpuart_per;

    // Verify that pointer to handle is not NULL
    PX_LOG_ASSERT(handle != NULL);

    // Handle not initialised
    handle->lpuart_per = NULL;
    // Set pointer to peripheral data
    lpuart_per = &px_lpuart_per;
    // Already open?
    if(lpuart_per->open_counter != 0)
    {
        PX_LOG_E("Already open");
        return false;
    }
    // Set transmit done flag
    lpuart_per->tx_done = true;
    // Initialise peripheral
    if(!px_lpuart_init_per(lpuart_per->lpuart_base_adr,
                           baud,
                           data_bits,
                           parity,
                           stop_bits))
    {
        // Invalid parameter specified
        return false;
    }
    // Point handle to peripheral
    handle->lpuart_per = lpuart_per;

    // Success
    lpuart_per->open_counter = 1;
    return true;
}

bool px_lpuart_close(px_lpuart_handle_t * handle)
{
    px_lpuart_per_t * lpuart_per;

    // Sanity checks
    PX_LOG_ASSERT(    (handle             != NULL)
                   && (handle->lpuart_per != NULL)  );
    // Set pointer to peripheral
    lpuart_per = handle->lpuart_per;
    // Already closed?
    if(lpuart_per->open_counter == 0)
    {
        PX_LOG_E("Already closed");
        return false;
    }

    // Disable interrupt handler
    NVIC_DisableIRQ(LPUART1_IRQn);
    // Disable peripheral
    lpuart_per->lpuart_base_adr->CR1 = 0;
    // Disable peripheral clock
    LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_LPUART1);
    // Close handle
    handle->lpuart_per = NULL;

    // Success
    lpuart_per->open_counter = 0;
    return true;
}

void px_lpuart_putchar(px_lpuart_handle_t * handle, char data)
{
    px_lpuart_per_t * lpuart_per;

    // Sanity checks
    PX_LOG_ASSERT(    (handle                              != NULL)
                   && (handle->lpuart_per                  != NULL)
                   && (handle->lpuart_per->lpuart_base_adr != NULL)
                   && (handle->lpuart_per->open_counter    != 0   )  );

    // Set pointer to peripheral
    lpuart_per = handle->lpuart_per;

    // Wait until transmit buffer has space for one byte and add it
    while(!px_ring_buf_wr_u8(&lpuart_per->tx_ring_buf, (uint8_t)data)) {;}
    // Make sure transmit process is started by enabling interrupt
    px_lpuart_start_tx(lpuart_per->lpuart_base_adr);
}

bool px_lpuart_wr_u8(px_lpuart_handle_t * handle, uint8_t data)
{
    px_lpuart_per_t * lpuart_per;

    // Sanity checks
    PX_LOG_ASSERT(    (handle                              != NULL)
                   && (handle->lpuart_per                  != NULL)
                   && (handle->lpuart_per->lpuart_base_adr != NULL)
                   && (handle->lpuart_per->open_counter    != 0   )  );

    // Set pointer to peripheral
    lpuart_per = handle->lpuart_per;

    // Add byte to transmit buffer
    if(!px_ring_buf_wr_u8(&lpuart_per->tx_ring_buf, (uint8_t)data))
    {
        // Could not buffer byte for transmission
        return false;
    }
    // Make sure transmit process is started by enabling interrupt
    px_lpuart_start_tx(lpuart_per->lpuart_base_adr);

    // Success
    return true;
}

size_t px_lpuart_wr(px_lpuart_handle_t * handle,
                    const void *       data,
                    size_t             nr_of_bytes)
{
    px_lpuart_per_t * lpuart_per;
    uint8_t  *      data_u8 = (uint8_t *)data;
    size_t          bytes_buffered = 0;

    // Sanity checks
    PX_LOG_ASSERT(    (handle                              != NULL)
                   && (handle->lpuart_per                  != NULL)
                   && (handle->lpuart_per->lpuart_base_adr != NULL)
                   && (handle->lpuart_per->open_counter    != 0   )  );

    // Set pointer to peripheral
    lpuart_per = handle->lpuart_per;

    // Add bytes to transmit buffer
    bytes_buffered = px_ring_buf_wr(&lpuart_per->tx_ring_buf, data_u8, nr_of_bytes);
    if(bytes_buffered > 0)
    {
        // Make sure transmit process is started by enabling interrupt
        px_lpuart_start_tx(lpuart_per->lpuart_base_adr);
    }

    // Indicate number of bytes buffered
    return bytes_buffered;
}

char px_lpuart_getchar(px_lpuart_handle_t * handle)
{
    px_lpuart_per_t * lpuart_per;
    uint8_t           data;

    // Sanity checks
    PX_LOG_ASSERT(    (handle                              != NULL)
                   && (handle->lpuart_per                  != NULL)
                   && (handle->lpuart_per->lpuart_base_adr != NULL)
                   && (handle->lpuart_per->open_counter    != 0   )  );

    // Set pointer to peripheral
    lpuart_per = handle->lpuart_per;

    // Wait until a byte is in receive buffer and fetch it
    while(!px_ring_buf_rd_u8(&lpuart_per->rx_ring_buf, &data)) {;}

    return (char)data;
}

bool px_lpuart_rd_u8(px_lpuart_handle_t * handle, uint8_t * data)
{
    px_lpuart_per_t * lpuart_per;

    // Sanity checks
    PX_LOG_ASSERT(    (handle                              != NULL)
                   && (handle->lpuart_per                  != NULL)
                   && (handle->lpuart_per->lpuart_base_adr != NULL)
                   && (handle->lpuart_per->open_counter    != 0   )  );

    // Set pointer to peripheral
    lpuart_per = handle->lpuart_per;

    // Return byte from receive buffer (if it is available)
    return px_ring_buf_rd_u8(&lpuart_per->rx_ring_buf, data);
}

size_t px_lpuart_rd(px_lpuart_handle_t * handle,
                    void *               buf,
                    size_t               nr_of_bytes)
{
    px_lpuart_per_t * lpuart_per;

    // Sanity checks
    PX_LOG_ASSERT(    (handle                              != NULL)
                   && (handle->lpuart_per                  != NULL)
                   && (handle->lpuart_per->lpuart_base_adr != NULL)
                   && (handle->lpuart_per->open_counter    != 0   )  );

    // Set pointer to peripheral
    lpuart_per = handle->lpuart_per;

    // Fetch data from receive buffer (up to the specified number of bytes)
    return px_ring_buf_rd(&lpuart_per->rx_ring_buf, buf, nr_of_bytes);
}

bool px_lpuart_wr_buf_is_full(px_lpuart_handle_t * handle)
{
    px_lpuart_per_t * lpuart_per;

    // Sanity checks
    PX_LOG_ASSERT(    (handle                              != NULL)
                   && (handle->lpuart_per                  != NULL)
                   && (handle->lpuart_per->lpuart_base_adr != NULL)
                   && (handle->lpuart_per->open_counter    != 0   )  );

    // Set pointer to peripheral
    lpuart_per = handle->lpuart_per;

    return px_ring_buf_is_full(&lpuart_per->tx_ring_buf);
}

bool px_lpuart_wr_buf_is_empty(px_lpuart_handle_t * handle)
{
    px_lpuart_per_t * lpuart_per;

    // Sanity checks
    PX_LOG_ASSERT(    (handle                              != NULL)
                   && (handle->lpuart_per                  != NULL)
                   && (handle->lpuart_per->lpuart_base_adr != NULL)
                   && (handle->lpuart_per->open_counter    != 0   )  );

    // Set pointer to peripheral
    lpuart_per = handle->lpuart_per;

    return px_ring_buf_is_empty(&lpuart_per->tx_ring_buf);
}

bool px_lpuart_wr_is_done(px_lpuart_handle_t * handle)
{
    px_lpuart_per_t * lpuart_per;

    // Sanity checks
    PX_LOG_ASSERT(    (handle                              != NULL)
                   && (handle->lpuart_per                  != NULL)
                   && (handle->lpuart_per->lpuart_base_adr != NULL)
                   && (handle->lpuart_per->open_counter    != 0   )  );

    // Set pointer to peripheral
    lpuart_per = handle->lpuart_per;

    // Any data to be transmitted in buffer?
    if(!px_ring_buf_is_empty(&lpuart_per->tx_ring_buf))
    {
        return false;
    }

    // Return transmission done flag
    return lpuart_per->tx_done;
}

bool px_lpuart_rd_buf_is_empty(px_lpuart_handle_t * handle)
{
    px_lpuart_per_t * lpuart_per;

    // Sanity checks
    PX_LOG_ASSERT(    (handle                              != NULL)
                   && (handle->lpuart_per                  != NULL)
                   && (handle->lpuart_per->lpuart_base_adr != NULL)
                   && (handle->lpuart_per->open_counter    != 0   )  );

    // Set pointer to peripheral
    lpuart_per = handle->lpuart_per;

    return px_ring_buf_is_empty(&lpuart_per->rx_ring_buf);
}

bool px_lpuart_change_baud(px_lpuart_handle_t * handle, uint32_t baud)
{
    px_lpuart_per_t * lpuart_per;
    USART_TypeDef *   lpuart_base_adr;
    bool              result;
    bool              enable;

    // Sanity checks
    PX_LOG_ASSERT(    (handle                              != NULL)
                   && (handle->lpuart_per                  != NULL)
                   && (handle->lpuart_per->lpuart_base_adr != NULL)
                   && (handle->lpuart_per->open_counter    != 0   )  );

    // Set pointer to peripheral
    lpuart_per = handle->lpuart_per;

    // Set pointer to peripheral base address
    lpuart_base_adr = lpuart_per->lpuart_base_adr;
    // Is LPUART enabled?
    if(LL_LPUART_IsEnabled(lpuart_base_adr))
    {
        enable = true;
    }
    else
    {
        enable = false;
    }
    // Disable LPUART
    LL_LPUART_Disable(lpuart_base_adr);
    // Set baud rate
    result = px_lpuart_init_baud(lpuart_base_adr, baud);
    if(enable)
    {
        // Enable LPUART
        LL_LPUART_Enable(lpuart_base_adr);
    }
    return result;
}

bool px_lpuart_change_data_format(px_lpuart_handle_t *  handle,
                                  px_lpuart_data_bits_t data_bits,
                                  px_lpuart_parity_t    parity,
                                  px_lpuart_stop_bits_t stop_bits)
{
    px_lpuart_per_t * lpuart_per;
    USART_TypeDef *   lpuart_base_adr;
    bool              result;
    bool              enable;

    // Sanity checks
    PX_LOG_ASSERT(    (handle                              != NULL)
                   && (handle->lpuart_per                  != NULL)
                   && (handle->lpuart_per->lpuart_base_adr != NULL)
                   && (handle->lpuart_per->open_counter    != 0   )  );

    // Set pointer to peripheral
    lpuart_per = handle->lpuart_per;

    // Set pointer to peripheral base address
    lpuart_base_adr = lpuart_per->lpuart_base_adr;
    // Is LPUART enabled?
    if(LL_LPUART_IsEnabled(lpuart_base_adr))
    {
        enable = true;
    }
    else
    {
        enable = false;
    }
    // Disable LPUART
    LL_LPUART_Disable(lpuart_base_adr);
    // Set data format
    result = px_lpuart_init_data_format(lpuart_base_adr, data_bits, parity, stop_bits);
    if(enable)
    {
        // Enable LPUART
        LL_LPUART_Enable(lpuart_base_adr);
    }
    return result;
}
