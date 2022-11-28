/**
  ******************************************************************************
  * @file    stage4/main.c
  * @author  Leo Ly - 44339124
  * @date    23042020
  * @brief   FreeRTOS Timer and Control
  ******************************************************************************
  *
  */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "debug_printf.h"
#include "processor_hal.h"
#include "s4433912_os_lta1000g.h"
#include "s4433912_os_joystick.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


/* Private typedef -----------------------------------------------------------*/
struct dualTimerMsg { 
    char type; //type is either l or r
    unsigned char timerValue;
    unsigned char increment;
    int delay;
    GPIO_TypeDef* port;
    uint32_t pin;
};

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
#define queueTIMER_LOAD 3
/* Private variables ---------------------------------------------------------*/
QueueHandle_t queueTimerDisplay;	/* Queue used */
TaskHandle_t taskTimerLeft, taskTimerRight, taskTimerDisplay;
volatile TickType_t ucLocalTickPrev = 0;
volatile uint8_t timerSuspendSts = 0;
struct dualTimerMsg timerRecv, timerRight, timerLeft;

/* Private function prototypes -----------------------------------------------*/
void Hardware_init();
void ApplicationIdleHook(void); /* The idle hook is used to blink the Blue 'Alive LED' every second */
void s4433912_TaskTimerLeft(void);
void s4433912_TaskTimerRight(void);
void s4433912_TaskTimerDisplay(void);
void JoystickTaskPb(void);

/* Task Priorities ------------------------------------------------------------*/
#define mainLTA1000GTASK_PRIORITY					( tskIDLE_PRIORITY + 3 )
#define mainTIMERLEFTTASK_PRIORITY					( tskIDLE_PRIORITY + 3 )
#define mainTIMERRIGHTTASK_PRIORITY					( tskIDLE_PRIORITY + 3 )
#define mainTIMERDDISPLAYTASK_PRIORITY				( tskIDLE_PRIORITY + 2 )

/* Task Stack Allocations -----------------------------------------------------*/
#define mainLTA1000G_TASK_STACK_SIZE		( configMINIMAL_STACK_SIZE * 2 )
#define mainTIMERLEFTTASK_STACK_SIZE		( configMINIMAL_STACK_SIZE * 2 )
#define mainTIMERRIGHTTASK_STACK_SIZE		( configMINIMAL_STACK_SIZE * 2 )
#define mainTIMEDISPLAYTASK_STACK_SIZE		( configMINIMAL_STACK_SIZE * 2 )
#define mainSINGLE_STACK_SIZE				( configMINIMAL_STACK_SIZE * 1 )
#define mainDOUBLE_STACK_SIZE				( configMINIMAL_STACK_SIZE * 2 )
#define mainTRIPLE_STACK_SIZE				( configMINIMAL_STACK_SIZE * 3 )


/**
  * @brief  Starts all the other tasks, then starts the scheduler.
  * @param  None
  * @retval None
  */
int main( void ) {

    BRD_init();
    Hardware_init();

    timerLeft.pin = BRD_D14_PIN;
    timerLeft.port = BRD_D14_GPIO_PORT;
    timerLeft.delay = 1000;
    timerLeft.type = 'l';
    timerLeft.timerValue = 0;
    timerLeft.increment = 0;

    timerRight.pin = BRD_D15_PIN;
    timerRight.port = BRD_D15_GPIO_PORT;
    timerRight.delay = 100;
    timerRight.type = 'r';
    timerRight.timerValue = 0;
    timerRight.increment = 0;

    queueTimerDisplay = xQueueCreate(queueTIMER_LOAD, sizeof(struct dualTimerMsg));
    s4433912_SemaphoreJoystickReadZ = xSemaphoreCreateBinary();

    if (queueTimerDisplay != NULL && s4433912_SemaphoreJoystickReadZ != NULL) {

        xTaskCreate( (void*) &s4433912_TaskTimerLeft, (const signed char *) "TIMER_LEFT", mainTIMERLEFTTASK_STACK_SIZE, NULL, mainTIMERLEFTTASK_PRIORITY, &taskTimerLeft);
        xTaskCreate( (void*) &s4433912_TaskTimerRight, (const signed char *) "TIMER_RIGHT", mainTIMERRIGHTTASK_STACK_SIZE, NULL, mainTIMERRIGHTTASK_PRIORITY, &taskTimerRight);
        xTaskCreate( (void*) &s4433912_TaskTimerDisplay, (const signed char *) "TIMER_DISPLAY", mainTIMEDISPLAYTASK_STACK_SIZE, NULL, mainTIMERDDISPLAYTASK_PRIORITY, &taskTimerDisplay);
        
        xTaskCreate( (void*) &s4433912_os_TaskLta1000gInit, (const signed char *) "LTA1000G", mainLTA1000G_TASK_STACK_SIZE, NULL, mainLTA1000GTASK_PRIORITY, &s4433912_TaskLta100g);

        xTaskCreate( (void*) &s4433912_os_TaskJoystickInit, (const signed char *) "JOYSTICK", mainDOUBLE_STACK_SIZE, NULL, 2, &s4433912_TaskJoystick);		
        xTaskCreate( (void*) &JoystickTaskPb, (const signed char *) "JOYSTICKPB", mainDOUBLE_STACK_SIZE, NULL, mainTIMERLEFTTASK_PRIORITY + 1, NULL);
        /* Start the scheduler.

        NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
        The processor MUST be in supervisor mode when vTaskStartScheduler is
        called.  The demo applications included in the FreeRTOS.org download switch
        to supervisor mode prior to main being called.  If you are not using one of
        these demo application projects then ensure Supervisor mode is used here. */

        vTaskStartScheduler();
    }

    /* We should never get here as control is now taken by the scheduler. */
      return 0;
}


/**
  * @brief  Take Task. Used to take semaphore events. Toggles LED.
  * @param  None
  * @retval None
  */
void JoystickTaskPb( void ) {

    for (;;) {

        if (s4433912_SemaphoreJoystickReadZ != NULL) {	/* Check if semaphore exists */

            /* See if we can obtain the PB semaphore. If the semaphore is not available
               wait 10 ticks to see if it becomes free. */
            if (xSemaphoreTake(s4433912_SemaphoreJoystickReadZ, portMAX_DELAY) == pdTRUE) {
                /* We were able to obtain the semaphore and can now access the shared resource. */
                // Use the handle to suspend the created task.

                if (taskTimerLeft != NULL && taskTimerRight != NULL &&
                    taskTimerDisplay != NULL && s4433912_TaskLta100g != NULL) {

                    if (timerSuspendSts == 0) {
                        vTaskSuspend(taskTimerLeft);
                        vTaskSuspend(taskTimerRight);
                        vTaskSuspend(taskTimerDisplay);
                        vTaskSuspend(s4433912_TaskLta100g);

                    } else {
                        vTaskResume(s4433912_TaskLta100g);
                        vTaskResume(taskTimerLeft);
                        vTaskResume(taskTimerRight);
                        vTaskResume(taskTimerDisplay);
                    }

                    timerSuspendSts = 1 - timerSuspendSts;
                }
                BRD_LEDBlueToggle();
            }
        }
    }
}

/**
 * @brief EXTI line detection callback
 * @param GPIO_Pin: Specifies the pins connected EXTI line
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    BaseType_t xHigherPriorityTaskWoken;

    if (GPIO_Pin == BRD_A3_PIN) {
        
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
}


/**
  * @brief  Timer increment Task that takes a timer struct and increments counter by a given delay
  * @param  pvParameters timer struct
  * @retval None
  */
void TaskTimerIncrement(void *pvParameters) {
    struct dualTimerMsg* timer;
    
    timer = (struct dualTimerMsg*) pvParameters;
    TickType_t delay = timer->delay / portTICK_PERIOD_MS;

    for (;;) {
        timer->timerValue |= (1 << timer->increment);
        timer->increment++;
        //reset when full
        if (timer->timerValue > 0b11111) {
            timer->timerValue = 0;
            timer->increment = 0;
        }

        HAL_GPIO_TogglePin(timer->port, timer->pin);

        vTaskDelay(delay);
    }
}


/**
  * @brief  Left timer task that counts every 100ms. Creates a task to run the timer independently 
  * 		while uploading the timer to the timer queue.
  * @param  None
  * @retval None
  */
void s4433912_TaskTimerLeft(void) {

    xTaskCreate( (void*) &TaskTimerIncrement, (const signed char *) "INCLEFT", mainSINGLE_STACK_SIZE, (void*) &timerLeft, 3, NULL );
    for (;;) {

        if (queueTimerDisplay != NULL) {
            if (xQueueSendToBack(queueTimerDisplay, (void*) &timerLeft, ( portTickType ) 10) != pdPASS ) {
                //Failed to send
            }
        }
    }
}


/**
  * @brief  Right timer task that counts every 1000ms. Creates a task to run the timer independently 
  * 		while uploading the timer to the timer queue.
  * @param  None
  * @retval None
  */
void s4433912_TaskTimerRight(void) {

    xTaskCreate( (void*) &TaskTimerIncrement, (const signed char *) "INCRIGHT", mainSINGLE_STACK_SIZE, (void*) &timerRight, 3, NULL );
    for (;;) {

        if (queueTimerDisplay != NULL) {
            if (xQueueSendToBack(queueTimerDisplay, (void*) &timerRight, ( portTickType ) 10) != pdPASS ) {
                //Failed to send
            }
        }
    }
}

/**
  * @brief  Timer display task that inteprets the left and right timer structs in display queue 
  * 		to display on the LTA1000g.
  * @param  None
  * @retval None
  */
void s4433912_TaskTimerDisplay(void) {
    unsigned short toDisplay = 0;

    for (;;) {

        if (queueTimerDisplay != NULL) {

            /*A block time is not necessary
            because this task will only run when the queue is full. */
            if (xQueueReceive(queueTimerDisplay, &timerRecv, 0) ==  pdPASS) {
                
                if (timerRecv.type == 'l') {
                    toDisplay &= ~(0b11111 << 5);; //Clear
                    toDisplay |= (timerRecv.timerValue << 5);
                    
                } else if (timerRecv.type =='r') {
                    toDisplay &= ~(0b11111); //Clear
                    toDisplay |= timerRecv.timerValue;
                }

                if (xQueueSendToFront(s4433912_QueueLta1000gReadMsg, (void*) &toDisplay, pdMS_TO_TICKS(1)) != pdPASS ) {
                    portENTER_CRITICAL();
                    debug_printf("TimerDisplay failed to send.\n\r");
                    portEXIT_CRITICAL();
                }

            }

        }
    }
}




/**
  * @brief  Hardware Initialisation.
  * @param  None
  * @retval None
  */
void Hardware_init( void ) {

    portDISABLE_INTERRUPTS();	//Disable interrupts

    BRD_LEDInit();				//Initialise Blue LED
    BRD_LEDBlueOn();				//Turn off Blue LED

    portENABLE_INTERRUPTS();	//Enable interrupts

}

/**
  * @brief  Application Tick Task.
  * @param  None
  * @retval None
  */
void vApplicationTickHook( void ) {

    BRD_LEDBlueOff();
}

/**
  * @brief  Idle Application Task
  * @param  None
  * @retval None
  */
void vApplicationIdleHook( void ) {
    static portTickType xLastTx = 0;

    BRD_LEDBlueOff();

    for (;;) {

        /* The idle hook simply prints the idle tick count, every second */
        if ((xTaskGetTickCount() - xLastTx ) > (1000 / portTICK_RATE_MS)) {

            xLastTx = xTaskGetTickCount();

            portENTER_CRITICAL();
            debug_printf("IDLE Tick %d\n", xLastTx);
            portEXIT_CRITICAL();

            /* Blink Alive LED */
            BRD_LEDBlueToggle();
        }
    }
}

/**
  * @brief  vApplicationStackOverflowHook
  * @param  Task Handler and Task Name
  * @retval None
  */
void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName ) {
    /* This function will get called if a task overflows its stack.   If the
    parameters are corrupt then inspect pxCurrentTCB to find which was the
    offending task. */

    BRD_LEDBlueOff();
    ( void ) pxTask;
    ( void ) pcTaskName;

    for( ;; );
}

