#include "px_defs.h"
#include "px_board.h"
#include "px_sysclk.h"
#include "px_uart.h"
#include "px_uart_stdio.h"
#include "px_log.h"

// Declare log name
PX_LOG_NAME("px_log_test");

// Declare UART handle structure
px_uart_handle_t px_uart_handle;

bool px_log_filter(px_log_level_t level, const char * name)
{
    // Filter all logs with a higher verbosity than DEBUG level
    if(level > PX_LOG_LEVEL_DEBUG)
    {
        return true;
    }
    // Does name start with "px"?
    if(strncmp(name, "px", 2) != 0)
    {
        return true;
    }
    // Allow log
    return false;
}

uint8_t calc(uint8_t val)
{
    uint8_t answer;

    // Report function call with parameter value
    PX_LOG_D("calc(" PX_LOG_BG_YELLOW "val = %u" PX_LOG_CR ")", val);
    // Is val equal to zero?
    if(val == 0)
    {
        PX_LOG_E("val may not be equal to zero");
        return 0;
    }
    // Calculate answer
    answer = 100 / val;
    // Report answer
    PX_LOG_I("answer = " PX_LOG_FG_GREEN "%u" PX_LOG_CR, answer);

    return answer;
}

int main(void)
{
    uint8_t val;
    uint8_t answer;
 
    // Initialise modules
    px_board_init();
    px_sysclk_init();
    px_uart_init();
    // Open UART1 @ 115200 BAUD, 8 data bits, no parity, 1 stop bit
    px_uart_open2(&px_uart_handle,
                  PX_UART_NR_1, 
                  115200, 
                  PX_UART_DATA_BITS_8, 
                  PX_UART_PARITY_NONE, 
                  PX_UART_STOP_BITS_1);
    // Direct stdio to UART0
    px_uart_stdio_init(&px_uart_handle);
    // Enable interrupts
    px_interrupts_enable();
   
    PX_LOG_V("Calculation started");
    for(val = 0; val < 3; val++)
    {
        // Calculate answer
        answer = calc(val);
    }
    PX_LOG_TRACE("Calculation finished. answer = %u\n", answer);
}

void main_log_timestamp(char * str)
{
    uint8_t i;
    uint8_t j;
    char    timestamp[9];

    sprintf(timestamp, "%08lu", (uint32_t)px_sysclk_get_tick_count());

    i = 0;
    j = 0;
    while(i < 5)
    {
        str[i++] = timestamp[j++];
    }
    // Insert decimal separator
    str[i++] = '.';
    while(i < 9)
    {
        str[i++] = timestamp[j++];
    }
    str[i++] = '\0';
}
