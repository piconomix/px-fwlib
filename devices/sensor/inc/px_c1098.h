#ifndef __PX_C1098_H__
#define __PX_C1098_H__
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

/** 
 *  @ingroup DEVICES_SENSOR
 *  @defgroup PX_C1098 px_c1098.h : COMedia C1098 CMOS Camera Module driver
 *  
 *  File(s):
 *  - devices/sensor/inc/px_c1098.h
 *  - devices/sensor/src/px_c1098.c
 *  
 *  Reference:
 *  - [COMedia C1098](http://www.comedia.com.hk/FP13/Spec_PDF/PX_C1098_UM.pdf) CMOS Camera module
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_uart.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
#define PX_C1098_UART_DEFAULT_BAUD     14400

#define PX_C1098_IF_SPEED_14400_BPS    0x07
#define PX_C1098_IF_SPEED_28800_BPS    0x06
#define PX_C1098_IF_SPEED_57600_BPS    0x05
#define PX_C1098_IF_SPEED_115200_BPS   0x04
#define PX_C1098_IF_SPEED_230400_BPS   0x03
#define PX_C1098_IF_SPEED_460800_BPS   0x02

#define PX_C1098_JPEG_RES_QVGA         0x05
#define PX_C1098_JPEG_RES_VGA          0x07

#define PX_C1098_RST_PRIORITY_NORMAL   0x00
#define PX_C1098_RST_PRIORITY_HIGH     0xFF

#define PX_C1098_DATA_BLOCK_SIZE       128

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
/** 
 *  Definition for a pointer to a function that will be called once a block of
 *  data has been received.
 *  
 *  @param data             Pointer to structure containing received data
 *  @param bytes_received   Number of bytes in structure
 */
typedef void (*px_c1098_on_rx_data_t)(const uint8_t * data, uint16_t bytes_received);

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
void     px_c1098_init            (px_uart_handle_t * handle);
         
void     px_c1098_task            (void);
bool     px_c1098_busy            (void);
bool     px_c1098_error           (void);
bool     px_c1098_task_execute    (void);
         
void     px_c1098_initial         (uint8_t if_speed, uint8_t jpeg_res);
void     px_c1098_set_package_size(void);
void     px_c1098_reset           (uint8_t rst_priority);
void     px_c1098_sync            (void);
void     px_c1098_snapshot        (void);
uint32_t px_c1098_get_picture_size(void);
void     px_c1098_get_picture     (px_c1098_on_rx_data_t on_rx_data);


/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif
