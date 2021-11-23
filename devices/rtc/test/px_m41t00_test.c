#include "px_defs.h"
#include "px_uart.h"
#include "px_uart_stdio.h"
#include "px_pgm_P.h"
#include "px_i2c.h"
#include "px_m41t00.h"

// Declare I2C handle structure
px_i2c_handle_t  px_i2c_handle;

// Declare UART handle structure
px_uart_handle_t px_uart_handle;

px_m41t00_time_t time = 
{
    .sec          = 0,
    .min          = 0,
    .hour         = 12,
    .day_of_week  = 1,
    .day_of_month = 1,
    .month        = 4,
    .year         = 7
};

void px_m41t00_test(void)
{
    uint8_t sec;

    // Initialise modules
    px_i2c_init();
    px_uart_init();

    // Open I2C Slave handle
    px_i2c_open(&px_i2c_handle, PX_I2C_NR_0, PX_M41T00_SLA_ADR);
    px_m41t00_init(&px_i2c_handle);

    // Open UART0 @ 115200 BAUD, 8 data bits, no parity, 1 stop bit
    px_uart_open2(&px_uart_handle,
                  PX_UART_NR_0, 
                  115200, 
                  PX_UART_DATA_BITS_8, 
                  PX_UART_PARITY_NONE, 
                  PX_UART_STOP_BITS_1);

    // Direct stdio to UART0
    px_uart_stdio_init(&px_uart_handle);

    // Enable interrupts
    px_interrupts_enable();

    // Set time and date
    if(!px_m41t00_set_time(&time))
    {
        PX_PRINTF_P("Unable to set time!\n");
        while(true) {;}
    }
  
    // Loop forever
    while(true)
    {
        // Remember current seconds value
        sec = time.sec;
        // Fetch new time
        if(!px_m41t00_get_time(&time))
        {
            PX_PRINTF_P("Unable to set time!\n");
            while(true) {;}
        }
        // See if a second has elapsed
        if(time.sec != sec)
        {
            // Display the new date and time
            PX_PRINTF_P("%02d-%02d-%02d %02dh%02d:%02d\n",
                        time.year,
                        time.month,
                        time.day_of_month,
                        time.hour,
                        time.min,
                        time.sec);
        }
    }    
}
