#ifndef __PX_GPIO_H__
#define __PX_GPIO_H__
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
    Creation Date:  2012-06-21

============================================================================= */

/** 
 *  @ingroup AVR
 *  @defgroup AVR_GPIO px_gpio.h : Microchip AVR GPIO driver
 *  
 *  A collection of macros and functions that simplifies defining and using
 *  General Purpose I/O pins.
 *  
 *  File(s):
 *  - arch/avr/inc/px_gpio.h
 *  - arch/avr/src/px_gpio.c
 *  
 *  @warn_s
 *  Use a minimum compiler optimization level of 1, otherwise code bloat will
 *  occur. Even an innocuous C statement such as "PORTB |= (1 << 2);" will result
 *  in several redundant assembly instructions at -O0.
 *  @warn_e
 *  
 *  A GPIO pin is defined using the #PX_GPIO utility macro, e.g.
 *  
 *  @code{.c}
 *      // LED is an output pin on PORTB, bit 2, initially off
 *      #define PX_GPIO_LED PX_GPIO(B, 2, PX_GPIO_DIR_OUT, PX_GPIO_INIT_LO)
 *  @endcode
 *  
 *  Now wherever PX_GPIO_LED is found, the C preprocessor will substitute it 
 *  with the following sequence:
 *  
 *  @code{.c}
 *      &PORTB, &DDRB, &PINB, 2, PX_GPIO_DIR_OUT, PX_GPIO_INIT_LO
 *  @endcode
 *  
 *  There are 3 ways to use this GPIO driver:
 *  
 *  The first way is to use macros that utilize the C preprocessor to resolve to
 *  optimal C code before it is fed to the compiler. The disadvantage of this
 *  approach is the lack of syntax checking and the obtuse error message that
 *  the compiler may generate. The recursive macro magic is demonstrated
 *  with an example:
 *  
 *  The following code:
 *  
 *  @code{.c}
 *      // Enable LED
 *      PX_GPIO_OUT_SET_HI(PX_GPIO_LED);
 *  @endcode
 *  
 *  Expands to:
 *  
 *  @code{.c}
 *      _PX_GPIO_OUT_SET_HI(&PORTB, &DDRB, &PINB, 2, PX_GPIO_DIR_OUT, PX_GPIO_INIT_LO);
 *  @endcode
 *  
 *  Which reduces to:
 *  
 *  @code{.c}
 *      PX_BIT_SET_HI(*(&PORTB), 2);
 *  @endcode
 *  
 *  Which reduces even further to:
 *  
 *  @code{.c}
 *      do {PORTB |= (1 << 2);} while(0);
 *  @endcode
 *  
 *  Which the optimizing compiler replaces with a single assembly statement:
 *  
 *  @code
 *      sbi 0x05, 2;
 *  @endcode
 *  
 *  Which enables the LED :)
 *  
 *  Example:
 *  @include arch/avr/test/px_gpio_test.c
 *  
 *  Even though it *appears* that functions calls are used, the lines reduce to 
 *  single assembly statements for maximum efficiency and minimum code size:
 *  
 *  @code
 *      // Initialise pins
 *      PX_GPIO_INIT(PX_GPIO_LED);
 *      7a: 28 98           cbi 0x05, 0 ; 5
 *      7c: 20 9a           sbi 0x04, 0 ; 4
 *      PX_GPIO_INIT(PX_GPIO_PB);
 *      7e: 5f 9a           sbi 0x0b, 7 ; 11
 *      80: 57 98           cbi 0x0a, 7 ; 10
 *
 *      // Loop forever
 *      while(true)
 *      {
 *          // Is button being pressed?
 *          if(PX_GPIO_IN_IS_LO(PX_GPIO_PB))
 *      82: 4f 99           sbic    0x09, 7 ; 9
 *      84: 02 c0           rjmp    .+4         ; 0x8a <main+0x10>
 *          {
 *              // Enable LED
 *              PX_GPIO_OUT_SET_HI(PX_GPIO_LED);
 *      86: 28 9a           sbi 0x05, 0 ; 5
 *      88: fc cf           rjmp    .-8         ; 0x82 <main+0x8>
 *          }
 *          else
 *          {
 *              // Disable LED
 *              PX_GPIO_OUT_SET_LO(PX_GPIO_LED);
 *      8a: 28 98           cbi 0x05, 0 ; 5
 *      8c: fa cf           rjmp    .-12        ; 0x82 <main+0x8>
 *  @endcode
 *  
 *  The second method is to declare the GPIO pin info as a 'static const'
 *  structure in an H file and use inline C functions. If the compiler
 *  optimisation is enabled then the code will also resolve to (mostly) single
 *  assembly statements during compile time.
 *  
 *  @warn_s
 *  Don't forget to prefix 'static const'! The compiler must know that the
 *  GPIO pin data will never change so that it can make the best optimization
 *  decisions.
 *  @warn_e
 *  
 *  Example (in an H file):
 *  @code{.c}
 *      // LED is an output pin on PORTB, bit 2, initially off
 *      #define PX_GPIO_LED PX_GPIO(B, 2, PX_GPIO_DIR_OUT, PX_GPIO_INIT_LO)
 *      static const px_gpio_t px_gpio_led = {PX_GPIO_LED};
 *
 *      // Inline function to enable the LED
 *      static inline void led_enable(void)
 *      {
 *          px_gpio_out_set_hi(&px_gpio_led);
 *      }
 *  @endcode
 *  
 *  Example:
 *  @include arch/avr/test/px_gpio_test2.c
 *  
 *  The third method is to declare a GPIO pin as a dynamic structure that can
 *  change by the code while it is executing. This is useful where GPIO pin
 *  mapping must be changed on the fly.
 *  
 *  Example (in a C file): 
 *   
 *  @code{.c}
 *      // LED is an output pin on PORTB, bit 2, initially off
 *      #define PX_GPIO_LED PX_GPIO(B, 2, PX_GPIO_DIR_OUT, PX_GPIO_INIT_LO)
 *
 *      // Declare LED gpio structure
 *      px_gpio_t px_gpio_led;
 *
 *      void led_init(bool led_on)
 *      {
 *          // LED is an output pin on PORTB, bit 2, initially off
 *          px_gpio_open(&px_gpio_led, PX_GPIO_LED);
 *          // Initialise LED pin
 *          px_gpio_init(&px_gpio_led);
 *          if(led_on)
 *          {
 *              // Enable LED
 *              px_gpio_out_set_hi(&px_gpio_led);
 *          }
 *      }
 *  @endcode
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <avr/io.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// GPIO dir selection
typedef enum
{
    PX_GPIO_DIR_IN  = 0,        ///< Set pin direction to input
    PX_GPIO_DIR_OUT = 1,        ///< Set pin direction to output
} px_gpio_dir_t;

/// GPIO init selection
typedef enum
{
    PX_GPIO_INIT_LO =      0,   ///< Initialize output pin low (0)
    PX_GPIO_INIT_HI =      1,   ///< Initialize output pin high (1)
    PX_GPIO_INIT_HIZ =     0,   ///< Initialize input pin as high impedance (no pull-up)
    PX_GPIO_INIT_PULL_UP = 1,   ///< Initialize input pin with pull-up enabled
} px_gpio_init_t;

/// GPIO peripheral register address definition
typedef volatile uint8_t * px_gpio_reg_t;

/// GPIO pin handle definition
typedef struct
{
    px_gpio_reg_t  port;        ///< PORTx register address
    px_gpio_reg_t  ddr;         ///< DDRx register address
    px_gpio_reg_t  pin;         ///< PINx register address
    uint8_t        bit;         ///< Bit: 0, 1, 2, 3, 4, 5, 6 or 7
    uint8_t        dir;         ///< Direction: PX_GPIO_DIR_OUT or PX_GPIO_DIR_IN
    uint8_t        init;        ///< Initial value: PX_GPIO_INIT_HI, PX_GPIO_INIT_LO, PX_GPIO_INIT_HIZ or PX_GPIO_INIT_PULL_UP
} px_gpio_handle_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____MACROS_______________________________________________________________ */
/** 
 *  Helper macro to define a GPIO pin.
 *  
 *  Usage example:
 *  
 *  @code{.c}
 *      // LED is an output pin on PORTD, bit 7, initially off
 *      #define PX_GPIO_LED    GPIO(D, 7, PX_GPIO_DIR_OUT, PX_GPIO_INIT_LO)
 *
 *      // PX_GPIO_LED will be replaced by the C preprocessor with the following
 *      // sequence:
 *      // &PORTD, &DDRD, &PIND, 7, PX_GPIO_DIR_OUT, PX_GPIO_INIT_LO
 *  @endcode
 *  
 *  @param port      A, B, C, ...
 *  @param bit       0, 1, 2, 3, 4, 5, 6 or 7
 *  @param dir       PX_GPIO_DIR_OUT or PX_GPIO_DIR_IN
 *  @param init      PX_GPIO_INIT_HI, PX_GPIO_INIT_LO, PX_GPIO_INIT_HIZ or PX_GPIO_INIT_PULL_UP
 */
#define PX_GPIO(port, bit, dir, init) \
    &PORT ## port, &DDR ## port, &PIN ## port, bit, dir, init

/// Initialise a GPIO pin
#define PX_GPIO_INIT(gpio) _GPIO_INIT(gpio)
#define _GPIO_INIT(port, ddr, pin, bit, dir, init) \
do \
{ \
    if(init == PX_GPIO_INIT_LO) \
    { \
        PX_BIT_SET_LO(*(port), bit); \
    } \
    else \
    { \
        PX_BIT_SET_HI(*(port), bit); \
    } \
    if(dir == PX_GPIO_DIR_IN) \
    { \
        PX_BIT_SET_LO(*(ddr), bit); \
    } \
    else \
    { \
        PX_BIT_SET_HI(*(ddr), bit); \
    } \
} \
while(0)

/// Set GPIO pin output high
#define PX_GPIO_OUT_SET_HI(gpio) _GPIO_OUT_SET_HI(gpio)
#define _GPIO_OUT_SET_HI(port, ddr, pin, bit, dir, init) PX_BIT_SET_HI(*port, bit)

/// Set GPIO pin output low
#define PX_GPIO_OUT_SET_LO(gpio) _GPIO_PIN_OUT_SET_LO(gpio)
#define _GPIO_PIN_OUT_SET_LO(port, ddr, pin, bit, dir, init) PX_BIT_SET_LO(*port, bit)

/// Toggle GPIO pin output
#define PX_GPIO_OUT_TOGGLE(gpio) _GPIO_OUT_TOGGLE(gpio)
#define _GPIO_OUT_TOGGLE(port, ddr, pin, bit, dir, init) PX_BIT_TOGGLE(*port, bit)

/// Test if GPIO pin output is set high
#define PX_GPIO_OUT_IS_HI(gpio) _GPIO_OUT_IS_HI(gpio)
#define _GPIO_OUT_IS_HI(port, ddr, pin, bit, dir, init) PX_BIT_IS_HI(*port, bit)

/// Test if GPIO pin output is set low
#define PX_GPIO_OUT_IS_LO(gpio) _GPIO_OUT_IS_LO(gpio)
#define _GPIO_OUT_IS_LO(port, ddr, pin, bit, dir, init) PX_BIT_IS_LO(*port, bit)

/// Test if GPIO pin input is high
#define PX_GPIO_IN_IS_HI(gpio) _GPIO_IN_IS_HI(gpio)
#define _GPIO_IN_IS_HI(port, ddr, pin, bit, dir, init) PX_BIT_IS_HI(*pin, bit)

/// Test if GPIO pin input is low
#define PX_GPIO_IN_IS_LO(gpio) _GPIO_IN_IS_LO(gpio)
#define _GPIO_IN_IS_LO(port, ddr, pin, bit, dir, init) PX_BIT_IS_LO(*pin, bit)

/// Set GPIO pin direction to output
#define PX_GPIO_DIR_SET_OUT(gpio) _GPIO_DIR_SET_OUT(gpio)
#define _GPIO_DIR_SET_OUT(port, ddr, pin, bit, dir, init) PX_BIT_SET_HI(*ddr, bit)

/// Set GPIO pin direction to input
#define PX_GPIO_DIR_SET_IN(gpio) _GPIO_DIR_SET_IN(gpio)
#define _GPIO_DIR_SET_IN(port, ddr, pin, bit, dir, init) PX_BIT_SET_LO(*ddr, bit)

/// Test if GPIO pin is configured to be an output
#define PX_GPIO_DIR_IS_OUT(gpio) _GPIO_DIR_IS_OUT(gpio)
#define _GPIO_DIR_IS_OUT(port, ddr, pin, bit, dir, init) PX_BIT_IS_HI(*ddr, bit)

/// Test if GPIO pin is configured to be an input
#define PX_GPIO_DIR_IS_IN(gpio) _GPIO_DIR_IS_IN(gpio)
#define _GPIO_DIR_IS_IN(port, ddr, pin, bit, dir, init) PX_BIT_IS_LO(*ddr, bit)

/// Enable pull-up on GPIO pin
#define PX_GPIO_PULL_UP_ENABLE(gpio) _GPIO_PULL_UP_ENABLE(gpio)
#define _GPIO_PULL_UP_ENABLE(port, ddr, pin, bit, dir, init) PX_BIT_SET_HI(*port, bit)

/// Disable pull-up on GPIO pin
#define PX_GPIO_PULL_DISABLE(gpio) _GPIO_PULL_DISABLE(gpio)
#define _GPIO_PULL_DISABLE(port, ddr, pin, bit, dir, init) PX_BIT_SET_LO(*port, bit)

/// Macro to calculate bit mask used to initialise DDR register
#define PX_GPIO_INIT_DDR(gpio) _GPIO_INIT_DDR(gpio)
#define _GPIO_INIT_DDR(port, ddr, pin, bit, dir, init) (dir << bit)

/// Macro to calculate bit mask used to initialise PORT register
#define PX_GPIO_INIT_PORT(gpio) _GPIO_INIT_PORT(gpio)
#define _GPIO_INIT_PORT(port, ddr, pin, bit, dir, init) (init << bit)

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise a GPIO structure.
 *  
 *  @param [out] gpio   px_gpio_t structure
 *  @param [in] port    PORTx register address, e.g. &PORTB
 *  @param [in] ddr     DDRx register address, e.g. &DDRB
 *  @param [in] pin     PINx register address, e.g. &PINB
 *  @param [in] bit     Bit: 0, 1, 2, 3, 4, 5, 6 or 7
 *  @param [in] dir     Direction: PX_GPIO_DIR_OUT or PX_GPIO_DIR_IN
 *  @param [in] init    Initial value: PX_GPIO_INIT_HI, PX_GPIO_INIT_LO, PX_GPIO_INIT_HIZ or PX_GPIO_INIT_PULL_UP
 *  
 *  Example:
 *  
 *  @code{.c}
 *      px_gpio_t px_gpio_led;
 *
 *      px_gpio_init(&px_gpio_led, &PORTD, &DDRD, &PIND, 7, PX_GPIO_DIR_OUT, PX_GPIO_INIT_LO);
 *  @endcode
 *  
 */
inline void px_gpio_open(px_gpio_handle_t * gpio, 
                         px_gpio_reg_t      port,
                         px_gpio_reg_t      ddr,
                         px_gpio_reg_t      pin,
                         uint8_t            bit,
                         uint8_t            dir,
                         uint8_t            init)
{
    gpio->port = port;
    gpio->ddr  = ddr;
    gpio->pin  = pin;
    gpio->bit  = bit;
    gpio->dir  = dir;
    gpio->init = init;
}

/// Initialise a GPIO pin
inline void px_gpio_init(const px_gpio_handle_t * gpio)
{
    if(gpio->init == PX_GPIO_INIT_LO)
    {
        PX_BIT_SET_LO(*(gpio->port), gpio->bit);
    }
    else
    {
        PX_BIT_SET_HI(*(gpio->port), gpio->bit);
    }
    if(gpio->dir == PX_GPIO_DIR_IN)
    {
        PX_BIT_SET_LO(*(gpio->ddr), gpio->bit);
    }
    else
    {
        PX_BIT_SET_HI(*(gpio->ddr), gpio->bit);
    }
}

/// Calculate bit mask used to initialise DDR register
inline uint8_t px_gpio_init_ddr(const px_gpio_handle_t * gpio)
{
    return ((gpio->dir) << (gpio->bit));
}

/// Calculate bit mask used to initialise PORT register
inline uint8_t px_gpio_init_port(const px_gpio_handle_t * gpio)
{
    return ((gpio->init) << (gpio->bit));
}

/// Set GPIO pin output high
inline void px_gpio_out_set_hi(const px_gpio_handle_t * gpio)
{
    PX_BIT_SET_HI(*(gpio->port), gpio->bit);
}

/// Set GPIO pin output low
inline void px_gpio_out_set_lo(const px_gpio_handle_t * gpio)
{
    PX_BIT_SET_LO(*(gpio->port), gpio->bit);
}

/// Toggle GPIO pin output
inline void px_gpio_out_toggle(const px_gpio_handle_t * gpio)
{
    PX_BIT_TOGGLE(*(gpio->port), gpio->bit);
}

/// Test if GPIO pin output is set high
inline bool px_gpio_out_is_hi(const px_gpio_handle_t * gpio)
{
    return PX_BIT_IS_HI(*(gpio->port), gpio->bit);
}

/// Test if GPIO pin output is set low
inline bool px_gpio_out_is_lo(const px_gpio_handle_t * gpio)
{
    return PX_BIT_IS_LO(*(gpio->port), gpio->bit);
}

/// Test if GPIO pin input is high
inline bool px_gpio_in_is_hi(const px_gpio_handle_t * gpio)
{
    return PX_BIT_IS_HI(*(gpio->pin), gpio->bit);
}

/// Test if GPIO pin input is low
inline bool px_gpio_in_is_lo(const px_gpio_handle_t * gpio)
{
    return PX_BIT_IS_LO(*(gpio->pin), gpio->bit);
}

/// Set GPIO pin direction to output
inline void px_gpio_dir_set_out(const px_gpio_handle_t * gpio)
{
    PX_BIT_SET_HI(*(gpio->ddr), gpio->bit);
}

/// Set GPIO pin direction to input
inline void px_gpio_dir_set_in(const px_gpio_handle_t * gpio)
{
    PX_BIT_SET_LO(*(gpio->ddr), gpio->bit);
}

/// Test if GPIO pin is configured to be an output
inline bool px_gpio_dir_is_out(const px_gpio_handle_t * gpio)
{
    return PX_BIT_IS_HI(*(gpio->ddr), gpio->bit);
}

/// Test if GPIO pin is configured to be an input
inline bool px_gpio_dir_is_in(const px_gpio_handle_t * gpio)
{
    return PX_BIT_IS_LO(*(gpio->ddr), gpio->bit);
}

/// Enable pull-up on GPIO pin
inline void px_gpio_pull_up_enable(const px_gpio_handle_t * gpio)
{
    PX_BIT_SET_HI(*(gpio->port), gpio->bit);
}

/// Disable pull-up on GPIO pin
inline void px_gpio_pull_up_disable(const px_gpio_handle_t * gpio)
{
    PX_BIT_SET_LO(*(gpio->port), gpio->bit);
}

#ifdef __cplusplus
}
#endif

/// @}
#endif
