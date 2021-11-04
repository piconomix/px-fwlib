/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2013 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          COMedia C1098 CMOS Camera Module driver
    Author(s):      Pieter Conradie
    Creation Date:  2013-03-30

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_c1098.h"
#include "px_uart.h"
#include "px_systmr.h"
#include "px_sysclk.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("px_c1098");

typedef enum
{
    PX_C1098_STATE_DONE = 0,
    PX_C1098_STATE_ERROR,
    PX_C1098_STATE_WAIT_ACK,
    PX_C1098_STATE_WAIT_AFTER_ACK,
    PX_C1098_STATE_SYNC,
    PX_C1098_STATE_SYNC_WAIT_ACK,
    PX_C1098_STATE_SYNC_WAIT_SYNC,
    PX_C1098_STATE_GET_PIC_WAIT_ACK,
    PX_C1098_STATE_WAIT_DATA_LENGTH,
    PX_C1098_STATE_WAIT_DATA_PACKAGE,
} px_c1098_state_t;

/// C1098 Command format
typedef struct
{
    uint8_t start;
    uint8_t id;
    uint8_t param1;
    uint8_t param2;
    uint8_t param3;
    uint8_t param4;
} px_c1098_cmd_t;

/// Package data format
typedef struct
{
    uint8_t id_lo8;
    uint8_t id_hi8;
    uint8_t data_size_lo8;
    uint8_t data_size_hi8;
    uint8_t image_data[PX_C1098_DATA_BLOCK_SIZE];
    uint8_t verify_code_lo8;
    uint8_t verify_code_hi8;
} px_c1098_package_data_t;

#define PX_C1098_RX_DATA_BUFFER_SIZE 256

// C1098 Start character
#define PX_C1098_START                 0xaa

// C1098 IDs
#define PX_C1098_ID_INITIAL            0x01
#define PX_C1098_ID_GET_PICTURE        0x04
#define PX_C1098_ID_SNAPSHOT           0x05
#define PX_C1098_ID_SET_PACKAGE_SIZE   0x06
#define PX_C1098_ID_RESET              0x08
#define PX_C1098_ID_DATA_LENGTH        0x0a
#define PX_C1098_ID_SYNC               0x0d
#define PX_C1098_ID_ACK                0x0e
#define PX_C1098_ID_NAK                0x0f

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static px_uart_handle_t *    px_c1098_uart_handle;
static px_c1098_state_t      px_c1098_state;
static px_systmr_t           px_c1098_state_tmr;
static px_systmr_ticks_t     px_c1098_delay_after_ack_in_ticks;
static px_c1098_cmd_t        px_c1098_tx_cmd_packet;
static uint8_t               px_c1098_retry_cntr;

static union
{
    px_c1098_cmd_t           cmd;
    px_c1098_package_data_t  package_data;
    uint8_t                  data[sizeof(px_c1098_package_data_t)];
} px_c1098_rx_packet;

static uint16_t              px_c1098_rx_data_index;
static px_c1098_on_rx_data_t px_c1098_on_rx_data;
static uint32_t              px_c1098_data_length;
static uint16_t              px_c1098_package_id;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
const char * px_c1098_state_to_str(px_c1098_state_t state)
{
    switch(state)
    {
    case PX_C1098_STATE_DONE:              return "DONE");
    case PX_C1098_STATE_ERROR:             return "ERROR");
    case PX_C1098_STATE_WAIT_ACK:          return "WAIT_ACK");
    case PX_C1098_STATE_WAIT_AFTER_ACK:    return "WAIT_AFTER_ACK");
    case PX_C1098_STATE_SYNC:              return "SYNC");
    case PX_C1098_STATE_SYNC_WAIT_ACK:     return "SYNC_WAIT_ACK");
    case PX_C1098_STATE_SYNC_WAIT_SYNC:    return "SYNC_WAIT_SYNC");
    case PX_C1098_STATE_GET_PIC_WAIT_ACK:  return "GET_PIC_WAIT_ACK");
    case PX_C1098_STATE_WAIT_DATA_LENGTH:  return "WAIT_DATA_LENGTH");
    case PX_C1098_STATE_WAIT_DATA_PACKAGE: return "WAIT_DATA_PACKAGE");
    default:                               return "???");
    }
}
static void px_c1098_tx_data(uint8_t * data, uint8_t nr_of_bytes)
{
    while(nr_of_bytes != 0)
    {
        px_uart_putchar(px_c1098_uart_handle, *data++);
        nr_of_bytes--;
    }
}

static void px_c1098_tx_cmd(uint8_t id, 
                            uint8_t param1, 
                            uint8_t param2, 
                            uint8_t param3,
                            uint8_t param4)
{
    px_c1098_tx_cmd_packet.start  = PX_C1098_START;
    px_c1098_tx_cmd_packet.id     = id;
    px_c1098_tx_cmd_packet.param1 = param1;
    px_c1098_tx_cmd_packet.param2 = param2;
    px_c1098_tx_cmd_packet.param3 = param3;
    px_c1098_tx_cmd_packet.param4 = param4;

    PX_LOG_D("TX %02X %02X %02X %02X %02X %02X", px_c1098_tx_cmd_packet.start,
                                                 px_c1098_tx_cmd_packet.id,
                                                 px_c1098_tx_cmd_packet.param1,
                                                 px_c1098_tx_cmd_packet.param2,
                                                 px_c1098_tx_cmd_packet.param3,
                                                 px_c1098_tx_cmd_packet.param4);

    px_c1098_tx_data((uint8_t *)&px_c1098_tx_cmd_packet, sizeof(px_c1098_tx_cmd_packet));
}

static void px_c1098_change_state(px_c1098_state_t new_state)
{
    // Change state
    px_c1098_state = new_state;
    // Reset receiver
    px_c1098_rx_data_index = 0;

    // Report new state
    PX_LOG_D("%010ul - state %s", (unsigned long)px_sysclk_get_tick_count(),
                                  px_c1098_state_to_str(px_c1098_state));
}

static void px_c1098_ack_data_package(void)
{
    // Send ACK
    px_c1098_tx_cmd(PX_C1098_ID_ACK, 0x00, 0x00, PX_U16_LO8(px_c1098_package_id),
                                                 PX_U16_HI8(px_c1098_package_id));
    px_systmr_start(&px_c1098_state_tmr, PX_SYSTMR_MS_TO_TICKS(200));
    px_c1098_change_state(PX_C1098_STATE_WAIT_DATA_PACKAGE);
}

static bool px_c1098_package_data_checksum_ok(uint16_t data_size)
{
    uint16_t checksum = 0;
    uint16_t i;

    for(i=0; i<(data_size+4); i++)
    {
        checksum += px_c1098_rx_packet.data[i];
    }

    if(  (px_c1098_rx_packet.data[i]   == PX_U16_LO8(checksum))
       &&(px_c1098_rx_packet.data[i+1] == 0x00             )  )
    {
        return true;
    }
    else
    {
        return false;
    }
}

static void px_c1098_on_rx_byte(uint8_t data)
{
    uint16_t data_size;

    if(PX_LOG_LEVEL_IS_D())
    {
        if(px_c1098_rx_data_index == 0)
        {
            PX_LOG_D("RX %02X", data);
        }
        else
        {
            PX_LOG_TRACE(" %02X", data);
        }
    }

    // Buffer full?
    if(px_c1098_rx_data_index >= sizeof(px_c1098_rx_packet))
    {
        // Discard received byte
        return;
    }

    // Buffer received data
    px_c1098_rx_packet.data[px_c1098_rx_data_index++] = data;

    // Received command?
    if(  (px_c1098_state               != PX_C1098_STATE_WAIT_DATA_PACKAGE)
       &&(px_c1098_rx_data_index       == sizeof(px_c1098_cmd_t)          )
       &&(px_c1098_rx_packet.cmd.start == PX_C1098_START                  )  )
    {
        // Received NAK?
        if(  (px_c1098_rx_packet.cmd.id     == PX_C1098_ID_NAK)
           &&(px_c1098_rx_packet.cmd.param4 == 0x00           )  )
        {
            //Error
            px_c1098_change_state(PX_C1098_STATE_ERROR);
			return;
        }

        // Handle command according to state
        switch(px_c1098_state)
        {
        case PX_C1098_STATE_WAIT_ACK:
            if(  (px_c1098_rx_packet.cmd.id     == PX_C1098_ID_ACK          )
               &&(px_c1098_rx_packet.cmd.param1 == px_c1098_tx_cmd_packet.id)
               &&(px_c1098_rx_packet.cmd.param3 == 0x00                     )
               &&(px_c1098_rx_packet.cmd.param4 == 0x00                     )  )
            {
                if(px_c1098_delay_after_ack_in_ticks != 0)
                {
                    px_systmr_start(&px_c1098_state_tmr, px_c1098_delay_after_ack_in_ticks);
                    px_c1098_change_state(PX_C1098_STATE_WAIT_AFTER_ACK);
                }
                else
                {
                    px_c1098_change_state(PX_C1098_STATE_DONE);
                }
            }
            break;

        case PX_C1098_STATE_SYNC_WAIT_ACK:
            if(  (px_c1098_rx_packet.cmd.id     == PX_C1098_ID_ACK )
               &&(px_c1098_rx_packet.cmd.param1 == PX_C1098_ID_SYNC)
               &&(px_c1098_rx_packet.cmd.param3 == 0x00            )
               &&(px_c1098_rx_packet.cmd.param4 == 0x00            )  )
            {
                px_c1098_change_state(PX_C1098_STATE_SYNC_WAIT_SYNC);
            }
            break;

        case PX_C1098_STATE_SYNC_WAIT_SYNC:
            if(  (px_c1098_rx_packet.cmd.id     == PX_C1098_ID_SYNC)
               &&(px_c1098_rx_packet.cmd.param1 == 0x00            )
               &&(px_c1098_rx_packet.cmd.param2 == 0x00            )
               &&(px_c1098_rx_packet.cmd.param3 == 0x00            )
               &&(px_c1098_rx_packet.cmd.param4 == 0x00            )  )
            {
                // Send ACK
                px_c1098_tx_cmd(PX_C1098_ID_ACK, 0x00, 0x00, 0x00, 0x00);
                px_c1098_change_state(PX_C1098_STATE_DONE);
    
            }
            break;

        case PX_C1098_STATE_GET_PIC_WAIT_ACK:
            if(  (px_c1098_rx_packet.cmd.id     == PX_C1098_ID_ACK          )
               &&(px_c1098_rx_packet.cmd.param1 == px_c1098_tx_cmd_packet.id)
               &&(px_c1098_rx_packet.cmd.param3 == 0x00                     )
               &&(px_c1098_rx_packet.cmd.param4 == 0x00                     )  )
            {
                px_c1098_change_state(PX_C1098_STATE_WAIT_DATA_LENGTH);
            }
            break;

        case PX_C1098_STATE_WAIT_DATA_LENGTH:
            if(  (px_c1098_rx_packet.cmd.id     == PX_C1098_ID_DATA_LENGTH)
               &&(px_c1098_rx_packet.cmd.param1 == 0x01                   )  )
            {
                px_c1098_data_length = PX_U32_CONCAT_U8(0,
                                                        px_c1098_rx_packet.cmd.param4,
                                                        px_c1098_rx_packet.cmd.param3,
                                                        px_c1098_rx_packet.cmd.param2);
                PX_LOG_D("Data Length = %u bytes", px_c1098_data_length);
                px_c1098_ack_data_package();
            }
            break;

        default:
            break;
        }
    }

    // Received Package ID and Package Length?
    if(px_c1098_rx_data_index < 4)
    {
        return;
    }

    // Package ID correct?
    if(  (px_c1098_rx_packet.package_data.id_hi8 != PX_U16_HI8(px_c1098_package_id))
       ||(px_c1098_rx_packet.package_data.id_lo8 != PX_U16_LO8(px_c1098_package_id))  )
    {
        return;
    }

    // Calculate data size
    data_size = PX_U16_CONCAT_U8(px_c1098_rx_packet.package_data.data_size_hi8,
                                 px_c1098_rx_packet.package_data.data_size_lo8);

    // Data size correct?
    if(data_size > PX_C1098_DATA_BLOCK_SIZE)
    {
        return;
    }
    if(data_size != PX_C1098_DATA_BLOCK_SIZE)
    {
        if(data_size != px_c1098_data_length)
        {
            return;
        }
    }

    // Received whole package?
    if(px_c1098_rx_data_index < (data_size+6))
    {
        return;
    }

    // Checksum correct?
    if(!px_c1098_package_data_checksum_ok(data_size))
    {
        PX_LOG_E("Checksum incorrect");
        return;
    }

    // Pass data to handler
    (*px_c1098_on_rx_data)(px_c1098_rx_packet.package_data.image_data, data_size);

    // Last package?
    if(px_c1098_data_length == data_size)
    {
        px_c1098_data_length = 0;
        // Done!
        px_c1098_change_state(PX_C1098_STATE_DONE);
    }
    else
    {
        px_c1098_data_length -= data_size;
        PX_LOG_D("Data Length = %u", px_c1098_data_length);
        // Next package ID
        px_c1098_package_id++;
        // Reset retry counter
        px_c1098_retry_cntr = 3;
        // Send ACK
        px_c1098_ack_data_package();
    }
}


/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_c1098_init(px_uart_handle_t * handle)
{
    px_c1098_uart_handle               = handle;
    px_c1098_state                     = PX_C1098_STATE_DONE;
    px_c1098_on_rx_data                = NULL;
    px_c1098_rx_data_index             = 0;
    px_c1098_delay_after_ack_in_ticks  = 0;
    px_systmr_stop(&px_c1098_state_tmr);
}

void px_c1098_task(void)
{
    uint8_t data;

    // Process received data
    while(px_uart_rd_u8(px_c1098_uart_handle, &data))
    {
        px_c1098_on_rx_byte(data);
    }

    switch(px_c1098_state)
    {
    case PX_C1098_STATE_WAIT_ACK:
        // Fall through...
    case PX_C1098_STATE_GET_PIC_WAIT_ACK:
        // Fall through...
    case PX_C1098_STATE_WAIT_DATA_LENGTH:
        // Timer expired?
        if(px_systmr_has_expired(&px_c1098_state_tmr))
        {
            // Error
            px_c1098_change_state(PX_C1098_STATE_ERROR);
        }
        break;

    case PX_C1098_STATE_WAIT_DATA_PACKAGE:
        // Timer expired?
        if(px_systmr_has_expired(&px_c1098_state_tmr))
        {
            // Retry?
            if(px_c1098_retry_cntr > 0)
            {
                // Retry
                px_c1098_retry_cntr--;
                px_c1098_ack_data_package();
            }
            else
            {
                // Error
                px_c1098_change_state(PX_C1098_STATE_ERROR);
            }
        }
        break;

    case PX_C1098_STATE_WAIT_AFTER_ACK:
        // Timer expired?
        if(px_systmr_has_expired(&px_c1098_state_tmr))
        {
            // Done
            px_c1098_change_state(PX_C1098_STATE_DONE);
        }
        break;

    case PX_C1098_STATE_SYNC:
        // Send SYNC command
        px_c1098_tx_cmd(PX_C1098_ID_SYNC, 0x00, 0x00, 0x00, 0x00);

        // Start timeout and change state
        px_systmr_start(&px_c1098_state_tmr, PX_SYSTMR_MS_TO_TICKS(200));
        px_c1098_change_state(PX_C1098_STATE_SYNC_WAIT_ACK);
        break;

    case PX_C1098_STATE_SYNC_WAIT_ACK:
        // Timer expired?
        if(px_systmr_has_expired(&px_c1098_state_tmr))
        {
            // Retry?
            if(px_c1098_retry_cntr > 0)
            {
                // Retry
                px_c1098_retry_cntr--;
                px_c1098_change_state(PX_C1098_STATE_SYNC);
            }
            else
            {
                // Error
                px_c1098_change_state(PX_C1098_STATE_ERROR);
            }
        }
        break;

    default:
        break;
    }
}

bool px_c1098_busy(void)
{
    if(  (px_c1098_state != PX_C1098_STATE_DONE )
       &&(px_c1098_state != PX_C1098_STATE_ERROR)  )
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool px_c1098_error(void)
{
    if(px_c1098_state == PX_C1098_STATE_ERROR)
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool px_c1098_task_execute(void)
{
    while(px_c1098_busy())
    {
        px_c1098_task();
    }

    return px_c1098_error();
}

void px_c1098_initial(uint8_t if_speed, uint8_t jpeg_res)
{
    PX_LOG_D("INITIAL");
    // Send INITIAL command
    px_c1098_tx_cmd(PX_C1098_ID_INITIAL, if_speed, 0x07, 0x00, jpeg_res);
    // Start timeout
    px_systmr_start(&px_c1098_state_tmr, PX_SYSTMR_MS_TO_TICKS(200));
    // post-ACK delay
    px_c1098_delay_after_ack_in_ticks = PX_SYSTMR_MS_TO_TICKS(50);
    // Wait for ACK
    px_c1098_change_state(PX_C1098_STATE_WAIT_ACK);
}

void px_c1098_set_package_size(void)
{
    PX_LOG_D("SET_PACKAGE_SIZE");
    // Send PACKAGE_SIZE command
    px_c1098_tx_cmd(PX_C1098_ID_SET_PACKAGE_SIZE, 0x08, PX_U16_LO8(PX_C1098_DATA_BLOCK_SIZE+6), PX_U16_HI8(PX_C1098_DATA_BLOCK_SIZE+6), 0x00);
    // Start timeout
    px_systmr_start(&px_c1098_state_tmr, PX_SYSTMR_MS_TO_TICKS(200));
    // No post-ACK delay
    px_c1098_delay_after_ack_in_ticks = 0;
    // Wait for ACK
    px_c1098_change_state(PX_C1098_STATE_WAIT_ACK);
}

void px_c1098_reset(uint8_t rst_priority)
{
    PX_LOG_D("RESET");
    // Send RESET command
    px_c1098_tx_cmd(PX_C1098_ID_RESET, 0x00, 0x00, 0x00, rst_priority);
    // Start timeout
    px_systmr_start(&px_c1098_state_tmr, PX_SYSTMR_MS_TO_TICKS(200));
    // post-ACK delay
    px_c1098_delay_after_ack_in_ticks = PX_SYSTMR_MS_TO_TICKS(20);
    // Wait for ACK
    px_c1098_change_state(PX_C1098_STATE_WAIT_ACK);
}

void px_c1098_sync(void)
{
    PX_LOG_D("SYNC");
    px_c1098_retry_cntr = 10;
    px_c1098_change_state(PX_C1098_STATE_SYNC);
}

void px_c1098_snapshot(void)
{
    PX_LOG_D("SNAPSHOT");
    // Send SNAPSHOT command
    px_c1098_tx_cmd(PX_C1098_ID_SNAPSHOT, 0x00, 0x00, 0x00, 0x00);

    // Start timeout and change state
    px_systmr_start(&px_c1098_state_tmr, PX_SYSTMR_MS_TO_TICKS(200));
    px_c1098_change_state(PX_C1098_STATE_WAIT_ACK);
}

uint32_t px_c1098_get_picture_size(void)
{
    return px_c1098_data_length;
}

void px_c1098_get_picture(px_c1098_on_rx_data_t on_rx_data)
{
    // Set handler to process / store received image data
    px_c1098_on_rx_data = on_rx_data;

    // Reset package ID
    px_c1098_package_id = 0;

    // Reset retry counter
    px_c1098_retry_cntr = 3;

    PX_LOG_D("GET_PICTURE");
    // Send GET_PICTURE command
    px_c1098_tx_cmd(PX_C1098_ID_GET_PICTURE, 0x01, 0x00, 0x00, 0x00);

    // Start timeout and change state
    px_systmr_start(&px_c1098_state_tmr, PX_SYSTMR_MS_TO_TICKS(200));
    px_c1098_change_state(PX_C1098_STATE_GET_PIC_WAIT_ACK);
}