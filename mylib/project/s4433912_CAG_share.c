/**
 ***************************************************************
 * @file mylib/project/CAG_share.c
 * @author Leo Ly - 44339124
 * @date 07/05/2020
 * @brief share resources among all other CAG files
 * REFERENCE: None
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 * s4433912_CAGPushbutton_callback(void) - on-board pushbutton interrupt semaphore callback
 * s4433912_CAGPushbutton_init(void) - initialises on-board leds and pushbuttons
 * s4433912_FreeRTOS_init(void) - initialises freertos elements to be used by various tasks
 * s4433912_grid_printf(uint8_t height, const char *fmt, ...) - safe debug print the CAG display
 * s4433912_os_CAG_TaskPbRecv(void) - Task that switches keypad leds
 * s4433912_os_CAG_TaskLedSend(void) - Tasks that interprets keypad_grid subgrid pos and adjusts the leds
 ***************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "s4433912_CAG_share.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DEBOUNCE_DELAY 300
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TickType_t debounce_tick = 0;
TickType_t xTicksToWait = 5;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  on-board pushbutton interrupt semaphore callback
  * @param  None
  * @retval None
  */
extern void s4433912_CAGPushbutton_callback(void) {

    BaseType_t xHigherPriorityTaskWoken;
    /* Is it time for another Task() to run? */
    xHigherPriorityTaskWoken = pdFALSE;
    if (debounce_tick == 0 || (xTaskGetTickCount() - debounce_tick > 300)) {

        debounce_tick = xTaskGetTickCount();
        /* Check if semaphore exists */
        if (s4433912_SemaphoreCAGPushButton != NULL) {	
            /* Give PB Semaphore from ISR*/
            xSemaphoreGiveFromISR(s4433912_SemaphoreCAGPushButton, &xHigherPriorityTaskWoken );		
        }
    }
    /* Perform context switching, if required. */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/**
  * @brief  initialises on-board leds and pushbuttons
  * @param  None
  * @retval None
  */
extern void s4433912_CAGPushbutton_init(void) {

    GPIO_InitTypeDef GPIO_InitStructure;

    BRD_LEDInit();
	BRD_LEDRedOff();
	BRD_LEDGreenOff();
	BRD_LEDBlueOff();

    BRD_USER_BUTTON_GPIO_CLK_ENABLE();

	GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Pin  = BRD_USER_BUTTON_PIN;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(BRD_USER_BUTTON_GPIO_PORT, &GPIO_InitStructure);

	HAL_NVIC_SetPriority(BRD_USER_BUTTON_EXTI_IRQn, 10, 0);
	HAL_NVIC_EnableIRQ(BRD_USER_BUTTON_EXTI_IRQn);
}

/**
  * @brief  initialises freertos elements to be used by various tasks
  * @param  None
  * @retval None
  */
extern void s4433912_FreeRTOS_init(void) {

    s4433912_EventGroupSimKPFncs = xEventGroupCreate();
    s4433912_SemaphoreCAGPushButton = xSemaphoreCreateBinary();
    s4433912_QueueCAGSimMsg = xQueueCreate(1, sizeof(caMessage_t));
    s4433912_QueueDspGridRead = xQueueCreate(1, sizeof(uint8_t[GRID_WIDTH][GRID_HEIGHT]));
    s4433912_QueueDspColourRead = xQueueCreate(1, sizeof(uint8_t));
    s4433912_QueueLedMsgSend = xQueueCreate(1, sizeof(uint8_t));
}

/**
  * @brief  print on CAG grid at given height
  * @param  height - height offset from the grid
  * @param  fmt - standard printf arguments
  * @retval None
  */
extern void s4433912_grid_printf(uint8_t height, const char *fmt, ...) {

    #if DEBUG
    va_list args;
    portENTER_CRITICAL();
    //Move cursor to top left corner
    debug_printf("\e[H");
    //Move cursor down off the grid
    debug_printf("\e[%dB", (GRID_HEIGHT + 3)*2 + height);
    //Clear line
    debug_printf("\e[K");
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    portEXIT_CRITICAL();
    #endif
}

/**
  * @brief  Task that switches keypad leds
  * @param  None
  * @retval None
  */
 extern void s4433912_os_CAG_TaskPbRecv(void) {
     
    TickType_t debounce_tick_in = 0;
    TickType_t xTicksToWait = 5;
    BRD_LEDBlueOff();
    BRD_LEDRedOn(); //default grid mode

    s4433912_keypad_mode = KP_GRD;
    for (;;) {
        if (s4433912_SemaphoreCAGPushButton != NULL) {

            if(xSemaphoreTake(s4433912_SemaphoreCAGPushButton, portMAX_DELAY) == pdTRUE) { 
                //Debounce button
                if (debounce_tick_in == 0 || (xTaskGetTickCount() - debounce_tick_in > DEBOUNCE_DELAY)) {

                    debounce_tick_in = xTaskGetTickCount();
                    BRD_LEDBlueToggle();
                    BRD_LEDRedToggle();
                    s4433912_keypad_mode = 1 - s4433912_keypad_mode;
                }
            }
        }
        vTaskDelay(xTicksToWait);
    }
 }

/**
  * @brief  Tasks that interprets keypad_grid subgrid pos and adjusts the leds
  * @param  None
  * @retval None
  */
extern void s4433912_os_CAG_TaskLedSend(void) {

    uint8_t recv = 0;
    uint8_t recv_sim = 0;
    uint8_t recv_kp = 0;
    unsigned short leds = 0;
    TickType_t xTicksToWait = 5;
    //Init lta1000g task, priority must be higher than SIM led
    xTaskCreate((void*) &s4433912_os_TaskLta1000gInit, (const signed char *) "LTA1000G", mainSINGLE_STACK_SIZE, NULL, mainHIGHTASK_PRIORITY, &s4433912_TaskLta100g);

    for (;;) {
        if (s4433912_QueueLedMsgSend != NULL) {

            if (xQueueReceive(s4433912_QueueLedMsgSend, (void*) &recv, xTicksToWait) == pdPASS) {
                //MSB denotes whose packet belongs to from the queue
                if ((recv & (1 << 7)) == 0) {
                    //KP writes on the last 6 bits
                    recv_kp = (recv & 0b00111111);
                } else {
                    //Sim writes on the 9th bit
                    recv_sim = (recv & 0b01000000);
                }
            }
        }
        //Combine packets from sim and keypad_grid
        if (s4433912_QueueLta1000gReadMsg != NULL) {

            leds = ((recv_sim << 2) | (recv_kp));
            xQueueSendToFront(s4433912_QueueLta1000gReadMsg, (void*) &leds, xTicksToWait);
        }
    }
    vTaskDelay(xTicksToWait);
}


