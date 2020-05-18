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

/* _____LOCAL DEFINITIONS____________________________________________________ */
typedef enum
{
    MAIN_EVENT_BTN_PRESS_3_UP = 0,
    MAIN_EVENT_BTN_PRESS_4_DN = 1,
} main_event_t;

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static SemaphoreHandle_t sem_btn_press_3_up;
static SemaphoreHandle_t sem_btn_press_4_dn;
static QueueHandle_t     queue_events;
static QueueSetHandle_t  queue_set;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void main_exti12_handler(void)
{
    BaseType_t higher_priority_task_woken = pdFALSE;

    // Give semaphore to indicate that 4/DN button has been pressed
    xSemaphoreGiveFromISR(sem_btn_press_4_dn, &higher_priority_task_woken);
    // Switch to higher priority task if unblocked by giving semaphore
    portYIELD_FROM_ISR(higher_priority_task_woken);
}

static void main_exti13_handler(void)
{
    BaseType_t higher_priority_task_woken = pdFALSE;

    // Give semaphore to indicate that 3/UP button has been pressed
    xSemaphoreGiveFromISR(sem_btn_press_3_up, NULL);
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
    uint8_t                event;
    QueueSetMemberHandle_t queue_set_member;

    // Create binary semaphores
    sem_btn_press_3_up = xSemaphoreCreateBinary();
    sem_btn_press_4_dn = xSemaphoreCreateBinary();
    // Create queue set for two binary semaphores
    queue_set = xQueueCreateSet(2);
    xQueueAddToSet(sem_btn_press_3_up, queue_set);
    xQueueAddToSet(sem_btn_press_4_dn, queue_set);

    // Enable external falling edge interrupt on Port C pin 12 (4/DN button)
    px_exti_open(PX_EXTI_PORT_C,
                 PX_EXTI_LINE_12,
                 &main_exti12_handler);
    px_exti_type_set(PX_EXTI_LINE_12, PX_EXTI_TYPE_FALLING_EDGE);
    px_exti_enable(PX_EXTI_LINE_12);
    // Enable external falling edge interrupt on Port C pin 13 (3/UP button)
    px_exti_open(PX_EXTI_PORT_C,
                 PX_EXTI_LINE_13,
                 &main_exti13_handler);
    px_exti_type_set(PX_EXTI_LINE_13, PX_EXTI_TYPE_FALLING_EDGE);
    px_exti_enable(PX_EXTI_LINE_13);

    // Loop forever
	for(;;)
	{
        // Wait until any button is pressed
        queue_set_member = xQueueSelectFromSet(queue_set, portMAX_DELAY);
        // 3/UP button?
        if(queue_set_member == sem_btn_press_3_up)
        {
            // Take semaphore so that it can be "given" again
            xSemaphoreTake(sem_btn_press_3_up, 0);
            // Signal event to LED task
            event = MAIN_EVENT_BTN_PRESS_3_UP;
            xQueueSendToBack(queue_events, &event, 0);
        }
        // 4/DN button?
        if(queue_set_member == sem_btn_press_4_dn)
        {
            // Take semaphore so that it can be "given" again
            xSemaphoreTake(sem_btn_press_4_dn, 0);
            // Signal event to LED task
            event = MAIN_EVENT_BTN_PRESS_4_DN;
            xQueueSendToBack(queue_events, &event, 0);
        }
	}
}

static void main_task_led(void *pvParameters)
{
    uint8_t    event;
    TickType_t delay = pdMS_TO_TICKS(250);

    // Loop forever
	for(;;)
	{
        // Toggle LED
        PX_USR_LED_TOGGLE();
        // Delay
        vTaskDelay(delay);

        // Is there an event in the queue?
        if(xQueueReceive(queue_events, &event, 0) == pdTRUE)
        {
            // Button press event?
            switch(event)
            {
            case MAIN_EVENT_BTN_PRESS_3_UP:
                // Decrease delay                
                if(delay > pdMS_TO_TICKS(50))
                {
                    delay -= pdMS_TO_TICKS(50);
                }
                break;

            case MAIN_EVENT_BTN_PRESS_4_DN:
                // Increase delay                
                if(delay < pdMS_TO_TICKS(500))
                {
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

    // Create event queue
    queue_events = xQueueCreate(16, sizeof(uint8_t));
    // Create LED task with a priority of 1
    xTaskCreate(main_task_led, "LED", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    // Create BUTTON task with a priority of 2
    xTaskCreate(main_task_btn, "BTN", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    // Start scheduler
    vTaskStartScheduler();

    for(;;)
    {
        ;
    }
}
