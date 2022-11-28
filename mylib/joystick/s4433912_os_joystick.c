/**
 ***************************************************************
 * @file mylib/joystick/s4433912_os_joystick.c
 * @author Leo Ly - 44339124
 * @date 25/04/2020
 * @brief Joystick OS driver
 * REFERENCE: csse3010_mylib_os_joystick.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 * s4433912_os_JoystickDeinit(void) - Deinitialise os functions
 * s4433912_os_TaskJoystickInit(void) - Initialises os and hal functions
 ***************************************************************
*/
/* Includes */
#include "s4433912_os_joystick.h"
/* Scheduler includes. */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Task Priorities ------------------------------------------------------------*/
/* Task Stack Allocations -----------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/**
  * @brief  Deinitialise all OS functions
  * @param  None
  * @retval None
  */
extern void s4433912_os_JoystickDeinit(void) { 

  if (s4433912_SemaphoreJoystickReadZ != NULL) {

    vSemaphoreDelete(s4433912_SemaphoreJoystickReadZ);
  }
  if (s4433912_TaskJoystick != NULL) {

    vTaskDelete(s4433912_TaskJoystick);
  }
  if (s4433912_QueueJoystickX != NULL && s4433912_QueueJoystickY != NULL) {

    vQueueDelete(s4433912_QueueJoystickX);
    vQueueDelete(s4433912_QueueJoystickY);
  }
}

/**
  * @brief  Joystick Z semaphore interrupt callback
  * @param  None
  * @retval None
  */
extern void s4433912_os_JoystickZ_EXTI_handler(void) {

  TickType_t ucLocalTickPrev = 0;
  BaseType_t xHigherPriorityTaskWoken;

  if (ucLocalTickPrev == 0 || (xTaskGetTickCount() - ucLocalTickPrev > 300)) {

      ucLocalTickPrev = xTaskGetTickCount();
      /* Is it time for another Task() to run? */
      xHigherPriorityTaskWoken = pdFALSE;
      if (s4433912_SemaphoreJoystickReadZ != NULL) {	/* Check if semaphore exists */

          xSemaphoreGiveFromISR(s4433912_SemaphoreJoystickReadZ, &xHigherPriorityTaskWoken);		/* Give PB Semaphore from ISR*/
      }
      /* Perform context switching, if required. */
      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}

/**
  * @brief  Initialise joystick os including hal
  * @param  None
  * @retval None
  */
extern void s4433912_os_TaskJoystickInit(void) {

  uint32_t x_reading, y_reading;
  const TickType_t xTicksToWait = 5;

  portDISABLE_INTERRUPTS();
  s4433912_hal_joystick_init();
  portENABLE_INTERRUPTS();
  s4433912_SemaphoreJoystickReadZ = xSemaphoreCreateBinary();
  s4433912_QueueJoystickX = xQueueCreate(1, sizeof(uint32_t));
  s4433912_QueueJoystickY = xQueueCreate(1, sizeof(uint32_t));

  for (;;) {
    x_reading = S4433912_HAL_JOYSTICK_X_READ();
    y_reading = S4433912_HAL_JOYSTICK_Y_READ(); 
    if (s4433912_QueueJoystickX != NULL && s4433912_QueueJoystickY != NULL) {

      xQueueSendToFront(s4433912_QueueJoystickX, (void*) &x_reading, xTicksToWait);
      xQueueSendToFront(s4433912_QueueJoystickY, (void*) &y_reading, xTicksToWait);
    }
    vTaskDelay(xTicksToWait);
  }
}
