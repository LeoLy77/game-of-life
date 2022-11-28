/**
 ***************************************************************
 * @file mylib/atimer/s4433912_hal_atimer.h
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
*/
#include "board.h"
#include <stdint.h>

#ifndef S4433912_HAL_ATIMER_H
#define S4433912_HAL_ATIMER_H

// #ifdef S4433912_ATIMER_CONFIGURE_INC
// #include "s4433912_hal_atimer_configure.h"
// // #endif
// #else

#ifdef S4433912_MYLIB_CONFIGURE_INC
#include "s4433912_hal_mylib_configure.h"
// #endif
#else

// #ifndef S4433912_ATIMER_CONFIGURE_INC

#define S4433912_HAL_ATIMER_PIN BRD_D0_PIN
#define S4433912_HAL_ATIMER_PINPORT BRD_D0_GPIO_PORT
#define S4433912_HAL_ATIMER_PINCLK()  __BRD_D0_GPIO_CLK()
#define S4433912_HAL_ATIMER_PERIOD  1000
#define S4433912_HAL_ATIMER_CLKSPEED    50000

#endif

void s4433912_hal_atimer_init();
int s4433912_hal_atimer_timer_getms();
void s4433912_hal_atimer_timer_reset();
void s4433912_hal_atimer_timer_pause();
void s4433912_hal_atimer_timer_resume();
void s4433912_hal_atimer_clkspeed_set(int frequency);
void s4433912_hal_atimer_period_set(int period);
void s4433912_hal_atimer_init_pin();
void atimer_isr_callback();

#endif
