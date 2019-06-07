/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2012-2016 Pieter Conradie <https://piconomix.com>
    
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.
    
    Title:          px_gpio.h : Microchip AVR GPIO driver
    Author(s):      Pieter Conradie
    Creation Date:  2014-12-01

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_gpio.h"
#include "px_dbg.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_DBG_DECL_NAME("gpio");

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

