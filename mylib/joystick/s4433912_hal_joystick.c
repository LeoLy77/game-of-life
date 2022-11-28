/**
 ***************************************************************
 * @file mylib/joystick/s4433912_hal_joystick.c
 * @author Leo Ly - 44339124
 * @date 12/03/2020
 * @brief Joystick driver
 * REFERENCE: csse3010_mylib_hal_joystick.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 * s4433912_hal_joystick_init() - Initialise GPIO pins and ADC
 * int joystick_readxy(ADC Handler) - Internal generic function to read X or Y joystick value.
 ***************************************************************

The  Joystick  HAL  Driver  processes  the  Joystick  X,  Y  and  Z  inputs. 
Joystick  X  and  Y  areanalog voltage inputs and Z is a digital on/off input.
*/

/* Includes ------------------------------------------------------------------*/
#include "s4433912_hal_joystick.h"
// #include "processor_hal.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ADC_ChannelConfTypeDef AdcChanConfig;

/* Private function prototypes -----------------------------------------------*/

/**
  * @brief  Initialise ADC for Joystick pin X
  * @param  None
  * @retval None
  */
void VRx_init(void) {
	__ADC1_CLK_ENABLE();

	AdcHandleX.Instance = (ADC_TypeDef *)(ADC1_BASE);						//Use ADC1
	AdcHandleX.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV2;	//Set clock prescaler
	AdcHandleX.Init.Resolution            = ADC_RESOLUTION12b;				//Set 12-bit data resolution
	AdcHandleX.Init.ScanConvMode          = DISABLE;
	AdcHandleX.Init.ContinuousConvMode    = DISABLE;
	AdcHandleX.Init.DiscontinuousConvMode = DISABLE;
	AdcHandleX.Init.NbrOfDiscConversion   = 0;
	AdcHandleX.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;	//No Trigger
	AdcHandleX.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;		//No Trigger
	AdcHandleX.Init.DataAlign             = ADC_DATAALIGN_RIGHT;				//Right align data
	AdcHandleX.Init.NbrOfConversion       = 1;
	AdcHandleX.Init.DMAContinuousRequests = DISABLE;
	AdcHandleX.Init.EOCSelection          = DISABLE;

	HAL_ADC_Init(&AdcHandleX);		//Initialise ADC

	/* Configure ADC Channel */
	AdcChanConfig.Channel = BRD_A1_ADC_CHAN;			//Use A1 pin

	HAL_ADC_ConfigChannel(&AdcHandleX, &AdcChanConfig);		//Initialise ADC channel
}

/**
  * @brief  Initialise ADC for Joystick pin Y
  * @param  None
  * @retval None
  */
void VRy_init(void) {
	__ADC2_CLK_ENABLE();

	AdcHandleY.Instance = (ADC_TypeDef *)(ADC2_BASE);						//Use ADC1
	AdcHandleY.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV2;	//Set clock prescaler
	AdcHandleY.Init.Resolution            = ADC_RESOLUTION12b;				//Set 12-bit data resolution
	AdcHandleY.Init.ScanConvMode          = DISABLE;
	AdcHandleY.Init.ContinuousConvMode    = DISABLE;
	AdcHandleY.Init.DiscontinuousConvMode = DISABLE;
	AdcHandleY.Init.NbrOfDiscConversion   = 0;
	AdcHandleY.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;	//No Trigger
	AdcHandleY.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;		//No Trigger
	AdcHandleY.Init.DataAlign             = ADC_DATAALIGN_RIGHT;				//Right align data
	AdcHandleY.Init.NbrOfConversion       = 1;
	AdcHandleY.Init.DMAContinuousRequests = DISABLE;
	AdcHandleY.Init.EOCSelection          = DISABLE;

	HAL_ADC_Init(&AdcHandleY);		//Initialise ADC

	/* Configure ADC Channel */
	AdcChanConfig.Channel = BRD_A2_ADC_CHAN;

	HAL_ADC_ConfigChannel(&AdcHandleY, &AdcChanConfig);		//Initialise ADC channel
}

/**
  * @brief  Initialise Joystick
  * @param  None
  * @retval None
  */
void s4433912_hal_joystick_init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStructure.Pull = GPIO_NOPULL;

	AdcChanConfig.Rank         = 1;
	AdcChanConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	AdcChanConfig.Offset       = 0;

	__BRD_A1_GPIO_CLK();
	GPIO_InitStructure.Pin = BRD_A1_PIN;
	HAL_GPIO_Init(BRD_A1_GPIO_PORT, &GPIO_InitStructure);
	VRx_init();

	__BRD_A2_GPIO_CLK();
	GPIO_InitStructure.Pin = BRD_A2_PIN;
	HAL_GPIO_Init(BRD_A2_GPIO_PORT, &GPIO_InitStructure);
	VRy_init();

  	__BRD_A3_GPIO_CLK();
	GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Pin  = BRD_A3_PIN;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(BRD_A3_GPIO_PORT, &GPIO_InitStructure);

	HAL_NVIC_SetPriority(BRD_A3_EXTI_IRQ, 10, 0);
	HAL_NVIC_EnableIRQ(BRD_A3_EXTI_IRQ);

	s4433912_hal_iss_init();
	s4433912_hal_iss_delay_timer_ctrl(JOYSTICK_ISS_SOURCE, 300);

}

/**
  * @brief  Internal generic function to read X or Y joystick value.
  * @param  Handler - ADC Handler
  * @retval None
  */
uint32_t joystick_readxy(ADC_HandleTypeDef AdcHandler) {
	HAL_ADC_Start(&AdcHandler); // Start ADC conversion

	// Wait for ADC conversion to finish
	while (HAL_ADC_PollForConversion(&AdcHandler, 10) != HAL_OK);

	return (uint32_t)(HAL_ADC_GetValue(&AdcHandler));
}

//Override default mapping of this handler to Default_Handler
void EXTI3_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(BRD_A3_PIN);
}