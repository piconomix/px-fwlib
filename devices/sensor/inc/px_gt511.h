#ifndef __PX_GT511_H__
#define __PX_GT511_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
 
    Title:          px_gt511.h : ADH Tech GT-511 Fingerprint sensor driver
    Author(s):      Pieter Conradie
    Creation Date:  2018-07-17

============================================================================= */

/** 
 *  @ingroup DEVICES_SENSOR
 *  @defgroup PX_GT511 px_gt511.h : ADH Tech GT-511 Fingerprint sensor driver
 *  
 *  This is a driver for the ADH Tech GT-511 Fingerprint sensor. It has been 
 *  tested on the GT-511C1R and GT-511C3 sensor. 
 *  
 *  File(s):
 *  - devices/sensor/inc/px_gt511.h
 *  - devices/sensor/src/px_gt511.c
 *  
 *  Reference: 
 *  - ADH Tech [GT-511C1R] (http://www.adh-tech.com.tw/?21,gt-511c1r-gt-511c11-%28uart%29) sensor datasheet 
 *  - ADH Tech [GT-511C3] (http://www.adh-tech.com.tw/?22,gt-511c3-gt-511c5-%28uart%29) sensor datasheet 
 *  
 *  Some operations take long to complete. To capture a finger image using 
 *  `px_gt511_capture_finger()` takes up to 250 ms. Each enroll step e.g. 
 *  `px_gt511_enroll_1()` takes up to 730 ms. To identify a finger using 
 *  `px_gt511_identify()` takes 715 ms.
 *   
 *  Example:
 *  
 *  @include devices/sensor/test/px_gt511_test.c
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_uart.h"
#include "px_systmr.h"

#ifdef __cplusplus
extern "C"
{
#endif
/* _____DEFINITIONS__________________________________________________________ */
#define PX_GT511_UART_DEFAULT_BAUD     9600
#define PX_GT511_UART_MAX_BAUD         115200
#define PX_GT511_TEMPLATE_SIZE         506
#define PX_GT511_IMAGE_SIZE_X          240
#define PX_GT511_IMAGE_SIZE_Y          216
#define PX_GT511_IMAGE_SIZE            (PX_GT511_IMAGE_SIZE_X * PX_GT511_IMAGE_SIZE_Y)

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// GT511 Error codes
typedef enum
{
    PX_GT511_ERR_NONE                  = 0,
    PX_GT511_ERR_TIMEOUT               = 0x1001,   ///< Timeout
    PX_GT511_ERR_INVALID_BAUD          = 0x1002,   ///< Invalid serial baud rate
    PX_GT511_ERR_INVALID_POS           = 0x1003,   ///< The specified ID is not in range
    PX_GT511_ERR_IS_NOT_USED           = 0x1004,   ///< The specified ID is not used
    PX_GT511_ERR_IS_ALREADY_USED       = 0x1005,   ///< The specified ID is already used
    PX_GT511_ERR_COMM_ERR              = 0x1006,   ///< Communication Error
    PX_GT511_ERR_VERIFY_FAILED         = 0x1007,   ///< 1:1 Verification Failure
    PX_GT511_ERR_IDENTIFY_FAILED       = 0x1008,   ///< 1:N Identification Failure
    PX_GT511_ERR_DB_IS_FULL            = 0x1009,   ///< The database is full
    PX_GT511_ERR_DB_IS_EMPTY           = 0x100A,   ///< The database is empty
    PX_GT511_ERR_TURN_ERR              = 0x100B,   ///< Invalid order of the enrollment
    PX_GT511_ERR_BAD_FINGER            = 0x100C,   ///< Too bad fingerprint
    PX_GT511_ERR_ENROLL_FAILED         = 0x100D,   ///< Enrollment Failure
    PX_GT511_ERR_IS_NOT_SUPPORTED      = 0x100E,   ///< The specified command is not supported
    PX_GT511_ERR_DEV_ERR               = 0x100F,   ///< Device Error, especially if Crypto-Chip is trouble
    PX_GT511_ERR_CAPTURE_CANCELLED     = 0x1010,   ///< The capturing is cancelled
    PX_GT511_ERR_INVALID_PARAM         = 0x1011,   ///< Invalid parameter
    PX_GT511_ERR_FINGER_IS_NOT_PRESSED = 0x1012,   ///< Finger is not pressed
} px_gt511_err_t;

/// GT511 Device Info
typedef struct
{
    uint32_t fw_version;
    uint32_t iso_area_max_size;
    uint8_t  device_serial_number[16];
} px_gt511_dev_info_t;

/// Size definition of ID
typedef uint8_t px_gt511_id_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
void           px_gt511_init                      (px_uart_handle_t * handle);
px_gt511_err_t px_gt511_auto_baud                 (void);
px_gt511_err_t px_gt511_open                      (bool get_device_info, 
                                                   px_gt511_dev_info_t * dev_info);
px_gt511_err_t px_gt511_close                     (void);
px_gt511_err_t px_gt511_cmos_led_control          (bool on);
px_gt511_err_t px_gt511_change_baud_rate          (uint32_t baud);
px_gt511_err_t px_gt511_get_enroll_count          (px_gt511_id_t * enroll_count);
px_gt511_err_t px_gt511_check_enrolled            (px_gt511_id_t id);
px_gt511_err_t px_gt511_enroll_start              (px_gt511_id_t id);
px_gt511_err_t px_gt511_enroll_1                  (void);
px_gt511_err_t px_gt511_enroll_2                  (void);
px_gt511_err_t px_gt511_enroll_3                  (void);
px_gt511_err_t px_gt511_is_finger_pressed         (bool * is_pressed);
px_gt511_err_t px_gt511_wait_until_finger_pressed (px_systmr_ticks_t timeout_ticks);
px_gt511_err_t px_gt511_wait_until_finger_released(px_systmr_ticks_t timeout_ticks);
px_gt511_err_t px_gt511_delete_id                 (px_gt511_id_t id);
px_gt511_err_t px_gt511_delete_all                (void);
px_gt511_err_t px_gt511_verify                    (px_gt511_id_t id);
px_gt511_err_t px_gt511_identify                  (px_gt511_id_t * id);
px_gt511_err_t px_gt511_verify_template           (px_gt511_id_t id, uint8_t * template_buf);
px_gt511_err_t px_gt511_identify_template         (px_gt511_id_t * id, uint8_t * template_buf);
px_gt511_err_t px_gt511_capture_finger            (bool best_image);
px_gt511_err_t px_gt511_make_template             (uint8_t * template_buf);
px_gt511_err_t px_gt511_get_image                 (uint8_t * image_buf);
px_gt511_err_t px_gt511_get_raw_image             (uint8_t * image_buf);
px_gt511_err_t px_gt511_get_template              (px_gt511_id_t id, uint8_t * template_buf);
px_gt511_err_t px_gt511_set_template              (px_gt511_id_t id, uint8_t * template_buf);

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif
