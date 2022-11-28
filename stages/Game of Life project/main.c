/**
	******************************************************************************
	* @file    stages/project/main.c
	* @author  Leo Ly - 44339124
	* @date    02/05/2020
	* @brief   Game of Life CAG project
	******************************************************************************
	*
	*/

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "debug_printf.h"
#include "processor_hal.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "s4433912_CAG_share.h"
#include "s4433912_CAG_keypad_grid.h"
#include "s4433912_CAG_keypad_mnemonic.h"
#include "s4433912_CAG_Simulator.h"
#include "s4433912_CAG_Display.h"
#include "s4433912_CAG_joystick.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void Hardware_init();
void ApplicationIdleHook(void); /* The idle hook is used to blink the Blue 'Alive LED' every second */


/**
	* @brief  Starts all the other tasks, then starts the scheduler.
	* @param  None
	* @retval None
	*/
int main( void ) {
	
	BRD_init();
	Hardware_init();

	xTaskCreate( (void*) &s4433912_os_TaskKeypadInit, (const signed char *) "KEYPADINIT", mainDOUBLE_STACK_SIZE, NULL, mainHIGHTASK_PRIORITY, &s4433912_TaskKeypadInit);
	xTaskCreate( (void*) &s4433912_os_CAG_TaskPbRecv, (const signed char *) "PB", mainSINGLE_STACK_SIZE, NULL, mainHIGHTASK_PRIORITY, &s4433912_TaskCAGPbRecv);
	xTaskCreate( (void*) &s4433912_os_CAG_TaskLedSend, (const signed char *) "LED", mainDOUBLE_STACK_SIZE, NULL, mainHIGHTASK_PRIORITY, &s4433912_TaskLedSend);
	xTaskCreate( (void*) &s4433912_os_KPgrid_TaskGridSend, (const signed char *) "KP_GRID", mainDOUBLE_STACK_SIZE, NULL, mainMEDIUMTASK_PRIORITY, &s4433912_TaskCAGGridSend);
	xTaskCreate( (void*) &s4433912_os_CAGMnc_TaskSendToSim, (const signed char *) "KP_MNC", mainSINGLE_STACK_SIZE*7, NULL, mainMEDIUMTASK_PRIORITY, &s4433912_TaskCAGMncSend);
	s4433912_os_JoystickTasksInit();
	s4433912_os_SimTasks_init();
	s4433912_os_CAGDspTasks_init();
	/* Start the scheduler.

	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used here. */

	vTaskStartScheduler();

	/* We should never get here as control is now taken by the scheduler. */
	return 0;
}


/**
 * @brief EXTI line detection callback
 * @param GPIO_Pin: Specifies the pins connected EXTI line
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

	if (GPIO_Pin == BRD_USER_BUTTON_PIN) {

		s4433912_CAGPushbutton_callback();
	} else if (GPIO_Pin == JOYSTICK_Z_PIN) {

		s4433912_os_CAG_Joystick_Z_callback();
	}
}


/**
	* @brief  Hardware Initialisation.
	* @param  None
	* @retval None
	*/
void Hardware_init( void ) {

	portDISABLE_INTERRUPTS();	//Disable interrupts
	s4433912_CAGPushbutton_init();
	s4433912_FreeRTOS_init();
	portENABLE_INTERRUPTS();	//Enable interrupts

}

/**
	* @brief  Override ExTI handler
	* @param  None
	* @retval None
	*/
void EXTI15_10_IRQHandler(void) {
	
	HAL_GPIO_EXTI_IRQHandler(BRD_USER_BUTTON_PIN);
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

