/**
 ***************************************************************
 * @file mylib/atimer/s4433912_hal_atimer.c
 * @author Leo Ly - 44339124
 * @date 12/03/2020
 * @brief Alive timer driver
 * REFERENCE: csse3010_mylib_hal_atimer.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 * s4433912_hal_atimer_init()
 * s4433912_hal_atimer_timer_getms()
 * s4433912_hal_atimer_timer_reset()
 * s4433912_hal_atimer_timer_pause()
 * s4433912_hal_atimer_timer_resume()
 * s4433912_hal_atimer_clkspeed_set(int frequency)
 * s4433912_hal_atimer_period_set(int period)
 * s4433912_hal_atimer_init_pin()
 * atimer_isr_callback()
 ***************************************************************

 The alive timer is a timer that uses a timer interrupt to keep track of time, 
 from when the alive timer is initialised. 
 The Mylib Alive Timer HAL Driver is used to configure, 
 control and access various features of the alive timer. 
 Timer also outputs toggles a pin, when the interruptoccurs. 
 The pin used, must be set at compile time, using compiler directives.
*/

/* Includes ------------------------------------------------------------------*/
// #include <stdint.h>
#include "s4433912_hal_atimer.h"
#include "processor_hal.h"
#include "debug_printf.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
int atimerClkspeedVal;
int atimerPeriodVal;
int atimerPrescalerVal;
volatile int atimerCounterVal;

volatile uint8_t atimerSts; //boolean timer status 

TIM_HandleTypeDef Atimer_TIM_Init;
GPIO_InitTypeDef atimer_TIM_GPIO_InitStructure;

/* Private function prototypes -----------------------------------------------*/
#define s4433912_hal_atimer_timer_read() __HAL_TIM_GET_COUNTER(&Atimer_TIM_Init)

/**
  * @brief  Calculate frequency in double to feed to Init.Period
  * @param  period - period to be set
  * @retval frequency in double
  */
double cal_half_freq_double(int period) {
	return 0.5 * (1.0 / (((float)period) /1000.0));
}

/**
  * @brief  Initialise atimer
  * @param  None
  * @retval None
  */
void s4433912_hal_atimer_init() {
	__TIM3_CLK_ENABLE();

	//Default values
	atimerPrescalerVal = (uint16_t) ((SystemCoreClock/2) / S4433912_HAL_ATIMER_CLKSPEED) - 1;
	atimerCounterVal = 0;
	atimerSts = 1;

	atimerPeriodVal = S4433912_HAL_ATIMER_PERIOD;
	atimerClkspeedVal = S4433912_HAL_ATIMER_CLKSPEED;

	/* TIM Base configuration */
	Atimer_TIM_Init.Instance = TIM3;

  	Atimer_TIM_Init.Init.Period = (atimerClkspeedVal/ cal_half_freq_double(atimerPeriodVal)) - 1;
  	Atimer_TIM_Init.Init.Prescaler = atimerPrescalerVal;	//Set prescaler value
  	Atimer_TIM_Init.Init.ClockDivision = 0;			//Set clock division
	Atimer_TIM_Init.Init.RepetitionCounter = 0;	// Set reload Value
  	Atimer_TIM_Init.Init.CounterMode = TIM_COUNTERMODE_UP;	//Set timer to count up.

	/* Initialise Timer */
	HAL_TIM_Base_Init(&Atimer_TIM_Init);

	HAL_NVIC_SetPriority(TIM3_IRQn, 10, 0);//Set Main priority to 10 and sub-priority to 0.
	HAL_NVIC_EnableIRQ(TIM3_IRQn);
	HAL_TIM_Base_Start_IT(&Atimer_TIM_Init);

}

//Override default mapping of this handler to Default_Handler
void TIM3_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&Atimer_TIM_Init);
}

/**
  * @brief  Return the atimer counter value in ms
  * @param  None
  * @retval period (ms)
  */
int s4433912_hal_atimer_timer_getms() {
	return atimerCounterVal * atimerPeriodVal;
}

/**
  * @brief  Reset the atimer counter value to zero
  * @param  None
  * @retval None
  */
void s4433912_hal_atimer_timer_reset() {
	atimerCounterVal = 0;
}

/**
  * @brief  Pause the atimer
  * @param  None
  * @retval None
  */
void s4433912_hal_atimer_timer_pause() {
	atimerSts = 0;
}

/**
  * @brief  Resume the atimer
  * @param  None
  * @retval None
  */
void s4433912_hal_atimer_timer_resume() {
	atimerSts = 1;
}

/**
  * @brief  Set the atimer clock speed (Hz)
  * @param  frequency - new frequency (Hz) to be updated
  * @retval None
  */
void s4433912_hal_atimer_clkspeed_set(int frequency) {
	//Update frequency
	atimerClkspeedVal = frequency;
	atimerPrescalerVal = (uint16_t) ((SystemCoreClock / 2) / atimerClkspeedVal) - 1;

  	Atimer_TIM_Init.Init.Prescaler = atimerPrescalerVal;	//Set prescaler value
	Atimer_TIM_Init.Init.Period = (atimerClkspeedVal/ cal_half_freq_double(atimerPeriodVal)) - 1;

	/* ReInitialise Timer */
	HAL_TIM_Base_Init(&Atimer_TIM_Init);
}

/**
  * @brief  Set the atimer period (ms)
  * @param  period - new period (ms) to be updated
  * @retval None
  */
void s4433912_hal_atimer_period_set(int period) {
	//Update period
	atimerPeriodVal = period;
	Atimer_TIM_Init.Init.Period = (atimerClkspeedVal/ cal_half_freq_double(atimerPeriodVal)) - 1;
	/* ReInitialise Timer */
	HAL_TIM_Base_Init(&Atimer_TIM_Init);
}


/**
  * @brief  Initialise the atimer GPIO pin.
  * @param  None
  * @retval None
  */
void s4433912_hal_atimer_init_pin() {

	S4433912_HAL_ATIMER_PINCLK();

	atimer_TIM_GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	atimer_TIM_GPIO_InitStructure.Pull = GPIO_PULLDOWN;
	atimer_TIM_GPIO_InitStructure.Pin  = S4433912_HAL_ATIMER_PIN;
	atimer_TIM_GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(S4433912_HAL_ATIMER_PINPORT, &atimer_TIM_GPIO_InitStructure);
}

/**
  * @brief  atimer interrupt service routine
  * @param  None
  * @retval None
  */
void atimer_isr_callback() {
	if (atimerSts) {
		atimerCounterVal++;
		// if (atimerCounterVal % (0.5 * (1/atimerPeriodVal))) {
			HAL_GPIO_TogglePin(S4433912_HAL_ATIMER_PINPORT, S4433912_HAL_ATIMER_PIN);
		// }
	}
}

