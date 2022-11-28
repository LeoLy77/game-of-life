/**
 ***************************************************************
 * @file mylib/iss/s4433912_hal_iss .c
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
#include <stdint.h>
#include "s4433912_hal_iss.h"
#include "debug_printf.h"



#include "processor_hal.h"
#include "board.h"


/**Global 32-bit Internal counter array thatholds  three  signal  event  
 * counter  values.Each  time  an  event  occurs,  the  corre-sponding signal 
 * event counter must be in-cremented.*/
uint32_t iss_eventcounter_val[3]; 

/**Global 32-bit Internal array that holds the 
 * last timer value for each source (default:0).*/
uint32_t iss_lasttimer_val[3]; 

/**Global 32-bit Internal array that holds the
 * delay value used by each source’s synchroniser function
 * (default:  10).*/
uint32_t iss_delay_val[3]; 

/** @brief Initialise the input signal synchroniser counters.
 * @param signal_source_index: source index
*/
extern void s4433912_hal_iss_init(void) {
	for (int i = 0; i < 3; i++) {
		iss_delay_val[i] = 10;
		iss_eventcounter_val[i] = 0;
		iss_lasttimer_val[i] = 0;
	}
}

/** @brief Synchronises the input signal. Must becalled from the signal source’s interrupt.
 * The signal counter index must be specified.  
 * Each signal source must have a signal event counter index assigned.  
 * A valid signal must last more than 50ms. Uses delay value.
 * @param signal_source_index: source index
 * */
extern uint16_t s4433912_hal_iss_synchroniser(unsigned char signal_source_index) {
	uint32_t event_time = HAL_GetTick();
	uint32_t valid_time = iss_lasttimer_val[signal_source_index] + iss_delay_val[signal_source_index];

	if (event_time > valid_time) {

		iss_eventcounter_val[signal_source_index]++;
		if (iss_eventcounter_val[signal_source_index] == 10) {
			s4433912_hal_iss_eventcounter_reset(signal_source_index);
		}
		iss_lasttimer_val[signal_source_index] = event_time;

		return 1;
	}
	return 0;
};

/** @brief Returns the specified event counter value.
 * @param signal_source_index: source index
*/
extern uint32_t s4433912_hal_iss_eventcounter_read(unsigned char signal_source_index) {
	return iss_eventcounter_val[signal_source_index];
};

/** @brief Returns the specified last time value.
 * @param signal_source_index: source index
*/
extern uint32_t s4433912_hal_iss_lasttimer_read(unsigned char signal_source_index) {
	return iss_lasttimer_val[signal_source_index];
};

/** @brief Reset the specified event counter value.
 * @param signal_source_index: source index
*/
extern void s4433912_hal_iss_eventcounter_reset(unsigned char signal_source_index) {
	iss_eventcounter_val[signal_source_index] = 0;
};

/** @brief Reset the specified last timer value.
 * @param signal_source_index: source index
*/
extern void s4433912_hal_iss_lasttimer_reset(unsigned char signal_source_index) {
	iss_lasttimer_val[signal_source_index] = 0;
};

/** @brief Set the delay value for the specified source.
 * @param signal_source_index: source index
 * @param delay_value: delay value to be set
*/
extern void s4433912_hal_iss_delay_timer_ctrl(unsigned char signal_source_index, int delay_value) {
	iss_delay_val[signal_source_index] = delay_value;
};