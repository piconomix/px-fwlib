#include <avr/interrupt.h>

#include "px_hdlc.h"
#include "printf.h"
#include "uart0.h"
#include "uart1.h"

static void rs485_tx_u8(uint8_t data)
{
    while(!uart1_tx_byte()data))
    {
        ;
    }
}

void px_hdlc_on_rx_frame(uint8_t * data, size_t nr_of_bytes)
{
    PX_PRINTF_P("\nHDLC RX [%d]: ", nr_of_bytes);
    while(nr_of_bytes != 0)
    {
        PX_PRINTF_P("%02X ", *data++);
        nr_of_bytes--;
    }
    putchar('\n');
}

void px_hdlc_test(void)
{
    uint8_t data;

    // Enable RS-485 GPIO for loopback operation
    PX_BIT_SET_HI(PORT_RS485_TX_EN_O, BIT_RS485_TX_EN_O);
    PX_BIT_SET_HI(DDR_RS485_TX_EN_O, BIT_RS485_TX_EN_O);

    PX_BIT_SET_LO(PORT_RS485_RX_EN_O, BIT_RS485_RX_EN_O);
    PX_BIT_SET_HI(DDR_RS485_RX_EN_O, BIT_RS485_RX_EN_O);

    // Initialise modules
    uart0_init();
    uart1_init();
    printf_init();
    px_hdlc_init(&rs485_tx_u8, &px_hdlc_on_rx_frame);

    // Enable global interrupts
    sei();
        
    // Send an HDLC packet
    data = 0x55;
    px_hdlc_tx_frame(&data, 1);

    // Process received data
    while(true)
    {
        if(uart1_get_rx_byte(&data))
        {
            PX_PRINTF_P("%02X ", data);
            // Feed received data to HDLC layer
            px_hdlc_on_rx_u8(data);
        }
    }
}
