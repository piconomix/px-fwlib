#include <stdio.h>
#include <string.h>

#include "px_buf_strings.h"

static px_buf_strings_t px_buf_strings;
static char             px_buf_strings_data[16];

int main(void)
{
    const char * str;

    px_buf_strings_init(&px_buf_strings, px_buf_strings_data, sizeof(px_buf_strings_data));
    printf("'%s' count=%u available=%u\n", px_buf_strings.str, px_buf_strings_get_count(&px_buf_strings), px_buf_strings_get_size_remaining(&px_buf_strings));

    px_buf_strings_append_char(&px_buf_strings, 'h');
    px_buf_strings_append_char(&px_buf_strings, 'e');
    px_buf_strings_append_char(&px_buf_strings, 'l');
    px_buf_strings_append_char(&px_buf_strings, 'l');
    px_buf_strings_append_char(&px_buf_strings, 'o');
    printf("'%s' count=%u available=%u\n", px_buf_strings.str, px_buf_strings_get_count(&px_buf_strings), px_buf_strings_get_size_remaining(&px_buf_strings));
    px_buf_strings_append_char(&px_buf_strings, '\0');

    px_buf_strings_append_str(&px_buf_strings, "world");
    printf("'%s' count=%u available=%u\n", px_buf_strings.str, px_buf_strings_get_count(&px_buf_strings), px_buf_strings_get_size_remaining(&px_buf_strings));
    px_buf_strings_append_char(&px_buf_strings, '!');
    printf("'%s' count=%u available=%u\n", px_buf_strings.str, px_buf_strings_get_count(&px_buf_strings), px_buf_strings_get_size_remaining(&px_buf_strings));

    px_buf_strings_append_str(&px_buf_strings, "1234567890");
    printf("'%s' count=%u available=%u\n", px_buf_strings.str, px_buf_strings_get_count(&px_buf_strings), px_buf_strings_get_size_remaining(&px_buf_strings));

    str = px_buf_strings_get_first(&px_buf_strings);
    while(str != NULL)
    {
        printf("%s\n", str);
        str = px_buf_strings_get_next(&px_buf_strings);
    }

    while(true) {;}
}
