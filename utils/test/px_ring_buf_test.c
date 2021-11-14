#include <stdio.h>

#include "px_ring_buf.h"

static px_ring_buf_t px_ring_buf;
static uint8_t       px_ring_buf_data[10];
static uint8_t       buf_wr[10];
static uint8_t       buf_rd[10];

static void report_data(uint8_t * data, size_t nr_of_bytes)
{
    while(nr_of_bytes != 0)
    {
        printf("%02X ", *data++);
        nr_of_bytes--;
    }
    putchar('\n');
}

static void px_ring_buf_report(void)
{
    printf("WR=%u\n",    px_ring_buf.idx_wr);
    printf("RD=%u\n",    px_ring_buf.idx_rd);
    printf("Size=%u\n",  px_ring_buf.buf_size);
    printf("Used=%u\n",  px_ring_buf_get_count_used(&px_ring_buf));
    printf("Free=%u\n",  px_ring_buf_get_count_free(&px_ring_buf));
    printf("Buf: ");
    report_data(px_ring_buf.buf, px_ring_buf.buf_size);
    putchar('\n');
}

int main(void)
{
    int i;

    printf("Buf init:\n");
    memset(px_ring_buf_data, 0xcc, sizeof(px_ring_buf_data));
    px_ring_buf_init(&px_ring_buf, px_ring_buf_data, sizeof(px_ring_buf_data));
    px_ring_buf_report();

    printf("Write 6 bytes:\n");
    for(i = 0; i < 10; i++)
    {
        buf_wr[i] = i;
    }
    px_ring_buf_wr(&px_ring_buf, buf_wr, 6);
    px_ring_buf_report();

    printf("Read 6 bytes:\n");
    px_ring_buf_rd(&px_ring_buf, buf_rd, 6);
    px_ring_buf_report();
    printf("RD: "); report_data(buf_rd, 6);

    printf("Write 10 bytes:\n");
    for(i = 0; i < 10; i++)
    {
        buf_wr[i] = 0xA0 | i;
    }
    px_ring_buf_wr(&px_ring_buf, buf_wr, 10);
    px_ring_buf_report();

    printf("Read 6 bytes:\n");
    px_ring_buf_rd(&px_ring_buf, buf_rd, 6);
    px_ring_buf_report();
    printf("RD: "); report_data(buf_rd, 6);

    while(true) {;}
}

