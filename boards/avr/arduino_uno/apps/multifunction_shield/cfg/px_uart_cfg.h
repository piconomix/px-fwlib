#ifndef __PX_UART_CFG_H__
#define __PX_UART_CFG_H__
/*
 * UART configuration for the multifunction shield example.
 * Arduino UNO hardware UART: D0/RX and D1/TX, 115200 8-N-1.
 */
#define PX_UART_CFG_UART0_EN          1
#define PX_UART_CFG_UART1_EN          0
#define PX_UART_CFG_DEFAULT_BAUD      115200ul
#define PX_UART_CFG_DEFAULT_UCSRC     ((1 << UCSZ01) | (1 << UCSZ00))
#define PX_UART_CFG_TX_BUF_SIZE       32
#define PX_UART_CFG_RX_BUF_SIZE       32
#define PX_UART_CFG_USE_2X_BAUD_RATE  1

#endif
