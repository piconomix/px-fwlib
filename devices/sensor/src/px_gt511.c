/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
 
    Title:          px_gt511.h : ADH Tech GT-511 Fingerprint sensor driver
    Author(s):      Pieter Conradie
    Creation Date:  2018-07-17

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_gt511.h"
#include "px_uart.h"
#include "px_systmr.h"
#include "px_sysclk.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("px_gt511");

/// @name GT511 Commands
/// @{
#define PX_GT511_CMD_OPEN                   0x01    ///< Initialization
#define PX_GT511_CMD_CLOSE                  0x02    ///< Termination
#define PX_GT511_CMD_USB_CHECK              0x03    ///< Check if the connected USB device is valid
#define PX_GT511_CMD_CHANGE_BAUD            0x04    ///< Change UART baud rate
#define PX_GT511_CMD_SET_IAP_MODE           0x05    ///< Enter IAP Mode. In this mode, FW Upgrade is available
#define PX_GT511_CMD_CMOS_LED               0x12    ///< Control CMOS LED
#define PX_GT511_CMD_GET_ENROLL_COUNT       0x20    ///< Get enrolled fingerprint count
#define PX_GT511_CMD_CHECK_ENROLLED         0x21    ///< Check whether the specified ID is already enrolled
#define PX_GT511_CMD_ENROLL_START           0x22    ///< Start an enrollment
#define PX_GT511_CMD_ENROLL_1               0x23    ///< Make 1st template for an enrollment
#define PX_GT511_CMD_ENROLL_2               0x24    ///< Make 2nd template for an enrollment
#define PX_GT511_CMD_ENROLL_3               0x25    ///< Make 3rd template for an enrollment, merge three templates into one template, save merged template to the database
#define PX_GT511_CMD_IS_PRESS_FINGER        0x26    ///< Check if a finger is placed on the sensor
#define PX_GT511_CMD_DELETE_ID              0x40    ///< Delete the fingerprint with the specified ID
#define PX_GT511_CMD_DELETE_ALL             0x41    ///< Delete all fingerprints from the database
#define PX_GT511_CMD_VERIFY                 0x50    ///< 1:1 Verification of the capture fingerprint image with the specified ID
#define PX_GT511_CMD_IDENTIFY               0x51    ///< 1:N Identification of the capture fingerprint image with the database
#define PX_GT511_CMD_VERIFY_TEMPLATE        0x52    ///< 1:1 Verification of a fingerprint template with the specified ID
#define PX_GT511_CMD_IDENTIFY_TEMPLATE      0x53    ///< 1:N Indentification of a fingerprint template with the database
#define PX_GT511_CMD_CAPTURE_FINGER         0x60    ///< Capture a fingerprint image(256x256) from the sensor
#define PX_GT511_CMD_MAKE_TEMPLATE          0x61    ///< Make template for transmission
#define PX_GT511_CMD_GET_IMAGE              0x62    ///< Download the captured fingerprint image(256x256)
#define PX_GT511_CMD_GET_RAW_IMAGE          0x63    ///< Capture & Download raw fingerprint image(320x240)
#define PX_GT511_CMD_GET_TEMPLATE           0x70    ///< Download the template of the specified ID
#define PX_GT511_CMD_SET_TEMPLATE           0x71    ///< Upload the template of the specified ID
#define PX_GT511_CMD_ACK                    0x30    ///< Acknowledge
#define PX_GT511_CMD_NACK                   0x31    ///< Non-acknowledge
/// @}

/// GT511 Command Packet
typedef struct
{
    uint8_t  start_code1;       ///< 0x55
    uint8_t  start_code2;       ///< 0xaa
    uint16_t device_id;         ///< 0x001
    uint32_t param;
    uint16_t cmd;
    uint16_t checksum;
} px_gt511_cmd_packet_t;

/// GT511 Response Packet
typedef struct
{
    uint8_t  start_code1;       ///< 0x55
    uint8_t  start_code2;       ///< 0xaa
    uint16_t device_id;         ///< 0x001
    uint32_t param;
    uint16_t resp;
    uint16_t checksum;
} px_gt511_resp_packet_t;

/// GT511 Data Packet (variable length data and checksum excluded)
typedef struct
{
    uint8_t  start_code1;       ///< 0x5a
    uint8_t  start_code2;       ///< 0xa5
    uint16_t device_id;         ///< 0x001
} px_gt511_data_packet_t;


/// Receive timeout (in milliseconds)
#define PX_GT511_RX_TIMEOUT_MS              200

/// Identify / verify timeout (in milliseconds)
#define PX_GT511_ID_TIMEOUT_MS              2000

/// Image timeout (in milliseconds)
#define PX_GT511_IMAGE_TIMEOUT_MS           1000

/// Enroll timeout (in milliseconds)
#define PX_GT511_ENROLL_TIMEOUT_MS          1000

/// Delete timeout (in milliseconds)
#define PX_GT511_DELETE_TIMEOUT_MS          1000

/// Receive interbyte timeout (in milliseconds)
#define PX_GT511_RX_INTERBYTE_TIMEOUT_MS    100

/// Receive buffer flush timeout (in milliseconds)
#define PX_GT511_RX_FLUSH_TIMEOUT_MS        100

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static px_uart_handle_t *     px_gt511_uart_handle;
static px_systmr_t            px_gt511_tmr;
static px_gt511_cmd_packet_t  px_gt511_cmd_packet;
static px_gt511_resp_packet_t px_gt511_resp_packet;
static px_gt511_data_packet_t px_gt511_data_packet;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */
static uint16_t px_gt511_calc_checksum(uint16_t          checksum, 
                                       void *            data, 
                                       size_t            nr_of_bytes);

static void px_gt511_tx               (void *            data, 
                                       size_t            nr_of_bytes);

static bool px_gt511_rx               (void *            buffer, 
                                       size_t            nr_of_bytes,
                                       px_systmr_ticks_t timeout_ticks);

static void px_gt511_rx_flush         (void);

static void px_gt511_tx_cmd_packet    (uint16_t          cmd, 
                                       uint32_t          param);

static void px_gt511_tx_data_packet   (void *            data, 
                                       size_t            nr_of_bytes);

static bool px_gt511_rx_resp_packet   (px_systmr_ticks_t timeout_ticks);

static bool px_gt511_rx_data_packet   (void *            data, 
                                       size_t            nr_of_bytes,
                                       px_systmr_ticks_t timeout_ticks);

/* _____LOCAL FUNCTIONS______________________________________________________ */
static uint16_t px_gt511_calc_checksum(uint16_t checksum, 
                                       void *   data, 
                                       size_t   nr_of_bytes)
{
    uint8_t * data_u8  = (uint8_t *)data;

    while(nr_of_bytes != 0)
    {
        checksum += *data_u8++;
        nr_of_bytes--;
    }

    return checksum;
}

static void px_gt511_tx(void * data, size_t nr_of_bytes)
{
    uint8_t * data_u8 = (uint8_t *)data;
    while(nr_of_bytes != 0)
    {
        px_uart_putchar(px_gt511_uart_handle, *data_u8++);
        nr_of_bytes--;
    }
}

static bool px_gt511_rx(void *            buffer, 
                        size_t            nr_of_bytes, 
                        px_systmr_ticks_t timeout_ticks)
{
    uint8_t * buffer_u8    = (uint8_t *)buffer;
    size_t    buffer_index = 0;

    // Receive bytes until buffer is full or timeout
    px_systmr_start(&px_gt511_tmr, timeout_ticks);
    while(buffer_index < nr_of_bytes)
    {
        // Receive byte?
        if(px_uart_rd_u8(px_gt511_uart_handle, buffer_u8))
        {
            // Next byte
            buffer_u8++;
            buffer_index++;
            // (Re)start interbyte timer
            px_systmr_start(&px_gt511_tmr, PX_SYSTMR_MS_TO_TICKS(PX_GT511_RX_INTERBYTE_TIMEOUT_MS));
        }
        // Timeout?
        if(px_systmr_has_expired(&px_gt511_tmr))
        {
            PX_LOG_E("RX Timeout (received %u bytes)", buffer_index);
            return false;
        }
    }

    // Success
    return true;
}

static void px_gt511_rx_flush(void)
{
    uint8_t data;

    // Start timeout
    px_systmr_start(&px_gt511_tmr, PX_SYSTMR_MS_TO_TICKS(PX_GT511_RX_FLUSH_TIMEOUT_MS));
    // Wait until timeout
    while(!px_systmr_has_expired(&px_gt511_tmr))
    {
        // More data in receive buffer?
        if(px_uart_rd_u8(px_gt511_uart_handle, &data))
        {
            // Restart timeout
            px_systmr_restart(&px_gt511_tmr);
        }
    }
}

static void px_gt511_tx_cmd_packet(uint16_t cmd, uint32_t param)
{
    PX_LOG_D("TX Cmd %04X Param %08X", cmd, param);

    // Fill fields
    px_gt511_cmd_packet.start_code1 = 0x55;
    px_gt511_cmd_packet.start_code2 = 0xaa;
    px_gt511_cmd_packet.device_id   = 0x001;
    px_gt511_cmd_packet.param       = param;
    px_gt511_cmd_packet.cmd         = cmd;

    // Calculate checksum
    px_gt511_cmd_packet.checksum = px_gt511_calc_checksum(0,
                                                          &px_gt511_cmd_packet, 
                                                          offsetof(px_gt511_cmd_packet_t, checksum));

    px_gt511_tx(&px_gt511_cmd_packet, sizeof(px_gt511_cmd_packet));
}

static void px_gt511_tx_data_packet(void * data, size_t nr_of_bytes)
{
    uint16_t checksum;

    // Fill fields
    px_gt511_data_packet.start_code1 = 0x5a;
    px_gt511_data_packet.start_code2 = 0xa5;
    px_gt511_data_packet.device_id   = 0x001;

    px_gt511_tx(&px_gt511_data_packet, sizeof(px_gt511_data_packet));
    px_gt511_tx(data, nr_of_bytes);

    // Send checksum
    checksum = px_gt511_calc_checksum(0,
                                      &px_gt511_data_packet, 
                                      sizeof(px_gt511_data_packet_t));
    checksum = px_gt511_calc_checksum(checksum,
                                      data, 
                                      nr_of_bytes);
    px_gt511_tx(&checksum, sizeof(checksum));
}

static bool px_gt511_rx_resp_packet(px_systmr_ticks_t timeout_ticks)
{
    uint16_t  checksum;

    // Receive response packet
    if(!px_gt511_rx(&px_gt511_resp_packet, sizeof(px_gt511_resp_packet), timeout_ticks))
    {
        return false;
    }

    // Header OK?
    if(  (px_gt511_resp_packet.start_code1 != 0x55 )
       ||(px_gt511_resp_packet.start_code2 != 0xaa )
       ||(px_gt511_resp_packet.device_id   != 0x001)  )
    {
        PX_LOG_E("RX Header incorrect (%02X %02X %04X)",
                 px_gt511_resp_packet.start_code1,
                 px_gt511_resp_packet.start_code2,
                 px_gt511_resp_packet.device_id);
        // Flush remainder of packet
        px_gt511_rx_flush();
        return false;
    }
    // Checksum OK?
    checksum = px_gt511_calc_checksum(0,
                                      &px_gt511_resp_packet, 
                                      offsetof(px_gt511_resp_packet_t, checksum));
    if(checksum != px_gt511_resp_packet.checksum)
    {
        PX_LOG_E("RX Checksum does not match (%04X != %04X)",
                 px_gt511_resp_packet.checksum, checksum);
        // Flush remainder of packet
        px_gt511_rx_flush();
        return false;
    }
    // Response field OK?
    if(  (px_gt511_resp_packet.resp != PX_GT511_CMD_ACK )
       &&(px_gt511_resp_packet.resp != PX_GT511_CMD_NACK)  )
    {
        PX_LOG_E("Response field not OK (%02X)", px_gt511_resp_packet.resp);
        return false;
    }

    // Success
    if(px_gt511_resp_packet.resp == PX_GT511_CMD_ACK)
    {
        PX_LOG_D("RX Resp ACK");
    }
    else
    {
        PX_LOG_D("RX Resp NACK (Error Code %04X)", px_gt511_resp_packet.param);
    }
}

static bool px_gt511_rx_data_packet(void *            data,
                                    size_t            nr_of_bytes, 
                                    px_systmr_ticks_t timeout_ticks)
{
    uint16_t  checksum;
    uint16_t  checksum_data;

    // Receive data packet header
    if(!px_gt511_rx(&px_gt511_data_packet, sizeof(px_gt511_data_packet), timeout_ticks))
    {
        return false;
    }

    // Header OK?
    if(  (px_gt511_data_packet.start_code1 != 0x5a )
       ||(px_gt511_data_packet.start_code2 != 0xa5 )
       ||(px_gt511_data_packet.device_id   != 0x001)  )
    {
        PX_LOG_E("RX Header incorrect (%02X %02X %04X)",
                 px_gt511_data_packet.start_code1,
                 px_gt511_data_packet.start_code2,
                 px_gt511_data_packet.device_id);
        // Flush remainder of packet
        px_gt511_rx_flush();
        return false;
    }

    // Receive data portion
    if(!px_gt511_rx(data, nr_of_bytes, PX_SYSTMR_MS_TO_TICKS(PX_GT511_RX_INTERBYTE_TIMEOUT_MS)))
    {
        return false;
    }
    // Receive checksum
    if(!px_gt511_rx(&checksum_data, sizeof(checksum_data), PX_SYSTMR_MS_TO_TICKS(PX_GT511_RX_INTERBYTE_TIMEOUT_MS)))
    {
        return false;
    }

    // Checksum OK?
    checksum = px_gt511_calc_checksum(0,
                                      &px_gt511_data_packet, 
                                      sizeof(px_gt511_data_packet_t));
    checksum = px_gt511_calc_checksum(checksum,
                                      data, 
                                      nr_of_bytes);
    if(checksum != checksum_data)
    {
        PX_LOG_E("RX Checksum does not match (%04X != %04X)", checksum_data, checksum);
        // Flush remainder of packet
        px_gt511_rx_flush();
        return false;
    }

    // Success
    PX_LOG_D("RX Data (%u bytes)", nr_of_bytes);
    return true;
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_gt511_init(px_uart_handle_t * handle)
{
    px_gt511_uart_handle = handle;    
}

px_gt511_err_t px_gt511_auto_baud(void)
{
    uint8_t  i;
    uint32_t baud[] =
    {
        9600,
        115200,
        57600,
        38400,
        19200
    };

    for(i = 0; i < PX_SIZEOF_ARRAY(baud); i++)
    {
        PX_LOG_D("Baud = %lu", baud[i]);
        px_uart_ioctl_change_baud(px_gt511_uart_handle, baud[i]);
        if(px_gt511_open(false, NULL) == PX_GT511_ERR_NONE)
        {
            // Success
            return PX_GT511_ERR_NONE;
        }
        // Wait before attempting new baud
        px_systmr_wait(PX_SYSTMR_MS_TO_TICKS(PX_GT511_RX_TIMEOUT_MS));
    }

    // Failure
    return PX_GT511_ERR_INVALID_BAUD;

}

px_gt511_err_t px_gt511_open(bool get_device_info, px_gt511_dev_info_t * dev_info)
{
    // Send command
    PX_LOG_D("OPEN");
    px_gt511_tx_cmd_packet(PX_GT511_CMD_OPEN, (get_device_info == false)? 0 : 1);
    // Receive response
    if(!px_gt511_rx_resp_packet(PX_SYSTMR_MS_TO_TICKS(PX_GT511_RX_TIMEOUT_MS)))
    {
        return PX_GT511_ERR_TIMEOUT;
    }
    // Received NACK?
    if(px_gt511_resp_packet.resp == PX_GT511_CMD_NACK)
    {
        // Return error code
        return (px_gt511_err_t)px_gt511_resp_packet.param;
    }
    // Must device info be received?
    if(get_device_info)
    {
        // Receive data
        if(!px_gt511_rx_data_packet(dev_info, sizeof(px_gt511_dev_info_t), PX_SYSTMR_MS_TO_TICKS(PX_GT511_RX_TIMEOUT_MS)))
        {
            return PX_GT511_ERR_TIMEOUT;
        }    
    }
    // Success
    return PX_GT511_ERR_NONE;
}

px_gt511_err_t px_gt511_close(void)
{
    // Send command
    PX_LOG_D("CLOSE");
    px_gt511_tx_cmd_packet(PX_GT511_CMD_CLOSE, 0);
    // Receive response
    if(!px_gt511_rx_resp_packet(PX_SYSTMR_MS_TO_TICKS(PX_GT511_RX_TIMEOUT_MS)))
    {
        return PX_GT511_ERR_TIMEOUT;
    }
    // Received NACK?
    if(px_gt511_resp_packet.resp == PX_GT511_CMD_NACK)
    {
        // Return error code
        return (px_gt511_err_t)px_gt511_resp_packet.param;
    }    
    // Success
    return PX_GT511_ERR_NONE;
}

px_gt511_err_t px_gt511_cmos_led_control(bool on)
{
    // Send command
    PX_LOG_D("CMOS_LED");
    px_gt511_tx_cmd_packet(PX_GT511_CMD_CMOS_LED, (on == false)? 0 : 1);
    // Receive response
    if(!px_gt511_rx_resp_packet(PX_SYSTMR_MS_TO_TICKS(PX_GT511_RX_TIMEOUT_MS)))
    {
        return PX_GT511_ERR_TIMEOUT;
    }
    // Received NACK?
    if(px_gt511_resp_packet.resp == PX_GT511_CMD_NACK)
    {
        // Return error code
        return (px_gt511_err_t)px_gt511_resp_packet.param;
    }    
    // Success
    return PX_GT511_ERR_NONE;
}

px_gt511_err_t px_gt511_change_baud_rate(uint32_t baud)
{
    // Send command
    PX_LOG_D("CHANGE_BAUD");
    px_gt511_tx_cmd_packet(PX_GT511_CMD_CHANGE_BAUD, baud);
    // Receive response
    if(!px_gt511_rx_resp_packet(PX_SYSTMR_MS_TO_TICKS(PX_GT511_RX_TIMEOUT_MS)))
    {
        return PX_GT511_ERR_TIMEOUT;
    }
    // Received NACK?
    if(px_gt511_resp_packet.resp == PX_GT511_CMD_NACK)
    {
        // Return error code
        return (px_gt511_err_t)px_gt511_resp_packet.param;
    }    
    // Success
    return PX_GT511_ERR_NONE;
}

px_gt511_err_t px_gt511_get_enroll_count(px_gt511_id_t * enroll_count)
{
    // Send command
    PX_LOG_D("ENROLL_COUNT");
    px_gt511_tx_cmd_packet(PX_GT511_CMD_GET_ENROLL_COUNT, 0);
    // Receive response
    if(!px_gt511_rx_resp_packet(PX_SYSTMR_MS_TO_TICKS(PX_GT511_RX_TIMEOUT_MS)))
    {
        return PX_GT511_ERR_TIMEOUT;
    }
    // Received NACK?
    if(px_gt511_resp_packet.resp == PX_GT511_CMD_NACK)
    {
        // Return error code
        return (px_gt511_err_t)px_gt511_resp_packet.param;
    } 
    // Return enroll count
    *enroll_count = (px_gt511_id_t)px_gt511_resp_packet.param;
    // Success
    return PX_GT511_ERR_NONE;
}

px_gt511_err_t px_gt511_check_enrolled(px_gt511_id_t id)
{
    // Send command
    px_gt511_tx_cmd_packet(PX_GT511_CMD_CHECK_ENROLLED, id);
    // Receive response
    if(!px_gt511_rx_resp_packet(PX_SYSTMR_MS_TO_TICKS(PX_GT511_RX_TIMEOUT_MS)))
    {
        return PX_GT511_ERR_TIMEOUT;
    }
    // Received NACK?
    if(px_gt511_resp_packet.resp == PX_GT511_CMD_NACK)
    {
        // Return error code
        return (px_gt511_err_t)px_gt511_resp_packet.param;
    } 
    // Success
    return PX_GT511_ERR_NONE;
}

px_gt511_err_t px_gt511_enroll_start(px_gt511_id_t id)
{
    // Send command
    PX_LOG_D("ENROLL_START");
    px_gt511_tx_cmd_packet(PX_GT511_CMD_ENROLL_START, id);
    // Receive response
    if(!px_gt511_rx_resp_packet(PX_SYSTMR_MS_TO_TICKS(PX_GT511_RX_TIMEOUT_MS)))
    {
        return PX_GT511_ERR_TIMEOUT;
    }
    // Received NACK?
    if(px_gt511_resp_packet.resp == PX_GT511_CMD_NACK)
    {
        // Return error code
        return (px_gt511_err_t)px_gt511_resp_packet.param;
    } 
    // Success
    return PX_GT511_ERR_NONE;
}

px_gt511_err_t px_gt511_enroll_1(void)
{
    // Send command
    PX_LOG_D("ENROLL_1");
    px_gt511_tx_cmd_packet(PX_GT511_CMD_ENROLL_1, 0);
    // Receive response
    if(!px_gt511_rx_resp_packet(PX_SYSTMR_MS_TO_TICKS(PX_GT511_ENROLL_TIMEOUT_MS)))
    {
        return PX_GT511_ERR_TIMEOUT;
    }
    // Received NACK?
    if(px_gt511_resp_packet.resp == PX_GT511_CMD_NACK)
    {
        // Return error code
        return (px_gt511_err_t)px_gt511_resp_packet.param;
    } 
    // Success
    return PX_GT511_ERR_NONE;
}

px_gt511_err_t px_gt511_enroll_2(void)
{
    // Send command
    PX_LOG_D("ENROLL_2");
    px_gt511_tx_cmd_packet(PX_GT511_CMD_ENROLL_2, 0);
    // Receive response
    if(!px_gt511_rx_resp_packet(PX_SYSTMR_MS_TO_TICKS(PX_GT511_ENROLL_TIMEOUT_MS)))
    {
        return PX_GT511_ERR_TIMEOUT;
    }
    // Received NACK?
    if(px_gt511_resp_packet.resp == PX_GT511_CMD_NACK)
    {
        // Return error code
        return (px_gt511_err_t)px_gt511_resp_packet.param;
    } 
    // Success
    return PX_GT511_ERR_NONE;
}

px_gt511_err_t px_gt511_enroll_3(void)
{
    // Send command
    PX_LOG_D("ENROLL_3");
    px_gt511_tx_cmd_packet(PX_GT511_CMD_ENROLL_3, 0);
    // Receive response
    if(!px_gt511_rx_resp_packet(PX_SYSTMR_MS_TO_TICKS(PX_GT511_ENROLL_TIMEOUT_MS)))
    {
        return PX_GT511_ERR_TIMEOUT;
    }
    // Received NACK?
    if(px_gt511_resp_packet.resp == PX_GT511_CMD_NACK)
    {
        // Return error code
        return (px_gt511_err_t)px_gt511_resp_packet.param;
    } 
    // Success
    return PX_GT511_ERR_NONE;
}

px_gt511_err_t px_gt511_is_finger_pressed(bool * is_pressed)
{
    // Send command
    PX_LOG_D("IS_PRESS_FINGER");
    px_gt511_tx_cmd_packet(PX_GT511_CMD_IS_PRESS_FINGER, 0);
    // Receive response
    if(!px_gt511_rx_resp_packet(PX_SYSTMR_MS_TO_TICKS(PX_GT511_RX_TIMEOUT_MS)))
    {
        return PX_GT511_ERR_TIMEOUT;
    }
    // Received NACK?
    if(px_gt511_resp_packet.resp == PX_GT511_CMD_NACK)
    {
        // Return error code
        return (px_gt511_err_t)px_gt511_resp_packet.param;
    }

    if(px_gt511_resp_packet.param == 0)
    {
        // Finger is pressed
        *is_pressed = true;
    }
    else
    {
        // Finger is not pressed
        *is_pressed = false;
    }
    // Success
    return PX_GT511_ERR_NONE;
}

px_gt511_err_t px_gt511_wait_until_finger_pressed(px_systmr_ticks_t timeout_ticks)
{
    px_gt511_err_t err;
    bool           is_pressed;
    px_systmr_t    tmr;

    px_systmr_start(&tmr, timeout_ticks);
    while(true)
    {
        if((err = px_gt511_is_finger_pressed(&is_pressed)) != PX_GT511_ERR_NONE)
        {
            return err;
        }
        if(is_pressed)
        {
            return PX_GT511_ERR_NONE;
        }
        if(px_systmr_has_expired(&tmr))
        {
            return PX_GT511_ERR_TIMEOUT;
        }
        px_systmr_wait(PX_SYSTMR_MS_TO_TICKS(50));
    }
}

px_gt511_err_t px_gt511_wait_until_finger_released(px_systmr_ticks_t timeout_ticks)
{
    px_gt511_err_t err;
    bool           is_pressed;
    px_systmr_t    tmr;

    px_systmr_start(&tmr, timeout_ticks);
    while(true)
    {
        if((err = px_gt511_is_finger_pressed(&is_pressed)) != PX_GT511_ERR_NONE)
        {
            return err;
        }
        if(!is_pressed)
        {
            return PX_GT511_ERR_NONE;
        }
        if(px_systmr_has_expired(&tmr))
        {
            return PX_GT511_ERR_TIMEOUT;
        }
        px_systmr_wait(PX_SYSTMR_MS_TO_TICKS(50));
    }
}

px_gt511_err_t px_gt511_delete_id(px_gt511_id_t id)
{
    // Send command
    PX_LOG_D("DELETE_ID");
    px_gt511_tx_cmd_packet(PX_GT511_CMD_DELETE_ID, id);
    // Receive response
    if(!px_gt511_rx_resp_packet(PX_SYSTMR_MS_TO_TICKS(PX_GT511_DELETE_TIMEOUT_MS)))
    {
        return PX_GT511_ERR_TIMEOUT;
    }
    // Received NACK?
    if(px_gt511_resp_packet.resp == PX_GT511_CMD_NACK)
    {
        // Return error code
        return (px_gt511_err_t)px_gt511_resp_packet.param;
    } 
    // Success
    return PX_GT511_ERR_NONE;
}

px_gt511_err_t px_gt511_delete_all(void)
{
    // Send command
    PX_LOG_D("DELETE_ALL");
    px_gt511_tx_cmd_packet(PX_GT511_CMD_DELETE_ALL, 0);
    // Receive response
    if(!px_gt511_rx_resp_packet(PX_SYSTMR_MS_TO_TICKS(PX_GT511_DELETE_TIMEOUT_MS)))
    {
        return PX_GT511_ERR_TIMEOUT;
    }
    // Received NACK?
    if(px_gt511_resp_packet.resp == PX_GT511_CMD_NACK)
    {
        // Return error code
        return (px_gt511_err_t)px_gt511_resp_packet.param;
    } 
    // Success
    return PX_GT511_ERR_NONE;
}

px_gt511_err_t px_gt511_verify(px_gt511_id_t id)
{
    // Send command
    PX_LOG_D("VERIFY");
    px_gt511_tx_cmd_packet(PX_GT511_CMD_VERIFY, id);
    // Receive response
    if(!px_gt511_rx_resp_packet(PX_SYSTMR_MS_TO_TICKS(PX_GT511_ID_TIMEOUT_MS)))
    {
        return PX_GT511_ERR_TIMEOUT;
    }
    // Received NACK?
    if(px_gt511_resp_packet.resp == PX_GT511_CMD_NACK)
    {
        // Return error code
        return (px_gt511_err_t)px_gt511_resp_packet.param;
    } 
    // Success
    return PX_GT511_ERR_NONE;
}

px_gt511_err_t px_gt511_identify(px_gt511_id_t * id)
{
    // Send command
    PX_LOG_D("IDENTIFY");
    px_gt511_tx_cmd_packet(PX_GT511_CMD_IDENTIFY, 0);
    // Receive response
    if(!px_gt511_rx_resp_packet(PX_SYSTMR_MS_TO_TICKS(PX_GT511_ID_TIMEOUT_MS)))
    {
        return PX_GT511_ERR_TIMEOUT;
    }
    // Received NACK?
    if(px_gt511_resp_packet.resp == PX_GT511_CMD_NACK)
    {
        // Return error code
        return (px_gt511_err_t)px_gt511_resp_packet.param;
    } 
    // Return identified ID
    *id = px_gt511_resp_packet.param;
    // Success
    return PX_GT511_ERR_NONE;
}

px_gt511_err_t px_gt511_verify_template(px_gt511_id_t id, uint8_t * template_buf)
{
    // Send command
    PX_LOG_D("VERIFY_TEMPLATE");
    px_gt511_tx_cmd_packet(PX_GT511_CMD_VERIFY_TEMPLATE, id);
    // Receive response
    if(!px_gt511_rx_resp_packet(PX_SYSTMR_MS_TO_TICKS(PX_GT511_RX_TIMEOUT_MS)))
    {
        return PX_GT511_ERR_TIMEOUT;
    }
    // Received NACK?
    if(px_gt511_resp_packet.resp == PX_GT511_CMD_NACK)
    {
        // Return error code
        return (px_gt511_err_t)px_gt511_resp_packet.param;
    }
    // Send template
    px_gt511_tx_data_packet(template_buf, PX_GT511_TEMPLATE_SIZE);
    // Receive response
    if(!px_gt511_rx_resp_packet(PX_SYSTMR_MS_TO_TICKS(PX_GT511_ID_TIMEOUT_MS)))
    {
        return PX_GT511_ERR_TIMEOUT;
    }
    // Received NACK?
    if(px_gt511_resp_packet.resp == PX_GT511_CMD_NACK)
    {
        // Return error code
        return (px_gt511_err_t)px_gt511_resp_packet.param;
    }
    // Success
    return PX_GT511_ERR_NONE;
}

px_gt511_err_t px_gt511_identify_template(px_gt511_id_t * id, uint8_t * template_buf)
{
    // Send command
    PX_LOG_D("IDENTIFY_TEMPLATE");
    px_gt511_tx_cmd_packet(PX_GT511_CMD_IDENTIFY_TEMPLATE, 0);
    // Receive response
    if(!px_gt511_rx_resp_packet(PX_SYSTMR_MS_TO_TICKS(PX_GT511_RX_TIMEOUT_MS)))
    {
        return PX_GT511_ERR_TIMEOUT;
    }
    // Received NACK?
    if(px_gt511_resp_packet.resp == PX_GT511_CMD_NACK)
    {
        // Return error code
        return (px_gt511_err_t)px_gt511_resp_packet.param;
    }
    // Send template
    px_gt511_tx_data_packet(template_buf, PX_GT511_TEMPLATE_SIZE);
    // Receive response
    if(!px_gt511_rx_resp_packet(PX_SYSTMR_MS_TO_TICKS(PX_GT511_ID_TIMEOUT_MS)))
    {
        return PX_GT511_ERR_TIMEOUT;
    }
    // Received NACK?
    if(px_gt511_resp_packet.resp == PX_GT511_CMD_NACK)
    {
        // Return error code
        return (px_gt511_err_t)px_gt511_resp_packet.param;
    }
    // Return identified ID
    *id = px_gt511_resp_packet.param;
    // Success
    return PX_GT511_ERR_NONE;
}

px_gt511_err_t px_gt511_capture_finger(bool best_image)
{
    // Send command
    PX_LOG_D("CAPTURE_FINGER");
    px_gt511_tx_cmd_packet(PX_GT511_CMD_CAPTURE_FINGER, (best_image == false)? 0 : 1);
    // Receive response
    if(!px_gt511_rx_resp_packet(PX_SYSTMR_MS_TO_TICKS(PX_GT511_IMAGE_TIMEOUT_MS)))
    {
        return PX_GT511_ERR_TIMEOUT;
    }
    // Received NACK?
    if(px_gt511_resp_packet.resp == PX_GT511_CMD_NACK)
    {
        // Return error code
        return (px_gt511_err_t)px_gt511_resp_packet.param;
    } 
    // Success
    return PX_GT511_ERR_NONE;
}

px_gt511_err_t px_gt511_make_template(uint8_t * template_buf)
{
    // Send command
    PX_LOG_D("MAKE_TEMPLATE");
    px_gt511_tx_cmd_packet(PX_GT511_CMD_MAKE_TEMPLATE, 0);
    // Receive response
    if(!px_gt511_rx_resp_packet(PX_SYSTMR_MS_TO_TICKS(PX_GT511_RX_TIMEOUT_MS)))
    {
        return PX_GT511_ERR_TIMEOUT;
    }
    // Received NACK?
    if(px_gt511_resp_packet.resp == PX_GT511_CMD_NACK)
    {
        // Return error code
        return (px_gt511_err_t)px_gt511_resp_packet.param;
    }
    // Receive template
    if(!px_gt511_rx_data_packet(template_buf, PX_GT511_TEMPLATE_SIZE, PX_SYSTMR_MS_TO_TICKS(PX_GT511_IMAGE_TIMEOUT_MS)))
    {
        return PX_GT511_ERR_TIMEOUT;
    }
    // Success
    return PX_GT511_ERR_NONE;
}

px_gt511_err_t px_gt511_get_image(uint8_t * image_buf)
{
    // Send command
    PX_LOG_D("GET_IMAGE");
    px_gt511_tx_cmd_packet(PX_GT511_CMD_GET_IMAGE, 0);
    // Receive response
    if(!px_gt511_rx_resp_packet(PX_SYSTMR_MS_TO_TICKS(PX_GT511_RX_TIMEOUT_MS)))
    {
        return PX_GT511_ERR_TIMEOUT;
    }
    // Received NACK?
    if(px_gt511_resp_packet.resp == PX_GT511_CMD_NACK)
    {
        // Return error code
        return (px_gt511_err_t)px_gt511_resp_packet.param;
    }
    // Receive image
    if(!px_gt511_rx_data_packet(image_buf, PX_GT511_IMAGE_SIZE, PX_SYSTMR_MS_TO_TICKS(PX_GT511_RX_TIMEOUT_MS)))
    {
        return PX_GT511_ERR_TIMEOUT;
    }
    // Success
    return PX_GT511_ERR_NONE;
}

px_gt511_err_t px_gt511_get_raw_image(uint8_t * image_buf)
{
    // Send command
    PX_LOG_D("GET_RAW_IMAGE");
    px_gt511_tx_cmd_packet(PX_GT511_CMD_GET_RAW_IMAGE, 0);
    // Receive response
    if(!px_gt511_rx_resp_packet(PX_SYSTMR_MS_TO_TICKS(PX_GT511_RX_TIMEOUT_MS)))
    {
        return PX_GT511_ERR_TIMEOUT;
    }
    // Received NACK?
    if(px_gt511_resp_packet.resp == PX_GT511_CMD_NACK)
    {
        // Return error code
        return (px_gt511_err_t)px_gt511_resp_packet.param;
    }
    // Receive image
    if(!px_gt511_rx_data_packet(image_buf, PX_GT511_IMAGE_SIZE, PX_SYSTMR_MS_TO_TICKS(PX_GT511_RX_TIMEOUT_MS)))
    {
        return PX_GT511_ERR_TIMEOUT;
    }
    // Success
    return PX_GT511_ERR_NONE;
}

px_gt511_err_t px_gt511_get_template(px_gt511_id_t id, uint8_t * template_buf)
{
    // Send command
    PX_LOG_D("GET_TEMPLATE");
    px_gt511_tx_cmd_packet(PX_GT511_CMD_GET_TEMPLATE, id);
    // Receive response
    if(!px_gt511_rx_resp_packet(PX_SYSTMR_MS_TO_TICKS(PX_GT511_RX_TIMEOUT_MS)))
    {
        return PX_GT511_ERR_TIMEOUT;
    }
    // Received NACK?
    if(px_gt511_resp_packet.resp == PX_GT511_CMD_NACK)
    {
        // Return error code
        return (px_gt511_err_t)px_gt511_resp_packet.param;
    }
    // Receive template
    if(!px_gt511_rx_data_packet(template_buf, PX_GT511_TEMPLATE_SIZE, PX_SYSTMR_MS_TO_TICKS(PX_GT511_RX_TIMEOUT_MS)))
    {
        return PX_GT511_ERR_TIMEOUT;
    }
    // Success
    return PX_GT511_ERR_NONE;
}

px_gt511_err_t px_gt511_set_template(px_gt511_id_t id, uint8_t * template_buf)
{
    // Send command
    PX_LOG_D("SET_TEMPLATE");
    px_gt511_tx_cmd_packet(PX_GT511_CMD_SET_TEMPLATE, id);
    // Receive response
    if(!px_gt511_rx_resp_packet(PX_SYSTMR_MS_TO_TICKS(PX_GT511_RX_TIMEOUT_MS)))
    {
        return PX_GT511_ERR_TIMEOUT;
    }
    // Received NACK?
    if(px_gt511_resp_packet.resp == PX_GT511_CMD_NACK)
    {
        // Return error code
        return (px_gt511_err_t)px_gt511_resp_packet.param;
    }
    // Send template
    px_gt511_tx_data_packet(template_buf, PX_GT511_TEMPLATE_SIZE);
    // Receive response
    if(!px_gt511_rx_resp_packet(PX_SYSTMR_MS_TO_TICKS(PX_GT511_RX_TIMEOUT_MS)))
    {
        return PX_GT511_ERR_TIMEOUT;
    }
    // Received NACK?
    if(px_gt511_resp_packet.resp == PX_GT511_CMD_NACK)
    {
        // Return error code
        return (px_gt511_err_t)px_gt511_resp_packet.param;
    }
    // Success
    return PX_GT511_ERR_NONE;
}
