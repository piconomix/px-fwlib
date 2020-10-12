/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2020 Pieter Conradie <https://piconomix.com>

    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md

    Title:          Piconomix STM32 Hero Board FreeRTOS blinking LED example
    Author(s):      Pieter Conradie
    Creation Date:  2020-04-30

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_board.h"
#include "px_exti.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

#ifdef CFG_SEGGER_SYSVIEW_ENABLED
#warning "Segger SysView support enabled"
#include "SEGGER_SYSVIEW.h"
#endif

/* _____LOCAL DEFINITIONS____________________________________________________ */
typedef enum
{
    LED_TASK_CMD_BLINK_FASTER,
    LED_TASK_CMD_BLINK_SLOWER,
} led_task_cmd_t;

/* _____MACROS_______________________________________________________________ */
#ifdef CFG_SEGGER_SYSVIEW_ENABLED
#define SSV_LOG_INFO(...)    SEGGER_SYSVIEW_Print(__VA_ARGS__)
#define SSV_LOG_WARN(...)    SEGGER_SYSVIEW_Warn(__VA_ARGS__)
#define SSV_LOG_ERROR(...)   SEGGER_SYSVIEW_Error(__VA_ARGS__)
#else
#define SSV_LOG_INFO(...)
#define SSV_LOG_WARN(...)
#define SSV_LOG_ERROR(...)
#endif

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static volatile bool     btn_press_3_up_flag;
static volatile bool     btn_press_4_dn_flag;
static SemaphoreHandle_t btn_task_press_sem;
static QueueHandle_t     led_task_cmd_queue;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void exti_13_handler(void)
{
    BaseType_t higher_priority_task_woken = pdFALSE;

    // Record ISR enter event
    traceISR_ENTER();
    // Set flag to indicate that button has been pressed
    btn_press_3_up_flag = true;
    // Give semaphore to indicate that button has been pressed
    xSemaphoreGiveFromISR(btn_task_press_sem, &higher_priority_task_woken);
    // Switch to higher priority task if unblocked by giving semaphore
    portYIELD_FROM_ISR(higher_priority_task_woken);
}

static void exti_12_handler(void)
{
    BaseType_t higher_priority_task_woken = pdFALSE;

    // Record ISR enter event
    traceISR_ENTER();
    // Set flag to indicate that button has been pressed
    btn_press_4_dn_flag = true;
    // Give semaphore to indicate that button has been pressed
    xSemaphoreGiveFromISR(btn_task_press_sem, NULL);
    // Switch to higher priority task if unblocked by giving semaphore
    portYIELD_FROM_ISR(higher_priority_task_woken);
}

static void led_task_cmd(led_task_cmd_t cmd)
{
    uint8_t item = (uint8_t)cmd;
    // Post command to LED task queue
    if(xQueueSendToBack(led_task_cmd_queue, &item, 0) != pdPASS)
    {
        SSV_LOG_ERROR("LED task cmd queue full");
    }
}

static void task_btn(void *pvParameters)
{
    SSV_LOG_INFO("BTN task started");

    // Create binary semaphore
    btn_task_press_sem = xSemaphoreCreateBinary();
    // Enable external falling edge interrupt on Port C pin 13 (3/UP button)
    px_exti_open(PX_EXTI_PORT_C, PX_EXTI_LINE_13, &exti_13_handler);
    px_exti_type_set(PX_EXTI_LINE_13, PX_EXTI_TYPE_FALLING_EDGE);
    px_exti_enable(PX_EXTI_LINE_13);
    // Enable external falling edge interrupt on Port C pin 12 (4/DN button)
    px_exti_open(PX_EXTI_PORT_C, PX_EXTI_LINE_12, &exti_12_handler);
    px_exti_type_set(PX_EXTI_LINE_12, PX_EXTI_TYPE_FALLING_EDGE);
    px_exti_enable(PX_EXTI_LINE_12);

    // Loop forever
    for(;;)
    {
        // Wait until any button is pressed
        xSemaphoreTake(btn_task_press_sem, portMAX_DELAY);
        // 3/UP button?
        if(btn_press_3_up_flag)
        {
            SSV_LOG_INFO("BTN 3/UP pressed");
            // Clear flag
            btn_press_3_up_flag = false;
            // Send command to LED task
            led_task_cmd(LED_TASK_CMD_BLINK_FASTER);
        }
        // 4/DN button?
        if(btn_press_4_dn_flag)
        {
            SSV_LOG_INFO("BTN 4/DN pressed");
            // Clear flag
            btn_press_4_dn_flag = false;
            // Send command to LED task
            led_task_cmd(LED_TASK_CMD_BLINK_SLOWER);
        }
    }
}

static void task_led(void *pvParameters)
{
    uint8_t    cmd;
    TickType_t delay = pdMS_TO_TICKS(250);

    SSV_LOG_INFO("LED task started");

    // Loop forever
    for(;;)
    {
        // Toggle LED
        PX_USR_LED_TOGGLE();
        // Delay
        vTaskDelay(delay);

        // Is there a command in the queue?
        if(xQueueReceive(led_task_cmd_queue, &cmd, 0) == pdTRUE)
        {
            // Handle command
            switch(cmd)
            {
            case LED_TASK_CMD_BLINK_FASTER:
                // Decrease delay
                if(delay > pdMS_TO_TICKS(50))
                {
                    SSV_LOG_INFO("LED blink faster");
                    delay -= pdMS_TO_TICKS(50);
                }
                else
                {
                    SSV_LOG_WARN("LED max blink rate");
                }
                break;

            case LED_TASK_CMD_BLINK_SLOWER:
                // Increase delay
                if(delay < pdMS_TO_TICKS(500))
                {
                    SSV_LOG_INFO("LED blink slower");
                    delay += pdMS_TO_TICKS(50);
                }
                else
                {
                    SSV_LOG_WARN("LED min blink rate");
                }
                break;

            default:
                SSV_LOG_ERR("Invalid LED task command");
                break;
            }
        }
    }
}

/* _____PUBLIC FUNCTIONS_____________________________________________________ */
int main(void)
{
    // Initialize modules
    px_board_init();
    px_exti_init();

#ifdef CFG_SEGGER_SYSVIEW_ENABLED
    // Configure and enable Segger SystemView
    SEGGER_SYSVIEW_Conf();
    // Start logging
    SEGGER_SYSVIEW_Start();
    SSV_LOG_INFO("FreeRTOS Blinking LED example started");
#endif

    // Create LED command queue
    led_task_cmd_queue = xQueueCreate(2, sizeof(uint8_t));
    // Create LED task with a priority of 1
    xTaskCreate(&task_led, "LED", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    // Create BUTTON task with a priority of 2
    xTaskCreate(&task_btn, "BTN", configMINIMAL_STACK_SIZE, NULL, 2, NULL);

    // Start scheduler
    vTaskStartScheduler();

    // Error! Not supposed to get here...
    SSV_LOG_ERROR("FreeRTOS fatal error");
    for(;;)
    {
        ;
    }
}
