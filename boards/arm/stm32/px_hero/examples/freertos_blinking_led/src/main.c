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
#warning "Segger SystemView support enabled"
#include "SEGGER_SYSVIEW.h"
#endif

/* _____LOCAL DEFINITIONS____________________________________________________ */
typedef enum
{
    BTN_EVENT_PRESS_3_UP = 0,
    BTN_EVENT_PRESS_4_DN = 1,
} btn_event_t;

#ifdef CFG_SEGGER_SYSVIEW_ENABLED
#define SSV_LOG_INFO(...)    SEGGER_SYSVIEW_Print(__VA_ARGS__)
#define SSV_LOG_WARN(...)    SEGGER_SYSVIEW_Warn(__VA_ARGS__)
#define SSV_LOG_ERROR(...)   SEGGER_SYSVIEW_Error(__VA_ARGS__)
#else
#define SSV_LOG_INFO(...)
#define SSV_LOG_WARN(...)
#define SSV_LOG_ERROR(...)
#endif

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static SemaphoreHandle_t sem_btn_press;
static QueueHandle_t     queue_btn_events;

static volatile bool     btn_event_press_3_up_flag;
static volatile bool     btn_event_press_4_dn_flag;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void main_exti_13_handler(void)
{
    BaseType_t higher_priority_task_woken = pdFALSE;

    // Record ISR enter event
    traceISR_ENTER();
    // Set flag to indicate that button has been pressed
    btn_event_press_3_up_flag = true;
    // Give semaphore to indicate that button has been pressed
    xSemaphoreGiveFromISR(sem_btn_press, &higher_priority_task_woken);
    // Switch to higher priority task if unblocked by giving semaphore
    portYIELD_FROM_ISR(higher_priority_task_woken);
}

static void main_exti_12_handler(void)
{
    BaseType_t higher_priority_task_woken = pdFALSE;

    // Record ISR enter event
    traceISR_ENTER();
    // Set flag to indicate that button has been pressed
    btn_event_press_4_dn_flag = true;
    // Give semaphore to indicate that button has been pressed
    xSemaphoreGiveFromISR(sem_btn_press, &higher_priority_task_woken);
    // Switch to higher priority task if unblocked by giving semaphore
    portYIELD_FROM_ISR(higher_priority_task_woken);
}

static bool main_init(void)
{
    // Initialize modules
    px_board_init();
    px_exti_init();

    // Success
    return true;
}

static void main_task_btn(void *pvParameters)
{
    uint8_t event;

    SSV_LOG_INFO("BTN task started");

    // Create binary semaphore
    sem_btn_press = xSemaphoreCreateBinary();
    // Enable external falling edge interrupt on Port C pin 13 (3/UP button)
    px_exti_open(PX_EXTI_PORT_C,
                 PX_EXTI_LINE_13,
                 &main_exti_13_handler);
    px_exti_type_set(PX_EXTI_LINE_13, PX_EXTI_TYPE_FALLING_EDGE);
    px_exti_enable(PX_EXTI_LINE_13);
    // Enable external falling edge interrupt on Port C pin 12 (4/DN button)
    px_exti_open(PX_EXTI_PORT_C,
                 PX_EXTI_LINE_12,
                 &main_exti_12_handler);
    px_exti_type_set(PX_EXTI_LINE_12, PX_EXTI_TYPE_FALLING_EDGE);
    px_exti_enable(PX_EXTI_LINE_12);

    // Loop forever
	for(;;)
	{
        // Wait until any button is pressed
        xSemaphoreTake(sem_btn_press, portMAX_DELAY);
        // 3/UP button?
        if(btn_event_press_3_up_flag)
        {
            SSV_LOG_INFO("BTN 3/UP pressed");
            // Clear flag
            btn_event_press_3_up_flag = false;
            // Signal event to LED task
            event = BTN_EVENT_PRESS_3_UP;
            xQueueSendToBack(queue_btn_events, &event, 0);
        }
        // 4/DN button?
        if(btn_event_press_4_dn_flag)
        {
            SSV_LOG_INFO("BTN 4/DN pressed");
            // Clear flag
            btn_event_press_4_dn_flag = false;
            // Signal event to LED task
            event = BTN_EVENT_PRESS_4_DN;
            xQueueSendToBack(queue_btn_events, &event, 0);
        }
	}
}

static void main_task_led(void *pvParameters)
{
    uint8_t    event;
    TickType_t delay = pdMS_TO_TICKS(250);

    SSV_LOG_INFO("LED task started");

    // Loop forever
	for(;;)
	{
        // Toggle LED
        PX_USR_LED_TOGGLE();
        // Delay
        vTaskDelay(delay);

        // Is there an event in the queue?
        if(xQueueReceive(queue_btn_events, &event, 0) == pdTRUE)
        {
            // Button press event?
            switch(event)
            {
            case BTN_EVENT_PRESS_3_UP:
                // Decrease delay
                if(delay > pdMS_TO_TICKS(50))
                {
                    SSV_LOG_INFO("LED blink faster");
                    delay -= pdMS_TO_TICKS(50);
                }
                break;

            case BTN_EVENT_PRESS_4_DN:
                // Increase delay
                if(delay < pdMS_TO_TICKS(500))
                {
                    SSV_LOG_INFO("LED blink slower");
                    delay += pdMS_TO_TICKS(50);
                }
                break;

            default:
                break;
            }
        }
	}
}

/* _____PUBLIC FUNCTIONS_____________________________________________________ */
int main(void)
{
    // Init modules
    main_init();

#ifdef CFG_SEGGER_SYSVIEW_ENABLED
    // Configure and enable Segger SystemView
    SEGGER_SYSVIEW_Conf();
    // Start logging
    SEGGER_SYSVIEW_Start();
    SSV_LOG_INFO("FreeRTOS Blinking LED example started");
#endif

    // Create event queue
    queue_btn_events = xQueueCreate(16, sizeof(uint8_t));

    // Create LED task with a priority of 1
    xTaskCreate(main_task_led, "LED", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    // Create BUTTON task with a priority of 2
    xTaskCreate(main_task_btn, "BTN", configMINIMAL_STACK_SIZE, NULL, 2, NULL);

    // Start scheduler
    vTaskStartScheduler();

    for(;;)
    {
        ;
    }
}
