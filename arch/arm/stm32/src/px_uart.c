/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md

    Title:          px_uart.h : UART peripheral driver
    Author(s):      Pieter Conradie
    Creation Date:  2018-02-01

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdlib.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_uart.h"
#include "px_board.h"
#include "px_ring_buf.h"
#include "px_lib_stm32cube.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("uart");

/// Internal data for each peripheral
typedef struct px_uart_per_s
{
    USART_TypeDef * usart_base_adr;     /// Peripheral base register address
    px_uart_nr_t    uart_nr;            /// Peripheral
    uint8_t         open_counter;       /// Number of open handles referencing peripheral
    volatile bool   tx_done;            /// Transmit done flag
    px_ring_buf_t   tx_ring_buf;        /// Transmit ring buffer
    px_ring_buf_t   rx_ring_buf;        /// Receive ring buffer
} px_uart_per_t;

// Default peripheral clocks in Hz
#ifndef PX_UART_CFG_UART1_CLK_HZ
    #define PX_UART_CFG_UART1_CLK_HZ    PX_BOARD_PER_CLK_HZ
#endif
#ifndef PX_UART_CFG_UART2_CLK_HZ
    #define PX_UART_CFG_UART2_CLK_HZ    PX_BOARD_PER_CLK_HZ
#endif
#ifndef PX_UART_CFG_UART3_CLK_HZ
    #define PX_UART_CFG_UART3_CLK_HZ    PX_BOARD_PER_CLK_HZ
#endif
#ifndef PX_UART_CFG_UART4_CLK_HZ
    #define PX_UART_CFG_UART4_CLK_HZ    PX_BOARD_PER_CLK_HZ
#endif
#ifndef PX_UART_CFG_UART5_CLK_HZ
    #define PX_UART_CFG_UART5_CLK_HZ    PX_BOARD_PER_CLK_HZ
#endif

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
/// Allocate data for each enabled UART peripheral
#if PX_UART_CFG_UART1_EN
static px_uart_per_t px_uart_per_1;
static uint8_t       px_uart1_tx_buf[PX_UART_CFG_UART1_TX_BUF_SIZE];
static uint8_t       px_uart1_rx_buf[PX_UART_CFG_UART1_RX_BUF_SIZE];
#endif

#if PX_UART_CFG_UART2_EN
static px_uart_per_t px_uart_per_2;
static uint8_t       px_uart2_tx_buf[PX_UART_CFG_UART2_TX_BUF_SIZE];
static uint8_t       px_uart2_rx_buf[PX_UART_CFG_UART2_RX_BUF_SIZE];
#endif

#if PX_UART_CFG_UART3_EN
static px_uart_per_t px_uart_per_3;
static uint8_t       px_uart3_tx_buf[PX_UART_CFG_UART3_TX_BUF_SIZE];
static uint8_t       px_uart3_rx_buf[PX_UART_CFG_UART3_RX_BUF_SIZE];
#endif

#if PX_UART_CFG_UART4_EN
static px_uart_per_t px_uart_per_4;
static uint8_t       px_uart4_tx_buf[PX_UART_CFG_UART4_TX_BUF_SIZE];
static uint8_t       px_uart4_rx_buf[PX_UART_CFG_UART4_RX_BUF_SIZE];
#endif

#if PX_UART_CFG_UART5_EN
static px_uart_per_t px_uart_per_5;
static uint8_t       px_uart5_tx_buf[PX_UART_CFG_UART5_TX_BUF_SIZE];
static uint8_t       px_uart5_rx_buf[PX_UART_CFG_UART5_RX_BUF_SIZE];
#endif

/* _____LOCAL FUNCTIONS______________________________________________________ */
/// Generic interrupt handler
static void uart_irq_handler(px_uart_per_t * uart_per)
{
    USART_TypeDef * usart_base_adr = uart_per->usart_base_adr;
    uint8_t         data;

    // Received a byte?
    if(LL_USART_IsActiveFlag_RXNE(usart_base_adr))
    {
        bool discard = false;

        // Read receive data register
        data = LL_USART_ReceiveData8(usart_base_adr);

        // Overrrun Error?
        if(LL_USART_IsActiveFlag_ORE(usart_base_adr))
        {
            // Clear flag. Did not service IRQ fast enough and missed byte(s). Ignore
            LL_USART_ClearFlag_ORE(usart_base_adr);
        }
        // Framing Error?
        if(LL_USART_IsActiveFlag_FE(usart_base_adr))
        {
            // Clear flag
            LL_USART_ClearFlag_FE(usart_base_adr);
            // Received byte not correct. Discard
            discard = true;
        }
        // Parity Error?
        if(LL_USART_IsActiveFlag_PE(usart_base_adr))
        {
            // Clear flag
            LL_USART_ClearFlag_PE(usart_base_adr);
            // Received byte not correct. Discard
            discard = true;
        }
        // Buffer received byte?
        if(!discard)
        {
            // Parity enabled?
            uint32_t usart_cr1_val = usart_base_adr->CR1;
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
            // Add received byte to circular buffer (byte is discarded if buffer is full)
            px_ring_buf_wr_u8(&uart_per->rx_ring_buf, data);
        }
    }

    // Transmit data register interrupt enabled?
    if(LL_USART_IsEnabledIT_TXE(usart_base_adr))
    {
        // Transmit data register empty?
        if(LL_USART_IsActiveFlag_TXE(usart_base_adr))
        {
            // Data to transmit?
            if(px_ring_buf_rd_u8(&uart_per->tx_ring_buf, &data))
            {
                // Load transmit register with data
                LL_USART_TransmitData8(usart_base_adr, data);
                // Clear flag to indicate that transmission is busy
                uart_per->tx_done = false;
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
            uart_per->tx_done = true;
            // Disable Transmit complete interrupt
            LL_USART_DisableIT_TC(usart_base_adr);
        }
    }
}

#if PX_UART_CFG_UART1_EN
/// USART1 interrupt handler
void USART1_IRQHandler(void)
{
    uart_irq_handler(&px_uart_per_1);
}
#endif

#if PX_UART_CFG_UART2_EN
/// USART2 interrupt handler
void USART2_IRQHandler(void)
{
    uart_irq_handler(&px_uart_per_2);
}
#endif

#if PX_UART_CFG_UART3_EN
/// USART2 interrupt handler
void USART3_IRQHandler(void)
{
    uart_irq_handler(&px_uart_per_3);
}
#endif

#if PX_UART_CFG_UART4_EN || PX_UART_CFG_UART5_EN
/// USART4 & USART5 interrupt handler
void USART4_5_IRQHandler(void)
{
#if PX_UART_CFG_UART4_EN
    uart_irq_handler(&px_uart_per_4);
#endif
#if PX_UART_CFG_UART5_EN
    uart_irq_handler(&px_uart_per_5);
#endif
}
#endif

static inline void px_uart_start_tx(USART_TypeDef * usart_base_adr)
{
    // Sanity check
    PX_LOG_ASSERT(usart_base_adr != NULL);
    // Enable Transmit data register empty interrupt
    LL_USART_EnableIT_TXE(usart_base_adr);
}

static bool px_uart_init_baud(USART_TypeDef * usart_base_adr,
                              px_uart_nr_t    uart_nr,
                              uint32_t        baud)
{
    uint32_t uart_clk_hz;

    // Determine peripheral clock (in Hertz)
    switch(uart_nr)
    {
#if PX_UART_CFG_UART1_EN
    case PX_UART_NR_1: uart_clk_hz = PX_UART_CFG_UART1_CLK_HZ; break;
#endif
#if PX_UART_CFG_UART2_EN
    case PX_UART_NR_2: uart_clk_hz = PX_UART_CFG_UART2_CLK_HZ; break;
#endif
#if PX_UART_CFG_UART3_EN
    case PX_UART_NR_3: uart_clk_hz = PX_UART_CFG_UART3_CLK_HZ; break;
#endif
#if PX_UART_CFG_UART4_EN
    case PX_UART_NR_4: uart_clk_hz = PX_UART_CFG_UART4_CLK_HZ; break;
#endif
#if PX_UART_CFG_UART5_EN
    case PX_UART_NR_5: uart_clk_hz = PX_UART_CFG_UART5_CLK_HZ; break;
#endif
    default:           PX_LOG_E("Invalid peripheral"); return false;
    }
    // Set BAUD
    LL_USART_SetBaudRate(usart_base_adr,
                         uart_clk_hz,
#if STM32G0
                         LL_USART_PRESCALER_DIV1,
#endif
                         LL_USART_OVERSAMPLING_16,
                         baud);
    // Success
    return true;
}

static bool px_uart_init_data_format(USART_TypeDef *     usart_base_adr,
                                     px_uart_data_bits_t data_bits,
                                     px_uart_parity_t    parity,
                                     px_uart_stop_bits_t stop_bits)
{
    // Parity specified?
    if(parity != PX_UART_PARITY_NONE)
    {
        // Increment data bits to include parity bit
        if(data_bits == PX_UART_DATA_BITS_7)
        {
            data_bits = PX_UART_DATA_BITS_8;
        }
        else if(data_bits == PX_UART_DATA_BITS_8)
        {
            data_bits = PX_UART_DATA_BITS_9;
        }
        else
        {
            PX_LOG_E("Parity option invalid");
            return false;
        }
    }
    // Set number of data bits
    switch(data_bits)
    {
    case PX_UART_DATA_BITS_7: LL_USART_SetDataWidth(usart_base_adr, LL_USART_DATAWIDTH_7B); break;
    case PX_UART_DATA_BITS_8: LL_USART_SetDataWidth(usart_base_adr, LL_USART_DATAWIDTH_8B); break;
    case PX_UART_DATA_BITS_9: LL_USART_SetDataWidth(usart_base_adr, LL_USART_DATAWIDTH_9B); break;
    default:                  PX_LOG_E("Invalid number of data bits"); return false;
    }
    // Set parity
    switch(parity)
    {
    case PX_UART_PARITY_NONE: LL_USART_SetParity(usart_base_adr, LL_USART_PARITY_NONE); break;
    case PX_UART_PARITY_ODD:  LL_USART_SetParity(usart_base_adr, LL_USART_PARITY_ODD);  break;
    case PX_UART_PARITY_EVEN: LL_USART_SetParity(usart_base_adr, LL_USART_PARITY_EVEN); break;
    default:                  PX_LOG_E("Invalid parity specified"); return false;
    }
    // Set stop bits
    switch(stop_bits)
    {
    case PX_UART_STOP_BITS_1: LL_USART_SetStopBitsLength(usart_base_adr, LL_USART_STOPBITS_1); break;
    case PX_UART_STOP_BITS_2: LL_USART_SetStopBitsLength(usart_base_adr, LL_USART_STOPBITS_2); break;
    default:                  PX_LOG_E("Invalid number of stop bits"); return false;
    }
    // Success
    return true;
}

static bool px_uart_init_peripheral(USART_TypeDef *     usart_base_adr,
                                    px_uart_nr_t        uart_nr,
                                    uint32_t            baud,
                                    px_uart_data_bits_t data_bits,
                                    px_uart_parity_t    parity,
                                    px_uart_stop_bits_t stop_bits)
{
    // Sanity check
    PX_LOG_ASSERT(usart_base_adr != NULL);
    // Enable peripheral clock
    switch(uart_nr)
    {
#if PX_UART_CFG_UART1_EN
    case PX_UART_NR_1: LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1); break;
#endif
#if PX_UART_CFG_UART2_EN
    case PX_UART_NR_2: LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2); break;
#endif
#if PX_UART_CFG_UART3_EN
    case PX_UART_NR_3: LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3); break;
#endif
#if PX_UART_CFG_UART4_EN
    case PX_UART_NR_4: LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART4); break;
#endif
#if PX_UART_CFG_UART5_EN
    case PX_UART_NR_5: LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART5); break;
#endif
    default:
        PX_LOG_E("Invalid peripheral");
        return false;
    }
    // Set baud rate
    if(!px_uart_init_baud(usart_base_adr, uart_nr, baud))
    {
        return false;
    }
    // Set data format
    if(!px_uart_init_data_format(usart_base_adr, data_bits, parity, stop_bits))
    {
        return false;
    }
    // Enable transmitter, receiver and receive interrupt
    LL_USART_EnableDirectionTx(usart_base_adr);
    LL_USART_EnableDirectionRx(usart_base_adr);
    LL_USART_EnableIT_RXNE(usart_base_adr);
    // Enable UART
    LL_USART_Enable(usart_base_adr);
    // Enable interrupt handler
    switch(uart_nr)
    {
#if PX_UART_CFG_UART1_EN
    case PX_UART_NR_1: NVIC_EnableIRQ(USART1_IRQn);   break;
#endif
#if PX_UART_CFG_UART2_EN
    case PX_UART_NR_2: NVIC_EnableIRQ(USART2_IRQn);   break;
#endif
#if PX_UART_CFG_UART3_EN
    case PX_UART_NR_3: NVIC_EnableIRQ(USART3_IRQn);   break;
#endif
#if PX_UART_CFG_UART4_EN
    case PX_UART_NR_4: NVIC_EnableIRQ(USART4_5_IRQn); break;
#endif
#if PX_UART_CFG_UART5_EN
    case PX_UART_NR_5: NVIC_EnableIRQ(USART4_5_IRQn); break;
#endif
    default:
        break;
    }
    // Success
    return true;
}

static void px_uart_init_peripheral_data(px_uart_nr_t    uart_nr,
                                         px_uart_per_t * uart_per)
{
    // Set peripheral number
    uart_per->uart_nr = uart_nr;
    // Set peripheral base address and intialise circular buffers
    switch(uart_nr)
    {
#if PX_UART_CFG_UART1_EN
    case PX_UART_NR_1:
        uart_per->usart_base_adr = USART1;
        px_ring_buf_init(&uart_per->tx_ring_buf, px_uart1_tx_buf, PX_UART_CFG_UART1_TX_BUF_SIZE);
        px_ring_buf_init(&uart_per->rx_ring_buf, px_uart1_rx_buf, PX_UART_CFG_UART1_RX_BUF_SIZE);
        break;
#endif
#if PX_UART_CFG_UART2_EN
    case PX_UART_NR_2:
        uart_per->usart_base_adr = USART2;
        px_ring_buf_init(&uart_per->tx_ring_buf, px_uart2_tx_buf, PX_UART_CFG_UART2_TX_BUF_SIZE);
        px_ring_buf_init(&uart_per->rx_ring_buf, px_uart2_rx_buf, PX_UART_CFG_UART2_RX_BUF_SIZE);
        break;
#endif
#if PX_UART_CFG_UART3_EN
    case PX_UART_NR_3:
        uart_per->usart_base_adr = USART3;
        px_ring_buf_init(&uart_per->tx_ring_buf, px_uart3_tx_buf, PX_UART_CFG_UART3_TX_BUF_SIZE);
        px_ring_buf_init(&uart_per->rx_ring_buf, px_uart3_rx_buf, PX_UART_CFG_UART3_RX_BUF_SIZE);
        break;
#endif
#if PX_UART_CFG_UART4_EN
    case PX_UART_NR_4:
        uart_per->usart_base_adr = USART4;
        px_ring_buf_init(&uart_per->tx_ring_buf, px_uart4_tx_buf, PX_UART_CFG_UART4_TX_BUF_SIZE);
        px_ring_buf_init(&uart_per->rx_ring_buf, px_uart4_rx_buf, PX_UART_CFG_UART4_RX_BUF_SIZE);
        break;
#endif
#if PX_UART_CFG_UART5_EN
    case PX_UART_NR_5:
        uart_per->usart_base_adr = USART5;
        px_ring_buf_init(&uart_per->tx_ring_buf, px_uart5_tx_buf, PX_UART_CFG_UART5_TX_BUF_SIZE);
        px_ring_buf_init(&uart_per->rx_ring_buf, px_uart5_rx_buf, PX_UART_CFG_UART5_RX_BUF_SIZE);
        break;
#endif
    default:
        break;
    }
    // Clear open counter
    uart_per->open_counter = 0;
    // Set transmit done flag
    uart_per->tx_done = true;
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_uart_init(void)
{
    // Initialize peripheral data for each enabled peripheral
#if PX_UART_CFG_UART1_EN
    px_uart_init_peripheral_data(PX_UART_NR_1, &px_uart_per_1);
#endif
#if PX_UART_CFG_UART2_EN
    px_uart_init_peripheral_data(PX_UART_NR_2, &px_uart_per_2);
#endif
#if PX_UART_CFG_UART3_EN
    px_uart_init_peripheral_data(PX_UART_NR_3, &px_uart_per_3);
#endif
#if PX_UART_CFG_UART4_EN
    px_uart_init_peripheral_data(PX_UART_NR_4, &px_uart_per_4);
#endif
#if PX_UART_CFG_UART5_EN
    px_uart_init_peripheral_data(PX_UART_NR_5, &px_uart_per_5);
#endif
}

bool px_uart_open(px_uart_handle_t * handle,
                  px_uart_nr_t       uart_nr)
{
    return px_uart_open2(handle,
                         uart_nr,
                         PX_UART_CFG_DEFAULT_BAUD,
                         PX_UART_DATA_BITS_8,
                         PX_UART_PARITY_NONE,
                         PX_UART_STOP_BITS_1);
}

bool px_uart_open2(px_uart_handle_t *  handle,
                   px_uart_nr_t        uart_nr,
                   uint32_t            baud,
                   px_uart_data_bits_t data_bits,
                   px_uart_parity_t    parity,
                   px_uart_stop_bits_t stop_bits)
{
    px_uart_per_t * uart_per;

    // Verify that pointer to handle is not NULL
    PX_LOG_ASSERT(handle != NULL);
    // Handle not initialised
    handle->uart_per = NULL;
    // Set pointer to peripheral data
    switch(uart_nr)
    {
#if PX_UART_CFG_UART1_EN
    case PX_UART_NR_1:
        uart_per = &px_uart_per_1;
        break;
#endif
#if PX_UART_CFG_UART2_EN
    case PX_UART_NR_2:
        uart_per = &px_uart_per_2;
        break;
#endif
#if PX_UART_CFG_UART3_EN
    case PX_UART_NR_3:
        uart_per = &px_uart_per_3;
        break;
#endif
#if PX_UART_CFG_UART4_EN
    case PX_UART_NR_4:
        uart_per = &px_uart_per_4;
        break;
#endif
#if PX_UART_CFG_UART5_EN
    case PX_UART_NR_5:
        uart_per = &px_uart_per_5;
        break;
#endif
    default:
        PX_LOG_E("Invalid peripheral specified");
        return false;
    }
    // Already open?
    if(uart_per->open_counter != 0)
    {
        PX_LOG_E("Only one handle per UART peripheral can be opened");
        return false;
    }
    // Set transmit done flag
    uart_per->tx_done = true;
    // Initialise peripheral
    if(!px_uart_init_peripheral(uart_per->usart_base_adr,
                                uart_nr,
                                baud,
                                data_bits,
                                parity,
                                stop_bits))
    {
        // Invalid parameter specified
        return false;
    }
    // Point handle to peripheral
    handle->uart_per = uart_per;
    // Success
    uart_per->open_counter = 1;
    return true;
}

bool px_uart_close(px_uart_handle_t * handle)
{
    px_uart_per_t * uart_per;

    // Verify that pointer to handle is not NULL
    PX_LOG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    uart_per = handle->uart_per;
    // Check that handle is open
    PX_LOG_ASSERT(uart_per != NULL);

    // Already closed?
    if(uart_per->open_counter == 0)
    {
        PX_LOG_E("Peripheral already closed");
        return false;
    }

    // Disable interrupt handler
    switch(uart_per->uart_nr)
    {
#if PX_UART_CFG_UART1_EN
    case PX_UART_NR_1:
        NVIC_DisableIRQ(USART1_IRQn);
        break;
#endif

#if PX_UART_CFG_UART2_EN
    case PX_UART_NR_2:
        NVIC_DisableIRQ(USART2_IRQn);
        break;
#endif

#if PX_UART_CFG_UART3_EN
    case PX_UART_NR_3:
        NVIC_DisableIRQ(USART3_IRQn);
        break;
#endif

#if PX_UART_CFG_UART4_EN
    case PX_UART_NR_4:
#if PX_UART_CFG_UART5_EN
        if(px_uart_per_5.open_counter == 0)
#endif
        {
            NVIC_DisableIRQ(USART4_5_IRQn);
        }
        break;
#endif

#if PX_UART_CFG_UART5_EN
    case PX_UART_NR_5:
#if PX_UART_CFG_UART4_EN
        if(px_uart_per_4.open_counter == 0)
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
    uart_per->usart_base_adr->CR1 = 0;

    // Disable peripheral clock
    switch(uart_per->uart_nr)
    {
#if PX_UART_CFG_UART1_EN
    case PX_UART_NR_1:
        LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_USART1);
        break;
#endif
#if PX_UART_CFG_UART2_EN
    case PX_UART_NR_2:
        LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_USART2);
        break;
#endif
#if PX_UART_CFG_UART3_EN
    case PX_UART_NR_3:
        LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_USART2);
        break;
#endif
#if PX_UART_CFG_UART4_EN
    case PX_UART_NR_4:
        LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_USART4);
        break;
#endif
#if PX_UART_CFG_UART5_EN
    case PX_UART_NR_5:
        LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_USART5);
        break;
#endif
    default:
        PX_LOG_E("Invalid peripheral");
        return false;
    }

    // Close handle
    handle->uart_per = NULL;

    // Success
    uart_per->open_counter = 0;
    return true;
}

void px_uart_put_char(px_uart_handle_t * handle, char data)
{
    px_uart_per_t * uart_per;

    // Verify that pointer to handle is not NULL
    PX_LOG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    uart_per = handle->uart_per;
    // Check that handle is open
    PX_LOG_ASSERT(uart_per != NULL);
    PX_LOG_ASSERT(uart_per->open_counter != 0);
    PX_LOG_ASSERT(uart_per->usart_base_adr != NULL);

    // Wait until transmit buffer has space for one byte and add it
    while(!px_ring_buf_wr_u8(&uart_per->tx_ring_buf, (uint8_t)data))
    {
        ;
    }

    // Make sure transmit process is started by enabling interrupt
    px_uart_start_tx(uart_per->usart_base_adr);
}

bool px_uart_wr_u8(px_uart_handle_t * handle, uint8_t data)
{
    px_uart_per_t * uart_per;

    // Verify that pointer to handle is not NULL
    PX_LOG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    uart_per = handle->uart_per;
    // Check that handle is open
    PX_LOG_ASSERT(uart_per != NULL);
    PX_LOG_ASSERT(uart_per->open_counter != 0);
    PX_LOG_ASSERT(uart_per->usart_base_adr != NULL);

    // Add byte to transmit buffer
    if(!px_ring_buf_wr_u8(&uart_per->tx_ring_buf, (uint8_t)data))
    {
        // Could not buffer byte for transmission
        return false;
    }

    // Make sure transmit process is started by enabling interrupt
    px_uart_start_tx(uart_per->usart_base_adr);

    // Success
    return true;
}

size_t px_uart_wr(px_uart_handle_t * handle,
                  const void *       data,
                  size_t             nr_of_bytes)
{
    px_uart_per_t * uart_per;
    uint8_t  *      data_u8 = (uint8_t *)data;
    size_t          bytes_buffered = 0;

    // Verify that pointer to handle is not NULL
    PX_LOG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    uart_per = handle->uart_per;
    // Check that handle is open
    PX_LOG_ASSERT(uart_per != NULL);
    PX_LOG_ASSERT(uart_per->open_counter != 0);
    PX_LOG_ASSERT(uart_per->usart_base_adr != NULL);

    // Add bytes to transmit buffer
    bytes_buffered = px_ring_buf_wr(&uart_per->tx_ring_buf,
                                    data_u8,
                                    nr_of_bytes);

    if(bytes_buffered > 0)
    {
        // Make sure transmit process is started by enabling interrupt
        px_uart_start_tx(uart_per->usart_base_adr);
    }

    // Indicate number of bytes buffered
    return bytes_buffered;
}

char px_uart_get_char(px_uart_handle_t * handle)
{
    px_uart_per_t * uart_per;
    uint8_t         data;

    // Verify that pointer to handle is not NULL
    PX_LOG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    uart_per = handle->uart_per;
    // Check that handle is open
    PX_LOG_ASSERT(uart_per != NULL);
    PX_LOG_ASSERT(uart_per->open_counter != 0);
    PX_LOG_ASSERT(uart_per->usart_base_adr != NULL);

    // Wait until a byte is in receive buffer and fetch it
    while(!px_ring_buf_rd_u8(&uart_per->rx_ring_buf, &data))
    {
        ;
    }

    return (char)data;
}

bool px_uart_rd_u8(px_uart_handle_t * handle, uint8_t * data)
{
    px_uart_per_t * uart_per;

    // Verify that pointer to handle is not NULL
    PX_LOG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    uart_per = handle->uart_per;
    // Check that handle is open
    PX_LOG_ASSERT(uart_per != NULL);
    PX_LOG_ASSERT(uart_per->open_counter != 0);
    PX_LOG_ASSERT(uart_per->usart_base_adr != NULL);

    // Return byte from receive buffer (if it is available)
    return px_ring_buf_rd_u8(&uart_per->rx_ring_buf, data);
}

size_t px_uart_rd(px_uart_handle_t * handle,
                  void *             buffer,
                  size_t             nr_of_bytes)
{
    px_uart_per_t * uart_per;

    // Verify that pointer to handle is not NULL
    PX_LOG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    uart_per = handle->uart_per;
    // Check that handle is open
    PX_LOG_ASSERT(uart_per != NULL);
    PX_LOG_ASSERT(uart_per->open_counter != 0);
    PX_LOG_ASSERT(uart_per->usart_base_adr != NULL);

    // Fetch data from receive buffer (up to the specified number of bytes)
    return px_ring_buf_rd(&uart_per->rx_ring_buf,
                               buffer,
                               nr_of_bytes);
}

bool px_uart_wr_buf_is_full(px_uart_handle_t * handle)
{
    px_uart_per_t * uart_per;

    // Verify that pointer to handle is not NULL
    PX_LOG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    uart_per = handle->uart_per;
    // Check that handle is open
    PX_LOG_ASSERT(uart_per != NULL);
    PX_LOG_ASSERT(uart_per->open_counter != 0);
    PX_LOG_ASSERT(uart_per->usart_base_adr != NULL);

    return px_ring_buf_is_full(&uart_per->tx_ring_buf);
}

bool px_uart_wr_buf_is_empty(px_uart_handle_t * handle)
{
    px_uart_per_t * uart_per;

    // Verify that pointer to handle is not NULL
    PX_LOG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    uart_per = handle->uart_per;
    // Check that handle is open
    PX_LOG_ASSERT(uart_per != NULL);
    PX_LOG_ASSERT(uart_per->open_counter != 0);
    PX_LOG_ASSERT(uart_per->usart_base_adr != NULL);

    return px_ring_buf_is_empty(&uart_per->tx_ring_buf);
}

bool px_uart_wr_is_done(px_uart_handle_t * handle)
{
    px_uart_per_t * uart_per;

    // Verify that pointer to handle is not NULL
    PX_LOG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    uart_per = handle->uart_per;
    // Check that handle is open
    PX_LOG_ASSERT(uart_per != NULL);
    PX_LOG_ASSERT(uart_per->open_counter != 0);
    PX_LOG_ASSERT(uart_per->usart_base_adr != NULL);

    // Any data to be transmitted in buffer?
    if(!px_ring_buf_is_empty(&uart_per->tx_ring_buf))
    {
        return false;
    }
    // Return transmission done flag
    return uart_per->tx_done;
}

bool px_uart_rd_buf_is_empty(px_uart_handle_t * handle)
{
    px_uart_per_t * uart_per;

    // Verify that pointer to handle is not NULL
    PX_LOG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    uart_per = handle->uart_per;
    // Check that handle is open
    PX_LOG_ASSERT(uart_per != NULL);
    PX_LOG_ASSERT(uart_per->open_counter != 0);
    PX_LOG_ASSERT(uart_per->usart_base_adr != NULL);

    return px_ring_buf_is_empty(&uart_per->rx_ring_buf);
}

bool px_uart_ioctl_change_baud(px_uart_handle_t * handle, uint32_t baud)
{
    px_uart_per_t * uart_per;
    USART_TypeDef * usart_base_adr;
    bool            result;
    bool            enable;

    // Verify that pointer to handle is not NULL
    PX_LOG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    uart_per = handle->uart_per;
    // Check that handle is open
    PX_LOG_ASSERT(uart_per != NULL);
    PX_LOG_ASSERT(uart_per->open_counter != 0);
    PX_LOG_ASSERT(uart_per->usart_base_adr != NULL);
    // Set pointer to peripheral base address
    usart_base_adr = uart_per->usart_base_adr;

    // Is UART enabled?
    if(LL_USART_IsEnabled(usart_base_adr))
    {
        enable = true;
    }
    else
    {
        enable = false;
    }
    // Disable UART
    LL_USART_Disable(usart_base_adr);
    // Set baud rate
    result = px_uart_init_baud(usart_base_adr, uart_per->uart_nr, baud);
    if(enable)
    {
        // Enable UART
        LL_USART_Enable(usart_base_adr);
    }
    return result;
}

bool px_uart_ioctl_change_data_format(px_uart_handle_t *  handle,
                                      px_uart_data_bits_t data_bits,
                                      px_uart_parity_t    parity,
                                      px_uart_stop_bits_t stop_bits)
{
    px_uart_per_t * uart_per;
    USART_TypeDef * usart_base_adr;
    bool            result;
    bool            enable;

    // Verify that pointer to handle is not NULL
    PX_LOG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    uart_per = handle->uart_per;
    // Check that handle is open
    PX_LOG_ASSERT(uart_per != NULL);
    PX_LOG_ASSERT(uart_per->open_counter != 0);
    PX_LOG_ASSERT(uart_per->usart_base_adr != NULL);
    // Set pointer to peripheral base address
    usart_base_adr = uart_per->usart_base_adr;

    // Is UART enabled?
    if(LL_USART_IsEnabled(usart_base_adr))
    {
        enable = true;
    }
    else
    {
        enable = false;
    }
    // Disable UART
    LL_USART_Disable(usart_base_adr);
    // Set data format
    result = px_uart_init_data_format(usart_base_adr, data_bits, parity, stop_bits);
    if(enable)
    {
        // Enable UART
        LL_USART_Enable(usart_base_adr);
    }
    return result;
}
