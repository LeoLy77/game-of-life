/**
 ***************************************************************
 * @file mylib/project/CAG_joystick.c
 * @author Leo Ly - 44339124
 * @date 27/05/2020
 * @brief Cellular Automation Graphics (CAG) joystick os driver.
 * REFERENCE: CSSE3010 Project.pdf - Design Task 5: CAGJoystick
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 * s4433912_os_CAG_Joystick_Z_callback(void) - extended joystick Z interrupt callback
 * s4433912_os_JoystickTasksInit(void) - Initialise all joystick related tasks
 ***************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "s4433912_CAG_joystick.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MAX_PWM 4096 //12 bits
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
extern void s4433912_os_TaskJoystickReadX(void);
extern void s4433912_os_TaskJoystickReadY(void);

/**
  * @brief  extended joystick Z interrupt callback
  * @param  None
  * @retval None
  */
extern void s4433912_os_CAG_Joystick_Z_callback(void) {

    s4433912_os_JoystickZ_EXTI_handler();
}

/**
  * @brief  Task that takes semaphore from joystick Z interrupt and clears CAGdisplay
  * @param  None
  * @retval None
  */
void s4433912_os_TaskJoystickReadZ(void) {

    TickType_t xTicksToWait = 5;
    for (;;) {
        if (s4433912_SemaphoreJoystickReadZ != NULL) {
            //Wait forever for semaphore
            if (xSemaphoreTake(s4433912_SemaphoreJoystickReadZ, xTicksToWait) == pdPASS) {

                xEventGroupSetBits(s4433912_EventGroupSimKPFncs, (1 << CLEAR_GRID));
            }
        }
        vTaskDelay(xTicksToWait);
    }
}

/**
  * @brief  Timer that initialise all joystick reading tasks
  * @param  s4433912_TimerJoystickInit - Timer handler
  * @retval None
  */
void JoystickReadTasks_init(TimerHandle_t s4433912_TimerJoystickInit) {

    xTaskCreate((void*) &s4433912_os_TaskJoystickReadZ, (const signed char *) "JSK_Z", mainSINGLE_STACK_SIZE, NULL, mainMEDIUMTASK_PRIORITY, &s4433912_TaskJoystickReadZ);
    xTaskCreate((void*) &s4433912_os_TaskJoystickReadX, (const signed char *) "JSK_X", mainDOUBLE_STACK_SIZE, NULL, mainLOWTASK_PRIORITY, &s4433912_TaskJoystickReadX);
    xTaskCreate((void*) &s4433912_os_TaskJoystickReadY, (const signed char *) "JSK_Y", mainDOUBLE_STACK_SIZE, NULL, mainMEDIUMTASK_PRIORITY, &s4433912_TaskJoystickReadY);
}

/**
  * @brief  Timer that waits for notification from simulation to deinit joystick tasks
  * @param  s4433912_TimerJoystickDeinit - timer handler
  * @retval None
  */
void s4433912_TimerJoystickTasksDeinit(TimerHandle_t s4433912_TimerJoystickDeinit) {

    if (s4433912_TaskJoystickReadZ != NULL) {

        vTaskDelete(s4433912_TaskJoystickReadZ);
    }
    if (s4433912_TaskJoystickReadX != NULL) {

        vTaskDelete(s4433912_TaskJoystickReadX);
    }
    if (s4433912_TaskJoystickReadY != NULL) {

        vTaskDelete(s4433912_TaskJoystickReadY);
    }

}

/**
  * @brief  Initialise all joystick related tasks
  * @param  None
  * @retval None
  */
extern void s4433912_os_JoystickTasksInit(void) {

    s4433912_TaskJoystickReadZ = NULL;
    s4433912_TaskJoystickReadX = NULL;
    s4433912_TaskJoystickReadY = NULL;
    /* Create the one shot timers, execute immediately */
    s4433912_TimerJoystickDeinit = xTimerCreate("JSK_Deinit", 5, pdFALSE, 0,s4433912_TimerJoystickTasksDeinit); 
    s4433912_TimerJoystickInit = xTimerCreate("JSK_Init", 5, pdFALSE, 0,JoystickReadTasks_init); 

    xTaskCreate((void*) &s4433912_os_TaskJoystickInit, (const signed char *) "JSK_HAL_INIT", mainSINGLE_STACK_SIZE, NULL, mainMEDIUMTASK_PRIORITY, &s4433912_TaskJoystick);
    xTaskCreate((void*) &s4433912_os_TaskJoystickReadZ, (const signed char *) "JSK_Z", mainSINGLE_STACK_SIZE, NULL, mainMEDIUMTASK_PRIORITY, &s4433912_TaskJoystickReadZ);
    xTaskCreate((void*) &s4433912_os_TaskJoystickReadX, (const signed char *) "JSK_X", mainDOUBLE_STACK_SIZE, NULL, mainLOWTASK_PRIORITY, &s4433912_TaskJoystickReadX);
    xTaskCreate((void*) &s4433912_os_TaskJoystickReadY, (const signed char *) "JSK_Y", mainDOUBLE_STACK_SIZE, NULL, mainMEDIUMTASK_PRIORITY, &s4433912_TaskJoystickReadY);
}

/**
  * @brief  Task that receives joystick X reading and sends according changes in colour to the simulation
  * @param  None
  * @retval None
  */
void s4433912_os_TaskJoystickReadX(void) {

    const TickType_t delay = pdMS_TO_TICKS(200);
    TickType_t xTicksToWait = 5;
    uint32_t x_reading;
    uint8_t colour_sel = 4;
    for (;;) {
        if (s4433912_QueueJoystickX != NULL) {

            xQueueReceive(s4433912_QueueJoystickX, &x_reading, xTicksToWait);
        }
        if (s4433912_QueueDspColourRead != NULL) {

            colour_sel = (x_reading * COLOUR_RANGE)/MAX_PWM;
            xQueueSendToFront(s4433912_QueueDspColourRead, (void*) &colour_sel, xTicksToWait);
        }
        vTaskDelay(delay);
    }
}

/**
  * @brief  Task that receives joystick Y reading and sends according update time to the simulation
  * @param  None
  * @retval None
  */
void s4433912_os_TaskJoystickReadY(void) {

    const TickType_t delay = pdMS_TO_TICKS(100);
    TickType_t xTicksToWait = 5;
    uint32_t y_reading = 0;
    uint8_t rfs_sel = SIM_UPDATE_2;
    for (;;) {
        if (s4433912_EventGroupSimKPFncs != NULL && s4433912_QueueJoystickY != NULL) {

            if (xQueueReceive(s4433912_QueueJoystickY, &y_reading, xTicksToWait) == pdPASS) {
                //Scale up for bit shifts
                rfs_sel = (y_reading * RFS_RANGE)/MAX_PWM + SIM_UPDATE_0_5;
                xEventGroupSetBits(s4433912_EventGroupSimKPFncs, (1 << rfs_sel));
            }
        }
        vTaskDelay(delay);
    }
}
