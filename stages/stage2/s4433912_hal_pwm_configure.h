/**
 ***************************************************************
 * @file stages/stage2/s4433912_hal_pwm_configure.h
 * @author Leo Ly - 44339124
 * @date 17/03/2020
 * @brief stage 2 hal pwm configure
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 ***************************************************************
*/
// #include "board.h"
// #include "processor_hal.h"
#ifdef S4433912_PWM_CONFIGURE_INC

TIM_HandleTypeDef TIM_Init;

//D6 - PE9 I/O FT - TIM1_CH1, FMC_DPE9 I/O FT - TIM1_CH1, FMC_D
#define S4433912_HAL_PWM_CLOCKFREQ 50000
#define S4433912_HAL_PWM_PERIOD (S4433912_HAL_PWM_CLOCKFREQ/200)-1 //200 Hz
#define S4433912_HAL_PWM_PULSEPERIOD ((S4433912_HAL_PWM_PERIOD+1)*100)/100 - 1 //100%
#define S4433912_HAL_PWM_CHANNEL TIM_CHANNEL_1
#define S4433912_HAL_PWM_TIMER TIM1
#define S4433912_HAL_PWM_TIMER_CLK() __BRD_D6_GPIO_CLK()
#define S4433912_HAL_PWM_TIMER_HANDLER TIM_Init
#define S4433912_HAL_PWM_PIN BRD_D6_PIN
#define S4433912_HAL_PWM_PINGPIOPORT BRD_D6_GPIO_PORT
#define S4433912_HAL_PWM_PINGPIOAF GPIO_AF1_TIM1
#define S4433912_HAL_PWM_PINCLK() __TIM1_CLK_ENABLE()
#define S4433912_HAL_PWM_PINCLKSPEED GPIO_SPEED_FAST

#endif