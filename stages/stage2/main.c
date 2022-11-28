/**
  ******************************************************************************
  * @file    main.c
  * @author  44339124 - Leo Ly
  * @date    12/03/2020
  * @brief   stage2 main.c
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "processor_hal.h"
#include "debug_printf.h"

// #ifdef S4433912_ATIMER_CONFIGURE_INC
// #include "s4433912_hal_atimer_configure.h"
// #endif

// #ifdef S4433912_PWM_CONFIGURE_INC
// #include "s4433912_hal_pwm_configure.h"
// #endif

#ifdef S4433912_MYLIB_CONFIGURE_INC
#include "s4433912_hal_mylib_configure.h"
#endif


#include "s4433912_hal_atimer.h"
#include "s4433912_hal_joystick.h"
#include "s4433912_hal_pwm.h"
#include "s4433912_hal_lta1000g.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define JOYSTICK_X BRD_A1_PIN
#define JOYSTICK_Y BRD_A2_PIN
#define JOYSTICK_Z BRD_A3_PIN

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
int hal_atimerClkspeedVal;
int hal_atimerPeriodVal;
uint16_t ltaControl;
uint16_t x_reading, y_reading;

/* Private function prototypes -----------------------------------------------*/
void Hardware_init(void);
void atimer_control (uint8_t command);
void meter_display(void);

/**
  * @brief  Main program - flashes onboard LEDs
  * @param  None
  * @retval None
  */
void main(void)  {
	BRD_init();			//Initalise Board
	Hardware_init();	//Initalise hardware modules

	HAL_Delay(20);

	char RxChar;
	hal_atimerClkspeedVal = S4433912_HAL_ATIMER_CLKSPEED;
	hal_atimerPeriodVal = S4433912_HAL_ATIMER_PERIOD;
	ltaControl = 0b1111111111;
	/* Main processing loop */

    while (1) {		
		x_reading = S4433912_HAL_JOYSTICK_X_READ();
		y_reading = S4433912_HAL_JOYSTICK_Y_READ();

		S4433912_HAL_PWM_DC_SET((x_reading*S4433912_HAL_PWM_PULSEPERIOD - 500)/4065);
		meter_display();

		RxChar = debug_getc();
		if (RxChar != '\0') {
			// debug_printf("%c", RxChar);// reflect byte using printf - must delay before calling printf again
			atimer_control((uint8_t) RxChar);
		}
	}
}

/**
  * @brief  interpret ADC reading to display on lta1000g
  * @param  None
  * @retval None
  */
void meter_display(void) {
	uint16_t ltaWrite;

	uint8_t shift_index = (y_reading * 10)/4095;
	//XOR operation to display the LEDs from right to left, and proportional to ADC value
	ltaWrite = ltaControl & ((0b1111111111 << shift_index)^(0b1111111111));
	s4433912_hal_lta1000g_write(ltaWrite);
}

/**
  * @brief  control atimer period/clkspeed with USART input
  * @param  command - single control character 
  * @retval None
  */
void atimer_control (uint8_t command) {
	switch (command) {
		case 'f':
			s4433912_hal_atimer_timer_pause(); //pause timer
			break;
		case 'r':
			s4433912_hal_atimer_timer_resume(); //resume timer
			break;
		case 'z':
			s4433912_hal_atimer_timer_reset(); //reset timer
			break;
		case 'c':
			debug_printf("%d ticks\n\r", s4433912_hal_atimer_timer_getms()*2); //print current timer value in ticks
			break;
		case 't':
			debug_printf("%d ms\n\r", s4433912_hal_atimer_timer_getms()); //print current timer value in ms
			break;
		case '+':
			hal_atimerPeriodVal += 10;
			s4433912_hal_atimer_period_set(hal_atimerPeriodVal); //increase period by 10ms
			break;
		case '-':
			hal_atimerPeriodVal -= 10;
			s4433912_hal_atimer_period_set(hal_atimerPeriodVal); //decrease period by 10ms
			break;
		case 'i':
			hal_atimerClkspeedVal += 1000;
			s4433912_hal_atimer_clkspeed_set(hal_atimerClkspeedVal); //increase clkspeed by 1000Hz
			break;
		case 'd':
			hal_atimerClkspeedVal -= 1000;
			s4433912_hal_atimer_clkspeed_set(hal_atimerClkspeedVal);//decrease clkspeed by 1000Hz
			break;

		default:
			break;
	}
}

/**
  * @brief  Initialise Hardware
  * @param  None
  * @retval None
  */
void Hardware_init(void) {

	BRD_LEDInit();	
	/* Turn off LEDs */
	BRD_LEDRedOff();
	BRD_LEDGreenOff();
	BRD_LEDBlueOff();

	s4433912_hal_pwm_init();
	s4433912_hal_lta1000g_init();
	s4433912_hal_joystick_init();
	s4433912_hal_atimer_init_pin();
	s4433912_hal_atimer_init();
}


/**
 * @brief Period elapsed callback in non blocking mode
 * @param htim: Pointer to a TIM_HandleTypeDef that contains 
 * 			the configuration information for the TIM module.
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM3) {
		atimer_isr_callback();
	}
}

/**
 * @brief EXTI line detection callback
 * @param GPIO_Pin: Specifies the pins connected EXTI line
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{	
	if (GPIO_Pin == JOYSTICK_Z) {
		if (S4433912_HAL_JOYSTICK_Z_READ()) {
			ltaControl ^= ltaControl | 0b1111111111;
			meter_display();
		}
	}
}


