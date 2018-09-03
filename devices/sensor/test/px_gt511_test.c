#include <stdio.h>

#include "px_board.h"
#include "px_sysclk.h"
#include "px_systmr.h"
#include "px_uart.h"
#include "px_uart_stdio.h"

#include "px_gt511.h"

static px_uart_handle_t    px_uart1_handle;
static px_uart_handle_t    px_uart2_handle;
px_sysclk_ticks_t          px_sysclk_timestamp;

static px_gt511_err_t      err;
static px_gt511_dev_info_t dev_info;
static px_gt511_id_t       enroll_count;
static px_gt511_id_t       id;

void main_report_err(px_gt511_err_t err)
{
    switch(err)
    {
    case PX_GT511_ERR_NONE:                  printf("No error");                                                    break;
    case PX_GT511_ERR_TIMEOUT:               printf("Error! Timeout");                                              break;
    case PX_GT511_ERR_INVALID_BAUD:          printf("Error! Invalid serial baud rate");                             break;
    case PX_GT511_ERR_INVALID_POS:           printf("Error! The specified ID is not in range");                     break;
    case PX_GT511_ERR_IS_NOT_USED:           printf("Error! The specified ID is not used");                         break;
    case PX_GT511_ERR_IS_ALREADY_USED:       printf("Error! The specified ID is already used");                     break;
    case PX_GT511_ERR_COMM_ERR:              printf("Error! Communication Error");                                  break;
    case PX_GT511_ERR_VERIFY_FAILED:         printf("Error! 1:1 Verification Failure");                             break;
    case PX_GT511_ERR_IDENTIFY_FAILED:       printf("Error! 1:N Identification Failure");                           break;
    case PX_GT511_ERR_DB_IS_FULL:            printf("Error! The database is full");                                 break;
    case PX_GT511_ERR_DB_IS_EMPTY:           printf("Error! The database is empty");                                break;
    case PX_GT511_ERR_TURN_ERR:              printf("Error! Invalid order of the enrollment");                      break;
    case PX_GT511_ERR_BAD_FINGER:            printf("Error! Too bad fingerprint");                                  break;
    case PX_GT511_ERR_ENROLL_FAILED:         printf("Error! Enrollment Failure");                                   break;
    case PX_GT511_ERR_IS_NOT_SUPPORTED:      printf("Error! The specified command is not supported");               break;
    case PX_GT511_ERR_DEV_ERR:               printf("Error! Device Error, especially if Crypto-Chip is trouble");   break;
    case PX_GT511_ERR_CAPTURE_CANCELLED:     printf("Error! The capturing is cancelled");                           break;
    case PX_GT511_ERR_INVALID_PARAM:         printf("Error! Invalid parameter");                                    break;
    case PX_GT511_ERR_FINGER_IS_NOT_PRESSED: printf("Error! Finger is not pressed");                                break;
    default:                                 printf("Unknown Error!");                                              break;
    }
}

void main_timestamp_start(void)
{
    px_sysclk_timestamp = px_sysclk_get_tick_count();
}

void main_timestamp_report(void)
{
    printf("%04lu", (uint32_t)(px_sysclk_get_tick_count() - px_sysclk_timestamp));
}

int main(void)
{
    uint8_t i;

    // Initialise board
    px_board_init();

    // Initialise modules
    px_sysclk_init();
    px_uart_init();

    // Open UART1 @ 115200 BAUD, 8 data bits, no parity, 1 stop bit
    px_uart_open2(&px_uart1_handle,
                  PX_UART_PER_1,
                  115200, 
                  PX_UART_DATA_BITS_8, 
                  PX_UART_PARITY_NONE, 
                  PX_UART_STOP_BITS_1);

    // Initialise stdio stream to use the specified UART
    px_uart_stdio_init(&px_uart1_handle);

    // Open UART2 @ 9600 BAUD, 8 data bits, no parity, 1 stop bit
    px_uart_open2(&px_uart2_handle,
                  PX_UART_PER_2,
                  PX_GT511_UART_DEFAULT_BAUD, 
                  PX_UART_DATA_BITS_8, 
                  PX_UART_PARITY_NONE, 
                  PX_UART_STOP_BITS_1);

    // Initialise GT-511 module driver
    px_gt511_init(&px_uart2_handle);

    // Enable LED
    PX_USR_LED_ON();

    // Show startup message
    printf("\nGT-511 Fingerprint Sensor test\n");

    // Perform auto BAUD procedure
    if((err = px_gt511_auto_baud()) != PX_GT511_ERR_NONE)
    {
        goto error;
    }

    // Send OPEN command and get device info
    if((err = px_gt511_open(true, &dev_info)) != PX_GT511_ERR_NONE)
    {
        goto error;
    }
    printf("Firmware Version: %08lx\n", dev_info.fw_version);
    printf("Device Serial Number: ");
    for(i=0; i < sizeof(dev_info.device_serial_number); i++)
    {
        printf("%02X", dev_info.device_serial_number[i]);
    }
    putchar('\n');
#if 0
    // Change BAUD to 115200
    if((err = px_gt511_change_baud_rate(115200)) != PX_GT511_ERR_NONE)
    {
        goto error;
    }
    px_uart_change_baud(&px_uart2_handle, 115200);
#endif
    // Get enroll count
    if((err = px_gt511_get_enroll_count(&enroll_count)) != PX_GT511_ERR_NONE)
    {
        goto error;
    }
    printf("Enroll count = %u\n", enroll_count);

    // Delete all IDs
    if(enroll_count != 0)
    {
        printf("Deleting all IDs... ");
        main_timestamp_start();
        if((err = px_gt511_delete_all()) != PX_GT511_ERR_NONE)
        {
            goto error;
        }
        printf("done (");
        main_timestamp_report();
        printf(" ticks)\n");
    }

    // Enable CMOS LED
    if((err = px_gt511_cmos_led_control(true)) != PX_GT511_ERR_NONE)
    {
        goto error;
    }

    // Start enrollment procedure for ID 0
    printf("Starting enrollment procedure\n");
    if((err = px_gt511_enroll_start(0)) != PX_GT511_ERR_NONE)
    {
        goto error;
    }

    for(i=0; i < 3; i++)
    {
        // Wait until finger is placed on sensor
        printf("\nPlace finger on sensor... ");
        if((err = px_gt511_wait_until_finger_pressed(PX_SYSTMR_MS_TO_TICKS(10000))) != PX_GT511_ERR_NONE)
        {
            goto error;
        }
        printf("done\n");

        // Capture finger image (in high resolution)
        printf("Capturing finger image... ");
        main_timestamp_start();
        if((err = px_gt511_capture_finger(true)) != PX_GT511_ERR_NONE)
        {
            goto error;
        }
        printf("done (");
        main_timestamp_report();
        printf(" ticks)\n");

        printf("Enroll %u... ", i);
        main_timestamp_start();
        switch(i)
        {
        case 0:
            // Enroll 1
            if((err = px_gt511_enroll_1()) != PX_GT511_ERR_NONE)
            {
                goto error;
            }
            break;
        case 1:
            // Enroll 2
            if((err = px_gt511_enroll_2()) != PX_GT511_ERR_NONE)
            {
                goto error;
            }
            break;
        case 2:
            // Enroll 3
            if((err = px_gt511_enroll_3()) != PX_GT511_ERR_NONE)
            {
                goto error;
            }
            break;
        default:
            break;
        }
        printf("done (");
        main_timestamp_report();
        printf(" ticks)\n");

        // Wait until finger is removed
        printf("Remove finger... ");
        if((err = px_gt511_wait_until_finger_released(PX_SYSTMR_MS_TO_TICKS(10000))) != PX_GT511_ERR_NONE)
        {
            goto error;
        }
        printf("done\n");
    }
    printf("Enrollment successful\n\n");

    // Disable CMOS LED
    if((err = px_gt511_cmos_led_control(false)) != PX_GT511_ERR_NONE)
    {
        goto error;
    }
    px_systmr_wait(PX_SYSTMR_MS_TO_TICKS(2000));

    for(i=0; i < 10; i++)
    {
        // Enable CMOS LED
        if((err = px_gt511_cmos_led_control(true)) != PX_GT511_ERR_NONE)
        {
            goto error;
        }
    
        // Wait until finger is placed on sensor
        printf("Place finger on sensor... ");
        if((err = px_gt511_wait_until_finger_pressed(PX_SYSTMR_MS_TO_TICKS(10000))) != PX_GT511_ERR_NONE)
        {
            goto error;
        }
        printf("done\n");

        // Capture finger image (in low resolution)
        printf("Capturing finger image... ");
        main_timestamp_start();
        if((err = px_gt511_capture_finger(false)) != PX_GT511_ERR_NONE)
        {
            main_report_err(err);
            printf("\n");
        }
        else
        {
            printf("done (");
            main_timestamp_report();
            printf(" ticks)\n");

            // Identify finger
            printf("Identify finger... ");
            main_timestamp_start();
            if((err = px_gt511_identify(&id)) != PX_GT511_ERR_NONE)
            {
                main_report_err(err);
            }
            else
            {
                printf("ID %u", id);
            }
            printf(" (");
            main_timestamp_report();
            printf(" ticks)\n");
        }

        // Wait until finger is removed
        printf("Remove finger... ");
        if((err = px_gt511_wait_until_finger_released(PX_SYSTMR_MS_TO_TICKS(10000))) != PX_GT511_ERR_NONE)
        {
            goto error;
        }
        printf("done\n\n");
        // Disable CMOS LED
        if((err = px_gt511_cmos_led_control(false)) != PX_GT511_ERR_NONE)
        {
            goto error;
        }
        px_systmr_wait(PX_SYSTMR_MS_TO_TICKS(2000));
    }

    // Send CLOSE command
    printf("Closing communication... ");
    if((err = px_gt511_close()) != PX_GT511_ERR_NONE)
    {
        goto error;
    }
    printf("done\n");

    // The end
    for(;;)
    {
        ;
    }

error:
    // Report error
    main_report_err(err);
    for(;;)
    {
        ;
    }
}
