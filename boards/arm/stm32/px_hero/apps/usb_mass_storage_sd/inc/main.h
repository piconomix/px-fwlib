#ifndef __MAIN_H__
#define __MAIN_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          Piconomix STM32 Hero Board USB Mass Storage Device example
    Author(s):      Pieter Conradie
    Creation Date:  2019-05-24

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */
#define MAIN_BUFFER_SIZE 512

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
extern uint8_t  main_buffer[MAIN_BUFFER_SIZE];
extern uint32_t main_sd_capacity_blocks;

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
void main_usb_event_connected(void);
void main_log_putchar        (char data);
void main_log_timestamp      (char * str);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif

#endif
