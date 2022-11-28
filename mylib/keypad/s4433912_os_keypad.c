/**
 ***************************************************************
 * @file mylib/keypad/s4433912_os_keypad.c
 * @author Leo Ly - 44339124
 * @date 05/05/2020
 * @brief keypad os driver
 * REFERENCE: csse3010_mylib_os_keypad.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 * s4433912_os_KeypadTaskDeInit(void) - Deinitiialise all running tasks, queue and eventgroup
 * s4433912_os_TaskKeypadInit(void) - Initialise hal_keypad and eventgroup
 * s4433912_os_TaskKeypadBitReading(void) - Initialise s4433912_QueueKeypadRead, read from eventgroup and send to the queue
 ***************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "s4433912_os_keypad.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DEBOUNCE_DELAY 200
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  deinitialises any OS functions
  * @param  None
  * @retval None
  */
extern void s4433912_os_KeypadTaskDeInit(void) {

  if (s4433912_TaskKeypadInit != NULL) {

    vTaskDelete(s4433912_TaskKeypadInit);
  }
  if (s4433912_QueueKeypadRead != NULL) {

    vQueueDelete(s4433912_QueueKeypadRead);
  }
}

/**
  * @brief  Task that initialises hal keypad, debounces and send pressed key to QueueKeypadRead
  * @param  None
  * @retval None
  */
extern void s4433912_os_TaskKeypadInit(void) {

  const TickType_t xTicksToDelay = 10;
  unsigned char keypad_out;
  TickType_t debounce_tick = 0;
  EventBits_t xBitsToSet;

  portDISABLE_INTERRUPTS();
  s4433912_hal_keypad_init();
  portENABLE_INTERRUPTS();

  s4433912_QueueKeypadRead = xQueueCreate(1, sizeof(EventBits_t));

  for (;;) {
    xBitsToSet = 0x00;
    if (s4433912_QueueKeypadRead != NULL) {

      s4433912_hal_keypad_fsmprocessing();
      if (s4433912_hal_keypad_read_status()) {

        if (debounce_tick == 0 || (xTaskGetTickCount() - debounce_tick > DEBOUNCE_DELAY)) {

          debounce_tick = xTaskGetTickCount();

          keypad_out = s4433912_hal_keypad_read_key();
          xBitsToSet |= (1 << keypad_out);
          //Send the event bits to the queue, it's up to the receiver to decode it
          if (xQueueSendToFront(s4433912_QueueKeypadRead, (void*) &xBitsToSet, xTicksToDelay) != pdPASS) {
            //shouldn't get here
          }
        }
        BRD_LEDGreenToggle();
      } 
    }
    vTaskDelay(xTicksToDelay);
  }
}


