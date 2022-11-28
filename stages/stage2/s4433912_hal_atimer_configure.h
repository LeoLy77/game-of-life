/**
 ***************************************************************
 * @file stages/stage2/s4433912_hal_atimer_configure.h
 * @author Leo Ly - 44339124
 * @date 17/03/2020
 * @brief atimer hal configure
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 ***************************************************************
*/
// #include "board.h"
// #include "processor_hal.h"
#ifdef S4433912_ATIMER_CONFIGURE_INC
//PE13 I/O FT - TIM1_CH3, SPI4_MISO, FMC_D10, LCD_DE, EVENTOUT
#define S4433912_HAL_ATIMER_PIN BRD_D10_PIN
#define S4433912_HAL_ATIMER_PINPORT BRD_D10_GPIO_PORT
#define S4433912_HAL_ATIMER_PINCLK()  __BRD_D10_GPIO_CLK()
#define S4433912_HAL_ATIMER_CLKSPEED    25000
#define S4433912_HAL_ATIMER_PERIOD     2//ms

#endif