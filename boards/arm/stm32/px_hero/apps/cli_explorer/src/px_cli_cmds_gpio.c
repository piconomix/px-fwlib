/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
 
    Title:          px_cli_cmds_gpio.h : CLI Commands for GPIO
    Author(s):      Pieter Conradie
    Creation Date:  2018-03-06

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <string.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_cli.h"
#include "px_board.h"
#include "px_gpio.h"
#include "px_dbg.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_DBG_DECL_NAME("cli_cmds_gpio");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static px_gpio_handle_t gpio_handle;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
const char * px_cli_cmd_gpio_str_to_handle(px_gpio_handle_t * handle,
                                           char *             pin_str)
{
    GPIO_TypeDef * gpio_base_reg;
    unsigned long  pin;
    char *         end;

    // <pin> or <port,pin> combination?
    if((strlen(pin_str) < 1) || (strlen(pin_str) > 3))
    {
        return "Error. <pin> must either be <0..6>, <a0..a15>, <b0..b15>, <c0..c15>, <d2> or <h0|h1>";
    }

    // Expansion GPIO pins <0..6>?
    if(strlen(pin_str) == 1)
    {
        switch(pin_str[0])
        {
        case '0': 
            gpio_base_reg = px_gpio_0.gpio_base_reg;
            pin           = px_gpio_0.pin;
            break;
        case '1': 
            gpio_base_reg = px_gpio_1.gpio_base_reg;
            pin           = px_gpio_1.pin;
            break;
        case '2': 
            gpio_base_reg = px_gpio_2.gpio_base_reg;
            pin           = px_gpio_2.pin;
            break;
        case '3': 
            gpio_base_reg = px_gpio_3.gpio_base_reg;
            pin           = px_gpio_3.pin;
            break;
        case '4': 
            gpio_base_reg = px_gpio_4.gpio_base_reg;
            pin           = px_gpio_4.pin;
            break;
        case '5': 
            gpio_base_reg = px_gpio_5.gpio_base_reg;
            pin           = px_gpio_5.pin;
            break;
        case '6': 
            gpio_base_reg = px_gpio_6.gpio_base_reg;
            pin           = px_gpio_6.pin;
            break;
        default:
            return "Error. <pin> must be <0..6>";
        }
    }
    else
    {
        // Convert pin number
        pin = 16;
        pin = strtoul(&pin_str[1], &end, 0);
        if(  (*end != '\0') || (pin > 15)  )
        {
            return "Error. <pin> must either be <a0..a15>, <b0..b15>, <c0..c15>, <d2> or <ph0,ph1>";
        }

        // First character is port
        switch(pin_str[0])
        {
        case 'a':
            if(pin >= 16)
            {
                return "Error. Pin number must be 0..15";
            }
            gpio_base_reg = GPIOA;
            break;
        case 'b':
            if(pin >= 16)
            {
                return "Error. Pin number must be 0..15";
            }
            gpio_base_reg = GPIOB;
            break;
        case 'c':
            if(pin >= 16)
            {
                return "Error. Pin number must be 0..15";
            }
            gpio_base_reg = GPIOC;
            break;
        case 'd':
            if(pin != 2)
            {
                return "Error. Pin number must be 2";
            }
            gpio_base_reg = GPIOD;
            break;
        case 'h':
            if(pin > 1)
            {
                return "Error. Pin number must be 0 or 1";
            }
            gpio_base_reg = GPIOH;
            break;
        default:
            return "Error. Port must be a, b, c, d or h";
        }        
    }

    px_gpio_open2(handle, gpio_base_reg, (uint8_t)pin);
    return NULL;
}

static void report_gpio_register(char      port,
                                 char *    reg_name, 
                                 uint32_t  reg_val,
                                 uint8_t   pin_hi,
                                 uint8_t   pin_lo,
                                 uint8_t   bits_per_option,
                                 char *    pin_option_map,
                                 uint16_t  pin_mask)
{
    int   i;
    uint32_t pin_val;

    printf("GPIO%c_%s : %04X %04X | ", 
           port,
	       reg_name, 
           PX_U32_HI16(reg_val), 
           PX_U32_LO16(reg_val));

    // Start at highest pin
    i = 15;
    while(true)
    {
        // Pin value in register?
        if(  (i >= pin_lo) && (i <= pin_hi)  )
        {
            // Should pin be reported?
            if(pin_mask & (1 << i))
            {
                // Extract pin value
                pin_val  = reg_val >> ((i - pin_lo) * bits_per_option);
                pin_val &= (1 << bits_per_option) - 1;
                // Map pin value to character and report
                putchar(pin_option_map[pin_val]);
            }
            else
            {
                putchar('.');
            }
        }
        else
        {
            putchar(' ');
        }
        putchar(' ');
        // Insert extra space?
        if((i & 3) == 0)
        {
            putchar (' ');
        }
        // Done?
        if(i == 0)
        {
            break;
        }
        // Next pin
        i--;
    }
    putchar('\n');
}

static void px_cli_fn_gpio_report_peripheral(char           port,
                                             GPIO_TypeDef * gpio_base_reg, 
                                             uint16_t       pin_mask)
{
    // Mode: I(Input) O(Output) F(Alternative Function) A(Analog)
    report_gpio_register(port, "MODER  ", gpio_base_reg->MODER,   15, 0, 2, "IOFA",             pin_mask);
    // Output Type: _(Push Pull) O(Open Drain)
    report_gpio_register(port, "OTYPER ", gpio_base_reg->OTYPER,  15, 0, 1, " O",               pin_mask);
    // Output Speed: L(Low) M(Medium) H(High) V(Very High)
    report_gpio_register(port, "OSPEEDR", gpio_base_reg->OSPEEDR, 15, 0, 2, "LMHV",             pin_mask);
    // Pull Up Pull Down: _(None) D(Pull Down) U(Pull Up) R(Reserved)
    report_gpio_register(port, "PUPDR  ", gpio_base_reg->PUPDR,   15, 0, 2, " UDR",             pin_mask);
    // Input: 0(zero) 1(one)
    report_gpio_register(port, "IDR    ", gpio_base_reg->IDR,     15, 0, 1, "01",               pin_mask);
    // Output: 0(zero) 1(one)
    report_gpio_register(port, "ODR    ", gpio_base_reg->ODR,     15, 0, 1, "01",               pin_mask);
    // Alternative Function Low: 0..7(AF0..AF7) R(Reserved)
    report_gpio_register(port, "AFRL   ", gpio_base_reg->AFR[0],   7, 0, 4, "01234567RRRRRRRR", pin_mask);
    // Alternative Function High: 0..7(AF0..AF7) R(Reserved)
    report_gpio_register(port, "AFRH   ", gpio_base_reg->AFR[1],  15, 8, 4, "01234567RRRRRRRR", pin_mask);
    putchar('\n');
}

static void px_cli_fn_gpio_report_pin(px_gpio_handle_t * handle)
{
    GPIO_TypeDef * gpio_base_reg = handle->gpio_base_reg;
    uint32_t       pin_bit_mask  = ((uint32_t)1) << handle->pin;

    // Report pin mode
    switch(handle->mode)
    {
    case PX_GPIO_MODE_IN:
        printf("Input ");
        break;
    case PX_GPIO_MODE_OUT:
        printf("Output ");
        break;
    case PX_GPIO_MODE_AF:
        printf("AF%u ", handle->af);
        break;
    case PX_GPIO_MODE_ANA:
        printf("Analog\n");
        return;
    default:
        printf("Error. Uknnown mode\n");
        return;
    }

    // Report pull-up/pull-down
    switch(handle->pull)
    {
    case PX_GPIO_PULL_UP:   printf("Pull-Up "); break;
    case PX_GPIO_PULL_DN: printf("Pull-Dn "); break;
    default:                break;
    }

    // Input?
    if(handle->mode == PX_GPIO_MODE_IN)
    {
        // Report value
        if(LL_GPIO_IsInputPinSet(gpio_base_reg, pin_bit_mask))
        {
            putchar('1');
        }
        else
        {
            putchar('0');
        }
        // Done
        putchar('\n');
        return;
    }

    // Report output type (push-pull or open drain)
    switch(handle->otype)
    {
    case PX_GPIO_OTYPE_PP:      printf("Push-Pull ");   break;
    case PX_GPIO_OTYPE_OD:      printf("Open-Drain ");  break;
    default: break;
    }

    // Report pin speed
    printf("Speed=");
    switch(handle->ospeed)
    {
    case PX_GPIO_OSPEED_LO:     printf("Low ");         break;
    case PX_GPIO_OSPEED_MD:    printf("Medium ");      break;
    case PX_GPIO_OSPEED_HI:     printf("High ");        break;
    case PX_GPIO_OSPEED_VH:     printf("VeryHigh ");    break;
    default:
        break;
    }

    // Output?
    if(handle->mode == PX_GPIO_MODE_OUT)
    {
        // Report value
        if(LL_GPIO_IsOutputPinSet(gpio_base_reg, pin_bit_mask))
        {
            putchar('1');
        }
        else
        {
            putchar('0');
        }        
    }

    // Done
    putchar('\n');
}

static void px_cli_fn_gpio_report_port(char           port,
                                       GPIO_TypeDef * gpio_base_reg, 
                                       uint8_t        pin_start,
                                       uint8_t        pin_end)
{
    uint8_t pin;

    for(pin = pin_start; pin <= pin_end; pin++)
    {
        printf("%c%u : ", port, pin);
        px_gpio_open2(&gpio_handle, gpio_base_reg, pin);
        px_cli_fn_gpio_report_pin(&gpio_handle);
    }
    putchar('\n');
}

static const char* px_cli_cmd_fn_gpio_info(uint8_t argc, char* argv[])
{
    const char * result;

    // <all>
    if(strcmp(argv[0],"all") == 0)
    {
        px_cli_fn_gpio_report_port('a', GPIOA, 0, 15);
        px_cli_fn_gpio_report_port('b', GPIOB, 0, 15);
        px_cli_fn_gpio_report_port('c', GPIOC, 0, 15);
        px_cli_fn_gpio_report_port('d', GPIOD, 2, 2);
        px_cli_fn_gpio_report_port('h', GPIOH, 0, 1);
    }
    // <regs>
    else if(strcmp(argv[0], "regs") == 0)
    {
        printf("                          | F E D C  B A 9 8  7 6 5 4  3 2 1 0\n\n");
        px_cli_fn_gpio_report_peripheral('A', GPIOA, 0xffff);
        px_cli_fn_gpio_report_peripheral('B', GPIOB, 0xffff);
        px_cli_fn_gpio_report_peripheral('C', GPIOC, 0xffff);
        px_cli_fn_gpio_report_peripheral('D', GPIOD, 0x0002);
        px_cli_fn_gpio_report_peripheral('H', GPIOH, 0x0003);
    }
    else
    // <pin>
    {
        result = px_cli_cmd_gpio_str_to_handle(&gpio_handle, argv[0]);
        if(result != NULL)
        {
            return result;
        }
        px_cli_fn_gpio_report_pin(&gpio_handle);
    }

    return NULL;
}

static const char* px_cli_cmd_fn_gpio_cfg_in(uint8_t argc, char* argv[])
{
    const char * result;

    // <pin>
    result = px_cli_cmd_gpio_str_to_handle(&gpio_handle, argv[0]);
    if(result != NULL)
    {
        return result;
    }

    // [pu|pd]
    if(argc >= 2)
    {
        switch(px_cli_util_argv_to_option(2, "pu\0pd\0"))
        {
        case 0: gpio_handle.pull = PX_GPIO_PULL_UP; break;
        case 1: gpio_handle.pull = PX_GPIO_PULL_DN; break;
        default: return "Error. <pu|pd> must be pu or pd";
        }
    }
    else
    {
        gpio_handle.pull = PX_GPIO_PULL_NO;
    }

    // Configure specified GPIO pin as an input
    gpio_handle.mode        = PX_GPIO_MODE_IN;
    gpio_handle.otype       = PX_GPIO_OTYPE_NA;
    gpio_handle.ospeed      = PX_GPIO_OSPEED_NA;
    gpio_handle.out_init    = PX_GPIO_OUT_INIT_NA;
    gpio_handle.af          = PX_GPIO_AF_NA;

    px_gpio_pin_init(&gpio_handle);

    return NULL;
}

static const char* px_cli_cmd_fn_gpio_cfg_out(uint8_t argc, char* argv[])
{
    const char * result;

    // <pin>
    result = px_cli_cmd_gpio_str_to_handle(&gpio_handle, argv[0]);
    if(result != NULL)
    {
        return result;
    }
 
    // <0|1>
    switch(argv[1][0])
    {
    case '0': gpio_handle.out_init = PX_GPIO_OUT_INIT_LO; break;
    case '1': gpio_handle.out_init = PX_GPIO_OUT_INIT_HI; break;
    default:  return "Error. <0|1> must be 0 or 1";
    }

    // [od]
    if(argc >= 3)
    {
        if(strcmp(argv[2], "od") == 0)
        {
            gpio_handle.otype = PX_GPIO_OTYPE_OD;
        }
        else
        {
            return "Error. <od> parameter incorrect";
        }
    }
    else
    {
        gpio_handle.otype = PX_GPIO_OTYPE_PP;
    }

    // [pu]
    if(argc >= 4)
    {
        if(strcmp(argv[3], "pu") == 0)
        {
            gpio_handle.pull = PX_GPIO_PULL_UP;
        }
        else
        {
            return "Error. <pu> parameter incorrect";
        }
    }
    else
    {
        gpio_handle.pull = PX_GPIO_PULL_NO;
    }

    // Configure specified GPIO pin as an output
    gpio_handle.mode   = PX_GPIO_MODE_OUT;
    gpio_handle.ospeed  = PX_GPIO_OSPEED_VH;
    gpio_handle.af = PX_GPIO_AF_NA;
    px_gpio_pin_init(&gpio_handle);

    return NULL;
}

static const char* px_cli_cmd_fn_gpio_in(uint8_t argc, char* argv[])
{
    const char * result;

    // <pin>
    result = px_cli_cmd_gpio_str_to_handle(&gpio_handle, argv[0]);
    if(result != NULL)
    {
        return result;
    }

    if(px_gpio_pin_is_lo(&gpio_handle))
    {
        printf("0\n");
    }
    else
    {
        printf("1\n");
    }

    return NULL;
}

static const char* px_cli_cmd_fn_gpio_out(uint8_t argc, char* argv[])
{
    const char * result;

    // <pin>
    result = px_cli_cmd_gpio_str_to_handle(&gpio_handle, argv[0]);
    if(result != NULL)
    {
        return result;
    }
 
    // <0|1>
    switch(argv[1][0])
    {
    case '0': px_gpio_pin_set_lo(&gpio_handle); break;
    case '1': px_gpio_pin_set_hi(&gpio_handle); break;
    default:  return "Error. <0|1> must be 0 or 1";
    }

    return NULL;
}

// Create CLI command structures
PX_CLI_CMD_CREATE(px_cli_cmd_gpio_info,       "info", 1, 1,   "<pin|all|regs>",         "Report GPIO info. <pin> is <0..6> or <a..h|0..15>")
PX_CLI_CMD_CREATE(px_cli_cmd_gpio_cfg_in,     "ci",   1, 2,   "<pin> [pu|pd]",          "Configure GPIO pin as input. Optionally PullUp or PullDown can be enabled")
PX_CLI_CMD_CREATE(px_cli_cmd_gpio_cfg_out,    "co",   2, 4,   "<pin> <0|1> [od] [pu]",  "Configure GPIO pin as output. Optionally OpenDrain and PullUp can be enabled")
PX_CLI_CMD_CREATE(px_cli_cmd_gpio_in,         "i",    1, 1,   "<pin>",                  "Read GPIO pin input")
PX_CLI_CMD_CREATE(px_cli_cmd_gpio_out,        "o",    2, 2,   "<pin> <0|1>",            "Write GPIO pin output 0 or 1")

PX_CLI_GROUP_CREATE(px_cli_group_gpio, "gpio")
    PX_CLI_CMD_ADD(px_cli_cmd_gpio_info,       px_cli_cmd_fn_gpio_info)
    PX_CLI_CMD_ADD(px_cli_cmd_gpio_cfg_in,     px_cli_cmd_fn_gpio_cfg_in)
    PX_CLI_CMD_ADD(px_cli_cmd_gpio_cfg_out,    px_cli_cmd_fn_gpio_cfg_out)
    PX_CLI_CMD_ADD(px_cli_cmd_gpio_in,         px_cli_cmd_fn_gpio_in)
    PX_CLI_CMD_ADD(px_cli_cmd_gpio_out,        px_cli_cmd_fn_gpio_out)    
PX_CLI_GROUP_END()
