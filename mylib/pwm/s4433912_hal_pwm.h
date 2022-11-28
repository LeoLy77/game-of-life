/**
 ***************************************************************
 * @file mylib/pwm/s4433912_hal_pwm.h
 * @author Leo Ly - 44339124
 * @date 14/03/2020
 * @brief PWM driver
 * REFERENCE: csse3010_mylib_hal_pwm.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 * s4433912_hal_pwm_init() - Initialise and start PWM output(s)
 * S4433912_HAL_PWM_DC_GET() - get current pwm value
 * S4433912_HAL_PWM_DC_SET(VALUE) - set pwm value
 ***************************************************************
*/

#ifndef S4433912_HAL_PWM_H
#define S4433912_HAL_PWM_H

// #ifdef S4433912_PWM_CONFIGURE_INC
// #include "s4433912_hal_pwm_configure.h"
// // #endif
// #else 

#ifdef S4433912_MYLIB_CONFIGURE_INC
#include "s4433912_hal_mylib_configure.h"
#else


// #ifndef S4433912_PWM_CONFIGURE_INC
// TIM_HandleTypeDef TIM_Init;

#define S4433912_HAL_PWM_CLOCKFREQ 100
#define S4433912_HAL_PWM_PULSEPERIOD 100
#define S4433912_HAL_PWM_PERIOD 100
#define S4433912_HAL_PWM_CHANNEL 0
#define S4433912_HAL_PWM_TIMER 0
#define S4433912_HAL_PWM_TIMER_CLK() 0
#define S4433912_HAL_PWM_TIMER_HANDLER 0
#define S4433912_HAL_PWM_PIN BRD_D0_PIN
#define S4433912_HAL_PWM_PINGPIOPORT BRD_D0_GPIO_PORT
#define S4433912_HAL_PWM_PINGPIOAF 0
#define S4433912_HAL_PWM_PINCLK() 0
#define S4433912_HAL_PWM_PINCLKSPEED 0

#endif

#define S4433912_HAL_PWM_DC_GET()   __HAL_TIM_GET_COMPARE(&S4433912_HAL_PWM_TIMER_HANDLER, S4433912_HAL_PWM_CHANNEL)
#define S4433912_HAL_PWM_DC_SET(VALUE)  __HAL_TIM_SET_COMPARE(&S4433912_HAL_PWM_TIMER_HANDLER, S4433912_HAL_PWM_CHANNEL, VALUE)

void s4433912_hal_pwm_init();

#endif
