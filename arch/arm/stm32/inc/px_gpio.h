#ifndef __PX_GPIO_H__
#define __PX_GPIO_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2017 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          px_gpio.h : ST STM32 GPIO driver
    Author(s):      Pieter Conradie
    Creation Date:  2017-11-16

============================================================================= */

/** 
 *  @ingroup STM32
 *  @defgroup STM32_GPIO px_gpio.h : GPIO peripheral driver
 *  
 *  Driver to communicate with GPIO slaves.
 *  
 *  File(s):
 *  - arch/arm/stm32/inc/px_gpio.h
 *  - arch/arm/stm32/inc/px_gpio_cfg_template.h
 *  - arch/arm/stm32/src/px_gpio.c
 *  
 *  The driver must be configured by supplying a project specific "px_gpio_cfg.h".
 *  "px_gpio_cfg_template.h" can be copied, renamed and modified to supply 
 *  compile time options.
 *  
 *  @par Example:
 *  @include arch/arm/stm32/test/px_gpio_test.c
 */
/// @{
/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_lib_stm32cube.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS__________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// GPIO mode selection; See GPIOx_MODER register
typedef enum
{
    PX_GPIO_MODE_IN         = 0,    ///< Set pin mode to input
    PX_GPIO_MODE_OUT        = 1,    ///< Set pin mode to output
    PX_GPIO_MODE_AF         = 2,    ///< Set pin mode to alternative function
    PX_GPIO_MODE_ANA        = 3,    ///< Set pin mode to analog
    PX_GPIO_MODE_DEF        = 3,    ///< Set pin mode to default (analog)
    PX_GPIO_MODE_BIT_MASK   = 0x3,  ///< Pin mode bit mask
} px_gpio_mode_t;

/// GPIO output type selection; See GPIOx_OTYPER register
typedef enum
{
    PX_GPIO_OTYPE_NA        = 0,    ///< Output type is not applicable (pin mode is input or analog)
    PX_GPIO_OTYPE_PP        = 0,    ///< Set output type to push-pull
    PX_GPIO_OTYPE_OD        = 1,    ///< Set output type to open-drain
    PX_GPIO_OTYPE_BIT_MASK  = 0x1,  ///< Output type bit mask
} px_gpio_otype_t;

/// GPIO output speed selection; See GPIOx_OSPEEDR register
typedef enum
{
    PX_GPIO_OSPEED_NA       = 0,    ///< Output speed is not applicable (pin mode is input or analog)
    PX_GPIO_OSPEED_LO       = 0,    ///< Set output speed to low
    PX_GPIO_OSPEED_MD       = 1,    ///< Set output speed to medium
    PX_GPIO_OSPEED_HI       = 2,    ///< Set output speed to high
    PX_GPIO_OSPEED_VH       = 3,    ///< Set output speed to very high
    PX_GPIO_OSPEED_BIT_MASK = 0x3,  ///< Output speed bit mask
} px_gpio_ospeed_t;

/// GPIO pull-up / pull-down selection; See GPIOx_PUPDR register
typedef enum
{
    PX_GPIO_PULL_NO         = 0,    ///< No pull-up / pull-down
    PX_GPIO_PULL_UP         = 1,    ///< Enable pull-up
    PX_GPIO_PULL_DN         = 2,    ///< Enable pull-down
    PX_GPIO_PULL_BIT_MASK   = 0x3,  ///< Pull-up / Pull-down bit mask
} px_gpio_pull_t;

/// GPIO initial output selection; See GPIOx_ODR register
typedef enum
{
    PX_GPIO_OUT_INIT_NA     = 0,    ///< Initialize output is not applicable (pin mode is input or analog)
    PX_GPIO_OUT_INIT_LO     = 0,    ///< Initialize output pin to low (0)
    PX_GPIO_OUT_INIT_HI     = 1,    ///< Initialize output pin to high (1)
} px_gpio_out_init_t;

/// GPIO alternate function selection; See GPIOx_AFRL and GPIOx_AFRH register
typedef enum
{
    PX_GPIO_AF_NA           = 0,    ///< Alternate Function is not applicable (pin mode is not alternate function)
    PX_GPIO_AF_0            = 0,    ///< Alternate Function 0
    PX_GPIO_AF_1            = 1,    ///< Alternate Function 1
    PX_GPIO_AF_2            = 2,    ///< Alternate Function 2
    PX_GPIO_AF_3            = 3,    ///< Alternate Function 3
    PX_GPIO_AF_4            = 4,    ///< Alternate Function 4
    PX_GPIO_AF_5            = 5,    ///< Alternate Function 5
    PX_GPIO_AF_6            = 6,    ///< Alternate Function 6
    PX_GPIO_AF_7            = 7,    ///< Alternate Function 7
    PX_GPIO_ALT_FN_BIT_MASK = 0xf,  ///< Alternate Function bit mask
} px_gpio_af_t;

/// GPIO pin handle definition
typedef struct
{
    GPIO_TypeDef *      gpio_base_reg;  ///< GPIO peripheral base register address
    uint8_t             pin;            ///< Pin: 0, 1, 2, ..., or 15
    px_gpio_mode_t      mode;           ///< Mode: Input, Output, Alternative Function or Analog
    px_gpio_otype_t     otype;          ///< Output type: push-pull or open-drain
    px_gpio_ospeed_t    ospeed;         ///< Output speed: low, medium, high or very high
    px_gpio_pull_t      pull;           ///< None, pull-up or pull-down
    px_gpio_out_init_t  out_init;       ///< Initial output value: Low (0) or high (1)
    px_gpio_af_t        af;             ///< Alternative function: AF0, AF1, ... or AF7
} px_gpio_handle_t;

typedef struct
{
    GPIO_TypeDef * gpio_base_reg;       ///< GPIO peripheral base register address
    uint32_t       moder;               ///< Mode register value
    uint32_t       otyper;              ///< Output type register value
    uint32_t       ospeedr;             ///< Output speed register value
    uint32_t       pupdr;               ///< Pull up / pull-down register value
    uint32_t       odr;                 ///< Output data register value
    uint32_t       afrl;                ///< Alternative function low register value
    uint32_t       afrh;                ///< Alternative function high register value
} px_gpio_port_init_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____MACROS_______________________________________________________________ */
/** 
 *  Helper macro to define a GPIO pin.
 *  
 *  Usage example:
 *  
 *      @code{.c}
 *      // LED is an output pin on port D, pin 7, initially low
 *      #define PX_GPIO_LED    PX_GPIO(D, 7, PX_GPIO_MODE_OUT, \
 *                                           PX_GPIO_OTYPE_PUSH_PULL, \
 *                                           PX_GPIO_OSPEED_LOW, \
 *                                           PX_GPIO_PULL_NONE, \
 *                                           PX_GPIO_OUT_INIT_LO, \
 *                                           PX_GPIO_ALT_FN_NA)
 *  
 *      // PX_GPIO_LED will be replaced by the C preprocessor with the following
 *      // sequence:
 *      // GPIOD, 7, PX_GPIO_MODE_OUT, PX_GPIO_OTYPE_PUSH_PULL, 
 *      // PX_GPIO_OSPEED_LOW, PX_GPIO_PULL_NONE, PX_GPIO_OUT_INIT_LO,
 *      // PX_GPIO_ALT_FN_NA
 *      @endcode
 *  
 *  @param port         A, B, C, D, E or H
 *  @param pin          0, 1, 2, ..., or 15
 *  @param mode         Input, Output, Alternative Function or Analog; @see px_gpio_mode_t
 *  @param output_type  push-pull or open-drain; @see px_gpio_output_type_t
 *  @param speed        low, medium, high or very high; @see px_gpio_output_speed_t
 *  @param pull         None, pull-up or pull-down; @see px_gpio_pull_t
 *  @param output_init  Low (0) or high (1); @see px_gpio_output_init_t 
 *  @param alt_fn       Alternative Function 0 to 7 or Not Applicable
 */
#define PX_GPIO(port, pin, mode, output_type, speed, pull, output_init, alt_fn) \
    GPIO ## port, pin, mode, output_type, speed, pull, output_init, alt_fn

/// Initialise a GPIO pin
#define PX_GPIO_PIN_INIT(gpio) _GPIO_PIN_INIT(gpio)
#define _GPIO_PIN_INIT(gpio_base_reg, pin, mode, output_type, speed, pull, output_init, alt_fn) \
    do \
    { \
        if(mode == PX_GPIO_MODE_IN) \
        { \
            LL_GPIO_SetPinPull(gpio_base_reg, (1 << pin), pull); \
        } \
        else if(mode == PX_GPIO_MODE_OUT) \
        { \
            if(output_type == PX_GPIO_OTYPE_OD) \
            { \
                LL_GPIO_SetPinPull(gpio_base_reg, (1 << pin), pull); \
            } \
            if(output_init) \
            { \
                LL_GPIO_SetOutputPin(gpio_base_reg, (1 << pin)); \
            } \
            else \
            { \
                LL_GPIO_ResetOutputPin(gpio_base_reg, (1 << pin)); \
            } \
            LL_GPIO_SetPinSpeed(gpio_base_reg, (1 << pin), speed); \
            LL_GPIO_SetPinOutputType(gpio_base_reg, (1 << pin), output_type); \
        } \
        else if(mode == PX_GPIO_MODE_AF) \
        { \
            LL_GPIO_SetPinSpeed(gpio_base_reg, (1 << pin), speed); \
            LL_GPIO_SetPinOutputType(gpio_base_reg, (1 << pin), output_type); \
            if(pin < 8) \
            { \
                LL_GPIO_SetAFPin_0_7(gpio_base_reg, (1 << pin), alt_fn); \
            } \
            else \
            { \
                LL_GPIO_SetAFPin_8_15(gpio_base_reg, (1 << pin), alt_fn); \
            } \
        } \
        LL_GPIO_SetPinMode(gpio_base_reg, (1 << pin), mode);         \
    } \
    while(0)

/// Set GPIO pin output high
#define PX_GPIO_PIN_SET_HI(gpio) _GPIO_PIN_SET_HI(gpio)
#define _GPIO_PIN_SET_HI(gpio_base_reg, pin, mode, output_type, speed, pull, output_init, alt_fn) \
        LL_GPIO_SetOutputPin(gpio_base_reg, ((uint32_t)1 << pin))

/// Set GPIO pin output low
#define PX_GPIO_PIN_SET_LO(gpio) _GPIO_PIN_SET_LO(gpio)
#define _GPIO_PIN_SET_LO(gpio_base_reg, pin, mode, output_type, speed, pull, output_init, alt_fn) \
        LL_GPIO_ResetOutputPin(gpio_base_reg, ((uint32_t)1 << pin))

/// Toggle GPIO pin output
#define PX_GPIO_PIN_TOGGLE(gpio) _GPIO_PIN_TOGGLE(gpio)
#define _GPIO_PIN_TOGGLE(gpio_base_reg, pin, mode, output_type, speed, pull, output_init, alt_fn) \
        LL_GPIO_TogglePin(gpio_base_reg, ((uint32_t)1 << pin))

/// Test if GPIO pin output is set high
#define PX_GPIO_OUT_IS_HI(gpio) _GPIO_OUT_IS_HI(gpio)
#define _GPIO_OUT_IS_HI(gpio_base_reg, pin, mode, output_type, speed, pull, output_init, alt_fn) \
        LL_GPIO_IsOutputPinSet(gpio_base_reg, ((uint32_t)1 << pin))

/// Test if GPIO pin output is set low
#define PX_GPIO_OUT_IS_LO(gpio) _GPIO_OUT_IS_LO(gpio)
#define _GPIO_OUT_IS_LO(gpio_base_reg, pin, mode, output_type, speed, pull, output_init, alt_fn) \
        (!LL_GPIO_IsOutputPinSet(gpio_base_reg, ((uint32_t)1 << pin)))

/// Test if GPIO pin input is high
#define PX_GPIO_PIN_IS_HI(gpio) _GPIO_PIN_IS_HI(gpio)
#define _GPIO_PIN_IS_HI(gpio_base_reg, pin, mode, output_type, speed, pull, output_init, alt_fn) \
        LL_GPIO_IsInputPinSet(gpio_base_reg, ((uint32_t)1 << pin))

/// Test if GPIO pin input is low
#define PX_GPIO_PIN_IS_LO(gpio) _GPIO_PIN_IS_LO(gpio)
#define _GPIO_PIN_IS_LO(gpio_base_reg, pin, mode, output_type, speed, pull, output_init, alt_fn) \
        (!LL_GPIO_IsInputPinSet(gpio_base_reg, ((uint32_t)1 << pin)))

/// Set GPIO pin direction to output
#define PX_GPIO_DIR_SET_OUT(gpio) _GPIO_DIR_SET_OUT(gpio)
#define _GPIO_DIR_SET_OUT(gpio_base_reg, pin, mode, output_type, speed, pull, output_init, alt_fn) \
        LL_GPIO_SetPinMode(gpio_base_reg, ((uint32_t)1 << pin), LL_GPIO_MODE_OUTPUT)

/// Set GPIO pin direction to input
#define PX_GPIO_DIR_SET_IN(gpio) _GPIO_DIR_SET_IN(gpio)
#define _GPIO_DIR_SET_IN(gpio_base_reg, pin, mode, output_type, speed, pull, output_init, alt_fn) \
        LL_GPIO_SetPinMode(gpio_base_reg, ((uint32_t)1 << pin), LL_GPIO_MODE_INPUT)

/// Test if GPIO pin is configured to be an output
#define PX_GPIO_DIR_IS_OUT(gpio) _GPIO_DIR_IS_OUT(gpio)
#define _GPIO_DIR_IS_OUT(gpio_base_reg, pin, mode, output_type, speed, pull, output_init, alt_fn) \
        (LL_GPIO_GetPinMode(gpio_base_reg, ((uint32_t)1 << pin)) == LL_GPIO_MODE_OUTPUT);

/// Test if GPIO pin is configured to be an input
#define PX_GPIO_DIR_IS_IN(gpio) _GPIO_DIR_IS_IN(gpio)
#define _GPIO_DIR_IS_IN(gpio_base_reg, pin, mode, output_type, speed, pull, output_init, alt_fn) \
        (LL_GPIO_GetPinMode(gpio_base_reg, ((uint32_t)1 << pin)) == LL_GPIO_MODE_OUTPUT);

/// Enable pull-up on GPIO pin
#define PX_GPIO_PULLUP_ENABLE(gpio) _GPIO_PULLUP_ENABLE(gpio)
#define _GPIO_PULLUP_ENABLE(gpio_base_reg, pin, mode, output_type, speed, pull, output_init, alt_fn) \
        LL_GPIO_SetPinPull(gpio_base_reg, ((uint32_t)1 << pin), LL_GPIO_PULL_UP);

/// Enable pull-down on GPIO pin
#define PX_GPIO_PULLDN_ENABLE(gpio) _GPIO_PULLDN_ENABLE(gpio)
#define _GPIO_PULLDN_ENABLE(gpio_base_reg, pin, mode, output_type, speed, pull, output_init, alt_fn) \
        LL_GPIO_SetPinPull(gpio_base_reg, ((uint32_t)1 << pin), LL_GPIO_PULL_DN);

/// Disable pull-up / pull-down on GPIO pin
#define PX_GPIO_PULL_DISABLE(gpio) _GPIO_PULL_DISABLE(gpio)
#define _GPIO_PULL_DISABLE(gpio_base_reg, pin, mode, output_type, speed, pull, output_init, alt_fn) \
        LL_GPIO_SetPinPull(gpio_base_reg, ((uint32_t)1 << pin), LL_GPIO_PULL_NO);

/// Macro to calculate bit mask used to initialise GPIOx_MODER register
#define PX_GPIO_REG_MODER_INIT(gpio) _PX_GPIO_REG_MODER_INIT(gpio)
#define _PX_GPIO_REG_MODER_INIT(gpio_base_reg, pin, mode, output_type, speed, pull, output_init, alt_fn) \
        (((uint32_t)mode) << (pin * 2))

/// Macro to calculate bit mask used to initialise GPIOx_OTYPER register
#define PX_GPIO_REG_OTYPER_INIT(gpio) _PX_GPIO_REG_OTYPER_INIT(gpio)
#define _PX_GPIO_REG_OTYPER_INIT(gpio_base_reg, pin, mode, output_type, speed, pull, output_init, alt_fn) \
        (((uint32_t)output_type) << (pin))

/// Macro to calculate bit mask used to initialise GPIOx_OSPEEDR register
#define PX_GPIO_REG_OSPEEDR_INIT(gpio) _PX_GPIO_REG_OSPEEDR_INIT(gpio)
#define _PX_GPIO_REG_OSPEEDR_INIT(gpio_base_reg, pin, mode, output_type, speed, pull, output_init, alt_fn) \
        (((uint32_t)speed) << (pin * 2))

/// Macro to calculate bit mask used to initialise GPIOx_PUPDR register
#define PX_GPIO_REG_PUPDR_INIT(gpio) _PX_GPIO_REG_PUPDR_INIT(gpio)
#define _PX_GPIO_REG_PUPDR_INIT(gpio_base_reg, pin, mode, output_type, speed, pull, output_init, alt_fn) \
        (((uint32_t)pull) << (pin * 2))

/// Macro to calculate bit mask used to initialise GPIOx_OTYPER register
#define PX_GPIO_REG_ODR_INIT(gpio) _PX_GPIO_REG_ODR_INIT(gpio)
#define _PX_GPIO_REG_ODR_INIT(gpio_base_reg, pin, mode, output_type, speed, pull, output_init, alt_fn) \
        (((uint32_t)output_init) << (pin))

/// Macro to calculate bit mask used to initialise GPIOx_AFRL register (pins 0 to 7)
#define PX_GPIO_REG_AFRL_INIT(gpio) _PX_GPIO_REG_AFRL_INIT(gpio)
#define _PX_GPIO_REG_AFRL_INIT(gpio_base_reg, pin, mode, output_type, speed, pull, output_init, alt_fn) \
        ((pin > 7) ? 0 : (((uint32_t)alt_fn) << (pin * 4)))

/// Macro to calculate bit mask used to initialise GPIOx_AFRH register (pins 8 to 15)
#define PX_GPIO_REG_AFRH_INIT(gpio) _PX_GPIO_REG_AFRH_INIT(gpio)
#define _PX_GPIO_REG_AFRH_INIT(gpio_base_reg, pin, mode, output_type, speed, pull, output_init, alt_fn) \
        ((pin < 8) ? 0 : (((uint32_t)alt_fn) << ((pin - 8) * 4)))

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise a GPIO port
 *  
 *  @param init Pointer to structure containing register initialization values
 */
void px_gpio_port_init(const px_gpio_port_init_t * init);

/**
 *  Open a GPIO handle using specified options.
 *  
 *  @param [in, out]    gpio_handle     Pointer to px_gpio_handle_t structure
 *  @param [in]         gpio_base_reg   GPIO register base address
 *  @param [in]         pin             Pin: 0, 1, 2, ..., or 15
 *  @param [in]         mode            Mode: Input, Output, Alternative Function or Analog
 *  @param [in]         output_type     Output type: push-pull or open-drain
 *  @param [in]         speed           Output speed: low, medium, high or very high
 *  @param [in]         pull            None, pull-up or pull-down
 *  @param [in]         output_init     Low (0) or high (1)
 *  @param [in]         alt_fn          Alternative function: AF0, AF1, ... or AF7
 *  
 * Example:
 *  
 *      @code{.c}
 *      px_gpio_handle_t px_gpio_led;
 *  
 *      px_gpio_init(&px_gpio_led,
 *                   GPIOD, 7,
 *                   PX_GPIO_MODE_OUT, 
 *                   PX_GPIO_OTYPE_PUSH_PULL,
 *                   PX_GPIO_OSPEED_LOW,
 *                   PX_GPIO_PULL_NONE,
 *                   PX_GPIO_OUT_INIT_LO,
 *                   PX_GPIO_ALT_FN_NA);
 *      @endcode
 */
static inline void px_gpio_open(px_gpio_handle_t * gpio, 
                                GPIO_TypeDef *     gpio_base_reg,
                                uint8_t            pin,
                                px_gpio_mode_t     mode,
                                px_gpio_otype_t    output_type,
                                px_gpio_ospeed_t   speed,
                                px_gpio_pull_t     pull,
                                px_gpio_out_init_t output_init,
                                px_gpio_af_t       alt_fn)
{
    gpio->gpio_base_reg = gpio_base_reg;
    gpio->pin           = pin;
    gpio->mode          = mode;
    gpio->otype         = output_type;
    gpio->ospeed        = speed;
    gpio->pull          = pull;
    gpio->out_init      = output_init;
    gpio->af            = alt_fn;
}

/**
 *  Open a GPIO handle and initialize with current pin configuration.
 *  
 *  @param [in, out]    gpio            Pointer to px_gpio_handle_t structure
 *  @param [in]         gpio_base_reg   GPIO register base address
 *  @param [in]         pin             Pin: 0, 1, 2, ..., or 15
 *  
 */
void px_gpio_open2(px_gpio_handle_t * gpio, 
                   GPIO_TypeDef *     gpio_base_reg,
                   uint8_t            pin);

/// Initialise a GPIO pin using supplied handle
void px_gpio_pin_init(const px_gpio_handle_t * gpio);

/// Set GPIO pin output high
static inline void px_gpio_pin_set_hi(const px_gpio_handle_t * gpio)
{
    uint32_t pin_bit_mask = ((uint32_t)1) << gpio->pin;

    LL_GPIO_SetOutputPin(gpio->gpio_base_reg, pin_bit_mask);
}

/// Set GPIO pin output low
static inline void px_gpio_pin_set_lo(const px_gpio_handle_t * gpio)
{
    uint32_t pin_bit_mask = ((uint32_t)1) << gpio->pin;

    LL_GPIO_ResetOutputPin(gpio->gpio_base_reg, pin_bit_mask);
}

/// Toggle GPIO pin output
static inline void px_gpio_pin_toggle(const px_gpio_handle_t * gpio)
{
    uint32_t pin_bit_mask = ((uint32_t)1) << gpio->pin;

    LL_GPIO_TogglePin(gpio->gpio_base_reg, pin_bit_mask);
}

/// Test if GPIO pin output is set high
static inline bool px_gpio_out_is_hi(const px_gpio_handle_t * gpio)
{
    uint32_t pin_bit_mask = ((uint32_t)1) << gpio->pin;

    return LL_GPIO_IsOutputPinSet(gpio->gpio_base_reg, pin_bit_mask);
}

/// Test if GPIO pin output is set low
static inline bool px_gpio_out_is_lo(const px_gpio_handle_t * gpio)
{
    uint32_t pin_bit_mask = ((uint32_t)1) << gpio->pin;

    return !LL_GPIO_IsOutputPinSet(gpio->gpio_base_reg, pin_bit_mask);
}

/// Test if GPIO pin input is high
static inline bool px_gpio_pin_is_hi(const px_gpio_handle_t * gpio)
{
    uint32_t pin_bit_mask = ((uint32_t)1) << gpio->pin;

    return LL_GPIO_IsInputPinSet(gpio->gpio_base_reg, pin_bit_mask);
}

/// Test if GPIO pin input is low
static inline bool px_gpio_pin_is_lo(const px_gpio_handle_t * gpio)
{
    uint32_t pin_bit_mask = ((uint32_t)1) << gpio->pin;

    return !LL_GPIO_IsInputPinSet(gpio->gpio_base_reg, pin_bit_mask);
}

/// Set GPIO pin direction to output
static inline void px_gpio_dir_set_out(const px_gpio_handle_t * gpio)
{
    uint32_t pin_bit_mask = ((uint32_t)1) << gpio->pin;

    LL_GPIO_SetPinMode(gpio->gpio_base_reg, pin_bit_mask, LL_GPIO_MODE_OUTPUT);
}

/// Set GPIO pin direction to input
static inline void px_gpio_dir_set_in(const px_gpio_handle_t * gpio)
{
    uint32_t pin_bit_mask = ((uint32_t)1) << gpio->pin;

    LL_GPIO_SetPinMode(gpio->gpio_base_reg, pin_bit_mask, PX_GPIO_MODE_IN);
}

/// Test if GPIO pin is configured to be an output
static inline bool px_gpio_dir_is_out(const px_gpio_handle_t * gpio)
{
    uint32_t pin_bit_mask = ((uint32_t)1) << gpio->pin;

    return (LL_GPIO_GetPinMode(gpio->gpio_base_reg, pin_bit_mask) == LL_GPIO_MODE_OUTPUT);
}

/// Test if GPIO pin is configured to be an input
static inline bool px_gpio_dir_is_in(const px_gpio_handle_t * gpio)
{
    uint32_t pin_bit_mask = ((uint32_t)1) << gpio->pin;

    return (LL_GPIO_GetPinMode(gpio->gpio_base_reg, pin_bit_mask) == LL_GPIO_MODE_INPUT);
}

/// Enable pull-up on GPIO pin
static inline void px_gpio_pullup_enable(const px_gpio_handle_t * gpio)
{
    uint32_t pin_bit_mask = ((uint32_t)1) << gpio->pin;

    LL_GPIO_SetPinPull(gpio->gpio_base_reg, pin_bit_mask, LL_GPIO_PULL_UP);
}

/// Enable pull-down on GPIO pin
static inline void px_gpio_pulldn_enable(const px_gpio_handle_t * gpio)
{
    uint32_t pin_bit_mask = ((uint32_t)1) << gpio->pin;

    LL_GPIO_SetPinPull(gpio->gpio_base_reg, pin_bit_mask, LL_GPIO_PULL_DOWN);
}

/// Disable pull-up / pull-down on GPIO pin
static inline void px_gpio_pull_disable(const px_gpio_handle_t * gpio)
{
    uint32_t pin_bit_mask = ((uint32_t)1) << gpio->pin;

    LL_GPIO_SetPinPull(gpio->gpio_base_reg, pin_bit_mask, LL_GPIO_PULL_NO);
}

/// Change GPIO pin mode
static inline void px_gpio_mode_set(const px_gpio_handle_t * gpio, 
                                    px_gpio_mode_t           mode)
{
    uint32_t pin_bit_mask = ((uint32_t)1) << gpio->pin;

    LL_GPIO_SetPinMode(gpio->gpio_base_reg, 
                       pin_bit_mask,
                       mode);
}

/// Calculate bit mask used to initialise GPIOx_MODER register
static inline uint32_t px_gpio_reg_moder_init(const px_gpio_handle_t * gpio)
{
    return (((uint32_t)gpio->mode) << (gpio->pin * 2));
}

/// Calculate bit mask used to initialise GPIOx_OTYPER register
static inline uint32_t px_gpio_reg_otyper_init(const px_gpio_handle_t * gpio)
{
    return (((uint32_t)gpio->otype) << (gpio->pin));
}

/// Calculate bit mask used to initialise GPIOx_OSPEEDR register
static inline uint32_t px_gpio_reg_ospeedr_init(const px_gpio_handle_t * gpio)
{
    return (((uint32_t)gpio->ospeed) << (gpio->pin * 2));
}

/// Calculate bit mask used to initialise GPIOx_PUPDR register
static inline uint32_t px_gpio_reg_pupdr_init(const px_gpio_handle_t * gpio)
{
    return (((uint32_t)gpio->pull) << (gpio->pin * 2));
}

/// Calculate bit mask used to initialise GPIOx_AFRL register (pins 0 to 7)
static inline uint32_t px_gpio_reg_afrl_init(const px_gpio_handle_t * gpio)
{
    if(gpio->pin < 8)
    {
        return (((uint32_t)gpio->pull) << (gpio->pin * 4));
    }
    else
    {
        return 0;
    }
}

/// Calculate bit mask used to initialise GPIOx_AFRL register (pins 8 to 15)
static inline uint32_t px_gpio_reg_afrh_init(const px_gpio_handle_t * gpio)
{
    if(gpio->pin >= 8)
    {
        return (((uint32_t)gpio->pull) << ((gpio->pin - 8)* 4));
    }
    else
    {
        return 0;
    }
}

/// @}
#ifdef __cplusplus
}
#endif

#endif
