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

/* _____STANDARD INCLUDES____________________________________________________ */
#include <avr/interrupt.h>
#include <stdlib.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_uart.h"
#include "px_uart_defs.h"
#include "px_ring_buf_macros.h"
#include "px_board.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("uart");

/// Definition of data for each UART peripheral
typedef struct px_uart_per_s
{ 
    /// Peripheral
    px_uart_nr_t uart_nr;
    /// Number of open handles referencing peripheral
    uint8_t open_counter;
    /// Transmit done flag
    volatile bool tx_done;
    /// Transmit circular buffer
    PX_RING_BUF_DECLARE(tx_circ_buf, PX_UART_CFG_TX_BUF_SIZE);
    /// Receive circular buffer
    PX_RING_BUF_DECLARE(rx_circ_buf, PX_UART_CFG_RX_BUF_SIZE);
} px_uart_per_t;

/* _____MACROS_______________________________________________________________ */
/// Macro to convert the specified BAUD rate to a 16-bit UBRR register value
#define PX_UART_CALC_UBRR(baud) \
    (PX_UDIV_ROUND(F_CPU, baud * (((PX_UART_CFG_USE_2X_BAUD_RATE == 0)? 16ul : 8ul))) - 1)

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
/// Allocate data for each en1bled UART peripheral
#if PX_UART_CFG_UART0_EN
static px_uart_per_t px_uart_per_0;
#endif

#if PX_UART_CFG_UART1_EN
static px_uart_per_t px_uart_per_1;
#endif

/* _____LOCAL FUNCTIONS______________________________________________________ */
#if PX_UART_CFG_UART0_EN
/// Received data interrupt handler
ISR(USART0_RX_vect)
{
    uint8_t ucsra = UCSR0A;
    uint8_t data  = UDR0;

    // Accept data only if there were no Framing or Parity Error(s)
    if(ucsra & ((1 << FE0) | (1 << UPE0)))
    {
        // Received data had an error, discard received data
        return;
    }
    // Make sure there is space available in buffer.
    if(!PX_RING_BUF_FULL(px_uart_per_0.rx_circ_buf))
    {
        // Add data to ring buffer
        PX_RING_BUF_WRITE(px_uart_per_0.rx_circ_buf, data);
    }
}

/// Transmit data register empty interrupt handler
ISR(USART0_UDRE_vect)
{
    uint8_t data;

    // Finished sending data?
    if(PX_RING_BUF_EMPTY(px_uart_per_0.tx_circ_buf))
    {
        // Disable transmit data register empty interrupt
        PX_BIT_SET_LO(UCSR0B, UDRIE0);
        // Enable transmit complete interrupt
        PX_BIT_SET_HI(UCSR0B, TXCIE0);
        return;
    }
    // Send data
    PX_RING_BUF_READ(px_uart_per_0.tx_circ_buf, data);
    UDR0 = data;
    // Clear flag to indicate that transmission is busy
    px_uart_per_0.tx_done = false;
}

/// Transmit complete interrupt handler
ISR(USART0_TX_vect)
{
    // Set flag to indicate that transmission has finished
    px_uart_per_0.tx_done = true;
    // Disable interrupt
    PX_BIT_SET_LO(UCSR0B, TXCIE0);
}
#endif // PX_UART_CFG_UART0_EN

#if PX_UART_CFG_UART1_EN
/// Received data interrupt handler
ISR(USART1_RX_vect)
{
    uint8_t ucsra = UCSR1A;
    uint8_t data  = UDR1;

    // Accept data only if there were no Framing or Parity Error(s)
    if(ucsra & ((1 << FE1) | (1 << UPE1)))
    {
        // Received data had an error, discard received data
        return;
    }
    // Make sure there is space available in buffer.
    if(!PX_RING_BUF_FULL(px_uart_per_1.rx_circ_buf))
    {
        // Add data to ring buffer
        PX_RING_BUF_WRITE(px_uart_per_1.rx_circ_buf, data);
    }
}

/// Transmit data register empty interrupt handler
ISR(USART1_UDRE_vect)
{
    uint8_t data;

    // Finished sending data?
    if(PX_RING_BUF_EMPTY(px_uart_per_1.tx_circ_buf))
    {
        // Disable transmit data register empty interrupt
        PX_BIT_SET_LO(UCSR1B, UDRIE1);
        // Enable transmit complete interrupt
        PX_BIT_SET_HI(UCSR1B, TXCIE1);
        return;
    }
    // Send data
    PX_RING_BUF_READ(px_uart_per_1.tx_circ_buf, data);
    UDR1 = data;
    // Clear flag to indicate that transmission is busy
    px_uart_per_1.tx_done = false;
}

/// Transmit complete interrupt handler
ISR(USART1_TX_vect)
{
    // Set flag to indicate that transmission has finished
    px_uart_per_1.tx_done = true;
    // Disable interrupt
    PX_BIT_SET_LO(UCSR1B, TXCIE1);
}
#endif

static void px_uart_start_tx(px_uart_nr_t peripheral)
{
    switch(peripheral)
    {
#if PX_UART_CFG_UART0_EN
    case PX_UART_NR_0: 
        PX_BIT_SET_HI(UCSR0B, UDRIE0); 
        break;
#endif
#if PX_UART_CFG_UART1_EN
    case PX_UART_NR_1:
        PX_BIT_SET_HI(UCSR1B, UDRIE1);
        break;
#endif
    default:
        break;
    }
}

static uint16_t px_uart_calc_ubrr(uint32_t baud)
{
    uint16_t ubrr;
    ldiv_t   div;

    // Calculate new 16-bit UBRR register value
#if (PX_UART_CFG_USE_2X_BAUD_RATE == 1)
    // Divide by 8
    baud <<= 3;
#else
    // Divide by 16
    baud <<= 4;
#endif
    div    = ldiv(F_CPU, baud);
    ubrr   = (uint16_t)div.quot;
    // Must UBBR be rounded?
    baud >>= 1;
    if((uint32_t)(div.rem) < baud)
    {
        // Yes
        ubrr--;
    }

    return ubrr;
}

static bool px_uart_init_peripheral(px_uart_nr_t uart_nr,
                                    uint16_t     ubrr,
                                    uint8_t      ucsrc)
{
    switch(uart_nr)
    {
#if PX_UART_CFG_UART0_EN
    case 0:
        // Disable UART
        UCSR0B = 0;
#if (PX_UART_CFG_USE_2X_BAUD_RATE == 1)
        // Double the USART transmission speed
        UCSR0A = (1 << U2X0);
#else
        UCSR0A = 0;
#endif
        // Set BAUD rate
        UBRR0H = PX_U16_HI8(ubrr);
        UBRR0L = PX_U16_LO8(ubrr);
        // Configure data bits, parity and stop bits
        UCSR0C = ucsrc;
        // Enable RxD/TxD and receive interupt
        UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);
        break;
#endif

#if PX_UART_CFG_UART1_EN
    case 1:
        // Disable UART
        UCSR1B = 0;
#if (PX_UART_CFG_USE_2X_BAUD_RATE == 1)
        // Double the USART transmission speed
        UCSR1A = (1 << U2X1);
#else
        UCSR1A = 0;
#endif
        // Set BAUD rate
        UBRR1H = PX_U16_HI8(ubrr);
        UBRR1L = PX_U16_LO8(ubrr);
        // Configure data bits, parity and stop bits
        UCSR1C = ucsrc;
        // Enable RxD/TxD and receive interupt
        UCSR1B = (1 << RXCIE1) | (1 << RXEN1) | (1 << TXEN1);
        break;
#endif

    default:
        // Invalid peripheral
        PX_LOG_E("Invalid peripheral");
        return false;
    }

    // Success
    return true;
}

static void px_uart_init_peripheral_data(px_uart_nr_t    uart_nr,
                                         px_uart_per_t * uart_per)
{
    // Set peripheral
    uart_per->uart_nr = uart_nr;
    // Clear reference counter
    uart_per->open_counter = 0;
    // Set transmit done flag
    uart_per->tx_done = true;
    // Initialise transmit circular buffer
    PX_RING_BUF_INIT(uart_per->tx_circ_buf);
    // Initialise receive circular buffer
    PX_RING_BUF_INIT(uart_per->rx_circ_buf);
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_uart_init(void)
{
    // Initialize peripheral data for each enabled peripheral
#if PX_UART_CFG_UART0_EN
    px_uart_init_peripheral_data(PX_UART_NR_0, &px_uart_per_0);
#endif
#if PX_UART_CFG_UART1_EN
    px_uart_init_peripheral_data(PX_UART_NR_1, &px_uart_per_1);
#endif
}

bool px_uart_open(px_uart_handle_t * handle, 
                  px_uart_nr_t       uart_nr)
{
    px_uart_per_t * uart_per;

    // Verify that pointer to handle is not NULL
    PX_LOG_ASSERT(handle != NULL);

    // Handle not initialised
    handle->uart_per = NULL;
    // Set pointer to peripheral data
    switch(uart_nr)
    {
#if PX_UART_CFG_UART0_EN
    case PX_UART_NR_0:
        uart_per = &px_uart_per_0;
        break;
#endif
#if PX_UART_CFG_UART1_EN
    case PX_UART_NR_1:
        uart_per = &px_uart_per_1;
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
    // Initialise peripheral
    if(!px_uart_init_peripheral(uart_nr,
                                PX_UART_CALC_UBRR(PX_UART_CFG_DEFAULT_BAUD),
                                PX_UART_CFG_DEFAULT_UCSRC))
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

bool px_uart_open2(px_uart_handle_t *  handle, 
                   px_uart_nr_t        uart_nr,
                   uint32_t            baud, 
                   px_uart_data_bits_t data_bits, 
                   px_uart_parity_t    parity, 
                   px_uart_stop_bits_t stop_bits)
{
    px_uart_per_t * uart_per;
    uint16_t        ubrr;
    uint8_t         ucsrc = 0x00;

    // Verify that pointer to handle is not NULL
    PX_LOG_ASSERT(handle != NULL);

    // Handle not initialised
    handle->uart_per = NULL;
    // Set pointer to peripheral data
    switch(uart_nr)
    {
#if PX_UART_CFG_UART0_EN
    case PX_UART_NR_0: uart_per = &px_uart_per_0; break;
#endif
#if PX_UART_CFG_UART1_EN
    case PX_UART_NR_1: uart_per = &px_uart_per_1; break;
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
    // Calculate new 16-bit UBRR register value
    ubrr = px_uart_calc_ubrr(baud);
    // Set parity
    switch(parity)
    {
    case PX_UART_PARITY_ODD:  ucsrc |= (1 << UPM01) | (1 << UPM00); break;
    case PX_UART_PARITY_EVEN: ucsrc |= (1 << UPM01) | (0 << UPM00); break;
    case PX_UART_PARITY_NONE: ucsrc |= (0 << UPM01) | (0 << UPM00); break;
    default:
        PX_LOG_E("Invalid parity specified");
        handle->uart_per = NULL;
        return false;
    }
    // Set data bits
    switch(data_bits)
    {
    case PX_UART_DATA_BITS_5: ucsrc |= (0 << UCSZ01) | (0 << UCSZ00); break;
    case PX_UART_DATA_BITS_6: ucsrc |= (0 << UCSZ01) | (1 << UCSZ00); break;
    case PX_UART_DATA_BITS_7: ucsrc |= (1 << UCSZ01) | (0 << UCSZ00); break;
    case PX_UART_DATA_BITS_8: ucsrc |= (1 << UCSZ01) | (1 << UCSZ00); break;
    default:
        PX_LOG_E("Invalid number of data bits");
        handle->uart_per = NULL;
        return false;
    }
    // Set stop bits
    switch(stop_bits)
    {
    case PX_UART_STOP_BITS_1: ucsrc |= (0 << USBS0); break;
    case PX_UART_STOP_BITS_2: ucsrc |= (1 << USBS0); break;
    default:
        PX_LOG_E("Invalid number of stop bits specified");
        handle->uart_per = NULL;
        return false;
    }
    // Initialise peripheral
    if(!px_uart_init_peripheral(uart_nr, ubrr, ucsrc))
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

    // Disable UART
    switch(uart_per->uart_nr)
    {
#if PX_UART_CFG_UART0_EN
    case 0: UCSR0B = 0; break;
#endif
#if PX_UART_CFG_UART1_EN
    case 1: UCSR1B = 0; break;
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

    // Wait until transmit buffer has space for one byte
    while(PX_RING_BUF_FULL(uart_per->tx_circ_buf)) {;}
    // Buffer the byte to be transmitted
    PX_RING_BUF_WRITE(uart_per->tx_circ_buf, (uint8_t)data);
    // Make sure transmit process is started by enabling interrupt
    px_uart_start_tx(uart_per->uart_nr);
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

    if(PX_RING_BUF_FULL(uart_per->tx_circ_buf))
    {
        return false;
    }
    PX_RING_BUF_WRITE(uart_per->tx_circ_buf, data);

    // Make sure transmit process is started by enabling interrupt
    px_uart_start_tx(uart_per->uart_nr);

    return true;
}

size_t px_uart_wr(px_uart_handle_t * handle, const void* data, size_t nr_of_bytes)
{
    px_uart_per_t * uart_per;
    uint8_t *       data_u8 = (uint8_t *)data;
    size_t          bytes_buffered = 0;    

    // Verify that pointer to handle is not NULL
    PX_LOG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    uart_per = handle->uart_per;
    // Check that handle is open
    PX_LOG_ASSERT(uart_per != NULL);
    PX_LOG_ASSERT(uart_per->open_counter != 0);

    while(nr_of_bytes)
    {
        // Make sure there is space available in buffer
        if(PX_RING_BUF_FULL(uart_per->tx_circ_buf))
        {
            break;
        }
        // Insert data into buffer
        PX_RING_BUF_WRITE(uart_per->tx_circ_buf, *data_u8++);
        // Next byte
        bytes_buffered++;
        nr_of_bytes--;
    }

    // Make sure transmit process is started by enabling interrupt
    px_uart_start_tx(uart_per->uart_nr);

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

    // Wait until at least one byte has been received
    while(PX_RING_BUF_EMPTY(uart_per->rx_circ_buf)) {;}
    // Fetch received byte
    PX_RING_BUF_READ(uart_per->rx_circ_buf, data);

    return (char)data;
}

bool px_uart_rd_u8(px_uart_handle_t * handle, uint8_t* data)
{
    px_uart_per_t * uart_per;

    // Verify that pointer to handle is not NULL
    PX_LOG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    uart_per = handle->uart_per;
    // Check that handle is open
    PX_LOG_ASSERT(uart_per != NULL);
    PX_LOG_ASSERT(uart_per->open_counter != 0);

    if(PX_RING_BUF_EMPTY(uart_per->rx_circ_buf))
    {
        return false;
    }
    else
    {
        PX_RING_BUF_READ(uart_per->rx_circ_buf, *data);
        return true;
    }
}

size_t px_uart_rd(px_uart_handle_t * handle, void* buffer, size_t nr_of_bytes)
{
    px_uart_per_t * uart_per;
    uint8_t  *      buffer_u8 = (uint8_t *)buffer;
    size_t          data_received = 0;

    // Verify that pointer to handle is not NULL
    PX_LOG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    uart_per = handle->uart_per;
    // Check that handle is open
    PX_LOG_ASSERT(uart_per != NULL);
    PX_LOG_ASSERT(uart_per->open_counter != 0);

    while(nr_of_bytes)
    {
        // See if there is data in the buffer
        if(PX_RING_BUF_EMPTY(uart_per->rx_circ_buf))
        {
            break;
        }
        // Fetch byte
        PX_RING_BUF_READ(uart_per->rx_circ_buf, *buffer_u8++);
        // Next byte
        data_received++;
        nr_of_bytes--;
    }

    return data_received;
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

    if(PX_RING_BUF_FULL(uart_per->tx_circ_buf))
    {
        return true;
    }
    else
    {
        return false;
    }
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

    if(PX_RING_BUF_EMPTY(uart_per->tx_circ_buf))
    {
        return true;
    }
    else
    {
        return false;
    }
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

    if(!(PX_RING_BUF_EMPTY(uart_per->tx_circ_buf)))
    {
        return false;
    }
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

    if(PX_RING_BUF_EMPTY(uart_per->rx_circ_buf))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void px_uart_ioctl_change_baud(px_uart_handle_t * handle, uint32_t baud)
{
    px_uart_per_t * uart_per;
    uint16_t        ubrr;
    uint8_t         ucsrc;

    // Verify that pointer to handle is not NULL
    PX_LOG_ASSERT(handle != NULL);
    // Set pointer to peripheral
    uart_per = handle->uart_per;
    // Check that handle is open
    PX_LOG_ASSERT(uart_per != NULL);
    PX_LOG_ASSERT(uart_per->open_counter != 0);

    // Calculate new 16-bit UBRR register value
    ubrr = px_uart_calc_ubrr(baud);
    // Get current UCSRxC register value (data bits, parity & stop bits)
    switch(uart_per->uart_nr)
    {
#if PX_UART_CFG_UART0_EN
    case 0: ucsrc = UCSR0C; break;
#endif
#if PX_UART_CFG_UART1_EN
    case 1: ucsrc = UCSR1C; break;
#endif
    default:
        PX_LOG_E("Invalid peripheral");
        return;
    }
    // Initialise peripheral with new BAUD rate
    px_uart_init_peripheral(uart_per->uart_nr, ubrr, ucsrc);
}
