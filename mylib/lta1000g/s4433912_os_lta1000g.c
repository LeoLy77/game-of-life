/**
 ***************************************************************
 * @file mylib/lta1000g/s4433912_os_lta1000g.c
 * @author Leo Ly - 44339124
 * @date 20/04/2020
 * @brief LED Light Bar OS driver
 * REFERENCE: csse3010_mylib_os_lta1000g.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 * s4433912_os_Lta1000gDeInit () - intialise LED bar OS
 * s4433912_os_TaskLta1000gInit () - deintialise LED bar OS
 ***************************************************************
*/
/* Includes */
#include "s4433912_os_lta1000g.h"
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
  * @brief  Task that deinitialises any OS functions
  * @param  None
  * @retval None
  */
extern void s4433912_os_Lta1000gDeInit(void) {
    if (s4433912_TaskLta100g != NULL) {
        vTaskDelete(s4433912_TaskLta100g);
    }
    
    if (s4433912_QueueLta1000gReadMsg != NULL) {
        vQueueDelete(s4433912_QueueLta1000gReadMsg);
    }
}

/**
  * @brief  LTA1000g task that takes value from display queue and displays on the LTA1000G
  * @param  None
  * @retval None
  */
extern void s4433912_os_TaskLta1000gInit(void) {

    const TickType_t xTicksToWait = 10;
    unsigned short recvValue;

    portDISABLE_INTERRUPTS();
    s4433912_hal_lta1000g_init();
    portENABLE_INTERRUPTS();

    s4433912_QueueLta1000gReadMsg = xQueueCreate(LTA1000G_QUEUE_LENGTH, sizeof(unsigned short));
    /* As per most tasks, this task is implemented in an infinite loop. */
    for (;;) {
        /* This call should always find the queue empty because this task will
        immediately remove any data that is written to the queue. */
        if (s4433912_QueueLta1000gReadMsg != NULL) {

            if (uxQueueMessagesWaiting(s4433912_QueueLta1000gReadMsg) != 0 ) {
                //Queue should not be full
            }
            if (xQueueReceive(s4433912_QueueLta1000gReadMsg, &recvValue, xTicksToWait) == pdPASS) {

                s4433912_hal_lta1000g_write((unsigned short) recvValue);
            } else {
                //Failed to receive
            }
        }

    }
}
