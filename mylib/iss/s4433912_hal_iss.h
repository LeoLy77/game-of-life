/**
 ***************************************************************
 * @file mylib/iss/s4433912_hal_iss.h
 * @author Leo Ly - 44339124
 * @date 29/02/2020
 * @brief ISS driver
 * REFERENCE: DON’T JUST COPY THIS BLINDLY.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 * s4433912_hal_iss_init () - intialise ISS
 * s4433912_hal_iss_synchroniser(unsigned char)
 * s4433912_hal_iss_eventcounter_read(unsigned char)
 * s4433912_hal_iss_lasttimer_read(unsigned char)
 * s4433912_hal_iss_eventcounter_reset(unsigned char)
 * s4433912_hal_iss_lasttimer_reset(unsigned char)
 * s4433912_hal_iss_delay_timer_ctrl(unsigned char, int)
 * 
 ***************************************************************
*/

#ifndef S4433912_HAL_ISS_H
#define S4433912_HAL_ISS_H

#define S4433912_HAL_ISS_SOURCE_1 0
#define S4433912_HAL_ISS_SOURCE_2 1
#define S4433912_HAL_ISS_SOURCE_3 2

extern void s4433912_hal_iss_init(void);

extern uint16_t s4433912_hal_iss_synchroniser(unsigned char signal_source_index);

extern uint32_t s4433912_hal_iss_eventcounter_read(unsigned char);

extern uint32_t s4433912_hal_iss_lasttimer_read(unsigned char);

extern void s4433912_hal_iss_eventcounter_reset(unsigned char);

extern void s4433912_hal_iss_lasttimer_reset(unsigned char);

extern void s4433912_hal_iss_delay_timer_ctrl(unsigned char, int);


#endif
