#include <stdio.h>
#include <string.h>

#include "px_sbuf.h"

int main(void)
{
    px_sbuf_t sbuf;
    char      buf[16];

    // Test 1. Fill buffer with magic values to detect buffer overflow
    memset(buf, 0xcc, sizeof(buf));
    // Initialise safe buffer
    px_sbuf_init(&sbuf, buf, 8);
    // Append strings
    px_sbuf_printf(&sbuf, "01234");
    px_sbuf_printf(&sbuf, "56789");
    // Report result
    printf("%s\n", sbuf.buf);

    // Test 2. Fill buffer with magic values to detect buffer overflow
    memset(buf, 0xcc, sizeof(buf));
    // Reset safe buffer
    px_sbuf_reset(&sbuf);
    // Append cahracters
    for(int i = 0; i < 16; i++)
    {
        px_sbuf_putchar(&sbuf, i + '0');
    }
    // Report result
    printf("%s\n", sbuf.buf);

    // Test 3. Fill buffer with magic values to detect buffer overflow
    memset(buf, 0xcc, sizeof(buf));
    // Reset safe buffer
    px_sbuf_reset(&sbuf);
    // Append strings
    px_sbuf_strcpy(&sbuf, "01234");
    px_sbuf_strcpy(&sbuf, "56789");
    // Report result
    printf("%s\n", sbuf.buf);

    for(;;)
    {
    }
}
