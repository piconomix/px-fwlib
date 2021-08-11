/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2012-2016 Pieter Conradie <https://piconomix.com>
    
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_gpio.h : Microchip AVR GPIO driver
    Author(s):      Pieter Conradie
    Creation Date:  2014-12-01

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_gpio.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("gpio");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
/*
    Declare "extern inline" functions so that if the compiler decides not to
    place the functions inline, normal versions of the functions will be created
    that can be called.
 */
extern inline void      px_gpio_open            (px_gpio_handle_t * gpio, 
                                                 px_gpio_reg_t      port,
                                                 px_gpio_reg_t      ddr,
                                                 px_gpio_reg_t      pin,
                                                 uint8_t            bit,
                                                 uint8_t            dir,
                                                 uint8_t            init);

extern inline void      px_gpio_pin_init        (const px_gpio_handle_t * gpio);
extern inline uint8_t   px_gpio_init_ddr        (const px_gpio_handle_t * gpio);
extern inline uint8_t   px_gpio_init_port       (const px_gpio_handle_t * gpio);

extern inline void      px_gpio_pin_set_hi      (const px_gpio_handle_t * gpio);
extern inline void      px_gpio_pin_set_lo      (const px_gpio_handle_t * gpio);
extern inline void      px_gpio_pin_toggle      (const px_gpio_handle_t * gpio);
                                                
extern inline bool      px_gpio_out_is_hi       (const px_gpio_handle_t * gpio);
extern inline bool      px_gpio_out_is_lo       (const px_gpio_handle_t * gpio);
                                                
extern inline bool      px_gpio_pin_is_hi       (const px_gpio_handle_t * gpio);
extern inline bool      px_gpio_pin_is_lo       (const px_gpio_handle_t * gpio);
                                                
extern inline void      px_gpio_dir_set_out     (const px_gpio_handle_t * gpio);
extern inline void      px_gpio_dir_set_in      (const px_gpio_handle_t * gpio);
                                                
extern inline bool      px_gpio_dir_is_out      (const px_gpio_handle_t * gpio);
extern inline bool      px_gpio_dir_is_in       (const px_gpio_handle_t * gpio);
                        
extern inline void      px_gpio_pullup_enable   (const px_gpio_handle_t * gpio);
extern inline void      px_gpio_pullup_disable  (const px_gpio_handle_t * gpio);

