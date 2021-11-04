/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          px_usb_cdc_stdio.h : Module to redirect stdio output (e.g. printf) to a USB
    Author(s):      Pieter Conradie
    Creation Date:  2018-02-06

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <string.h>
#include "px_compiler.h"

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_usb_cdc_stdio.h"
#include "px_ring_buf.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
#define PX_USB_CDC_STDIO_TX_BUF_SIZE   1024
#define PX_USB_CDC_STDIO_RX_BUF_SIZE   1024

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
#ifdef PX_COMPILER_ARM_CC
#pragma import(__use_no_semihosting_swi)
struct __FILE
{ 
    int handle;
};

FILE __stdout;
FILE __stdin;
#endif

// Transmit circular buffer
static px_ring_buf_t px_usb_cdc_stdio_tx_buffer;
static uint8_t       px_usb_cdc_stdio_tx_buffer_data[PX_USB_CDC_STDIO_TX_BUF_SIZE];

// Receive circular buffer
static px_ring_buf_t px_usb_cdc_stdio_rx_buffer;
static uint8_t       px_usb_cdc_stdio_rx_buffer_data[PX_USB_CDC_STDIO_RX_BUF_SIZE];

/* _____LOCAL FUNCTIONS______________________________________________________ */
#ifdef PX_COMPILER_GCC_ARM
int _write(int file, char * ptr, int len)
{
    int i;

    for(i = 0; i < len; i++)
    {
        px_usb_cdc_stdio_putchar(*ptr++);
    }
    return len;
}

int _read(int file, char * ptr, int len)
{
    int i;

    for(i = 0; i < len; i++)
    {
        *ptr++ = px_usb_cdc_stdio_getchar();
    }
    return len;  
}
#endif

#ifdef PX_COMPILER_ARM_CC
int fputc(int ch, FILE * f)
{
    px_usb_cdc_stdio_putchar(ch);

    return 0;
}

int fgetc(FILE * f)
{
    return px_usb_cdc_stdio_getchar();
}

void _sys_exit(int return_code)
{
    while(1);
}
#endif

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_usb_cdc_stdio_init(void)
{
    // Initialise transmit circular buffer
    px_ring_buf_init(&px_usb_cdc_stdio_tx_buffer, 
                     px_usb_cdc_stdio_tx_buffer_data, 
                     PX_USB_CDC_STDIO_TX_BUF_SIZE);
    // Initialise receive circular buffer
    px_ring_buf_init(&px_usb_cdc_stdio_rx_buffer, 
                     px_usb_cdc_stdio_rx_buffer_data, 
                     PX_USB_CDC_STDIO_RX_BUF_SIZE);

#ifdef PX_COMPILER_GCC_ARM
    // Disable IO buffering in libc to avoid putchar() bug. See:
    // https://bugs.launchpad.net/gcc-arm-embedded/+bug/1380268
    setvbuf(stdout, NULL, _IONBF, 0);
#endif
}

int px_usb_cdc_stdio_putchar(char data)
{
    // Newline character?
    if(data == '\n')
    {
        // Prepend cariage return
        px_usb_cdc_stdio_putchar('\r');
    }
    // Buffer transmit byte (wait until circular buffer accepts byte)
    while(!px_ring_buf_wr_u8(&px_usb_cdc_stdio_tx_buffer, (uint8_t)data))
    {
        ;
    }

    return 0;
}

int px_usb_cdc_stdio_getchar(void)
{
    uint8_t data;

    // Wait until a byte has been received
    while(!px_ring_buf_rd_u8(&px_usb_cdc_stdio_rx_buffer, &data))
    {
        ;
    }

    return (int)data;
}

bool px_usb_cdc_stdio_rd_u8(uint8_t * data)
{
    return px_ring_buf_rd_u8(&px_usb_cdc_stdio_rx_buffer, data);
}

bool px_usb_cdc_stdio_wr_u8(uint8_t data)
{
    return px_ring_buf_wr_u8(&px_usb_cdc_stdio_tx_buffer, data);
}

void _px_usb_cdc_stdio_on_rx_byte(uint8_t data)
{
    px_ring_buf_wr_u8(&px_usb_cdc_stdio_rx_buffer, data);
}

void _px_usb_cdc_stdio_on_rx_data(const uint8_t * data, uint16_t nr_of_bytes)
{
    px_ring_buf_wr(&px_usb_cdc_stdio_rx_buffer, data, nr_of_bytes);
}

bool _px_usb_cdc_stdio_get_tx_byte(uint8_t * data)
{
    return px_ring_buf_rd_u8(&px_usb_cdc_stdio_tx_buffer, data);
}

uint16_t _px_usb_cdc_stdio_get_tx_data(uint8_t * data, uint16_t nr_of_bytes)
{
    return px_ring_buf_rd(&px_usb_cdc_stdio_tx_buffer, data, nr_of_bytes);
}

