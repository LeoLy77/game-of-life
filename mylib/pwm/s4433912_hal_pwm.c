/**
 ***************************************************************
 * @file mylib/pwm/s4433912_hal_pwm.c
 * @author Leo Ly - 44339124
 * @date 14/03/2020
 * @brief PWM driver
 * REFERENCE: csse3010_mylib_hal_pwm.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 * s4433912_hal_pwm_init() - Initialise and start PWM output(s)
 *  * S4433912_HAL_PWM_DC_GET() - get current pwm value
 * S4433912_HAL_PWM_DC_SET(VALUE) - set pwm value
 ***************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "processor_hal.h"
#include <stdint.h>
#include "debug_printf.h"

#include "s4433912_hal_pwm.h"
// #include "s4433912_hal_iss.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// TIM_HandleTypeDef TIM_Init;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
static void Error_Handler(void)
{
	debug_printf("ERRRORRRRR\n\r");
	while (1)
	{
	}
}

/**
  * @brief  Initialise Joystick
  * @param  None
  * @retval None
  */
void s4433912_hal_pwm_init() {
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_OC_InitTypeDef PWMConfig;

	uint16_t PrescalerValue = 0;

	S4433912_HAL_PWM_PINCLK();
	S4433912_HAL_PWM_TIMER_CLK();

	GPIO_InitStruct.Pin 		= S4433912_HAL_PWM_PIN;
	GPIO_InitStruct.Mode 		= GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull 		= GPIO_NOPULL;
	GPIO_InitStruct.Speed 		= S4433912_HAL_PWM_PINCLKSPEED;
	GPIO_InitStruct.Alternate 	= S4433912_HAL_PWM_PINGPIOAF;
	HAL_GPIO_Init(S4433912_HAL_PWM_PINGPIOPORT, &GPIO_InitStruct);

	PrescalerValue = (uint16_t)((SystemCoreClock) / S4433912_HAL_PWM_CLOCKFREQ) - 1;

	S4433912_HAL_PWM_TIMER_HANDLER.Instance 			  = S4433912_HAL_PWM_TIMER;
	S4433912_HAL_PWM_TIMER_HANDLER.Init.Period            = (S4433912_HAL_PWM_CLOCKFREQ/2)-1;//S4433912_HAL_PWM_PERIOD;
	S4433912_HAL_PWM_TIMER_HANDLER.Init.Prescaler         = PrescalerValue;
	S4433912_HAL_PWM_TIMER_HANDLER.Init.ClockDivision     = 0;
	S4433912_HAL_PWM_TIMER_HANDLER.Init.RepetitionCounter = 0;
	S4433912_HAL_PWM_TIMER_HANDLER.Init.CounterMode       = TIM_COUNTERMODE_UP;

	PWMConfig.OCMode       = TIM_OCMODE_PWM1;
	PWMConfig.Pulse 	   = S4433912_HAL_PWM_PULSEPERIOD;
	PWMConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
	PWMConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
	PWMConfig.OCFastMode   = TIM_OCFAST_DISABLE;
	PWMConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;
	PWMConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;

	HAL_TIM_PWM_Init(&S4433912_HAL_PWM_TIMER_HANDLER);
	HAL_TIM_PWM_ConfigChannel(&S4433912_HAL_PWM_TIMER_HANDLER, &PWMConfig, S4433912_HAL_PWM_CHANNEL);
	HAL_TIM_PWM_Start(&S4433912_HAL_PWM_TIMER_HANDLER, S4433912_HAL_PWM_CHANNEL);
}
