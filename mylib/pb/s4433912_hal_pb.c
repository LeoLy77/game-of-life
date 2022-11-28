/**
 ***************************************************************
 * @file mylib/pb/ s4433912_hal_pb.c
 * @author Leo Ly - 44339124
 * @date 29/02/2020
 * @brief LED Light Bar peripheral driver
 * REFERENCE: DON’T JUST COPY THIS BLINDLY.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 * s4433912_hal_pb_on_init(void)
 * s4433912_hal_pb_on_isr(uint16_t*, uint32_t*)
 * s4433912_hal_pb_iss_isr(uint16_t*, uint32_t*)
 * s4433912_hal_pb_isr_init(void)
 * s4433912_hal_pb_isr_deinit(void)
 ***************************************************************
*/

#include <stdint.h>
#include "s4433912_hal_pb.h"


#include "processor_hal.h"
#include "board.h"

#include "s4433912_hal_iss.h"

#define XENON_SOURCE S4433912_HAL_ISS_SOURCE_1
#define PB_SOURCE S4433912_HAL_ISS_SOURCE_2
#define XENON_DELAY 150
#define PB_DELAY 200


GPIO_InitTypeDef GPIO_InitStructure;
GPIO_InitTypeDef GPIO_isr_InitStructure;
/** @brief Enabled the onboard pushbutton source, e.g. enables GPIO input and interrupt.*/
extern void s4433912_hal_pb_on_init(void) {
	BRD_USER_BUTTON_GPIO_CLK_ENABLE();

	GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Pin  = BRD_USER_BUTTON_PIN;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(BRD_USER_BUTTON_GPIO_PORT, &GPIO_InitStructure);

	HAL_NVIC_SetPriority(BRD_USER_BUTTON_EXTI_IRQn, 10, 0);
	HAL_NVIC_EnableIRQ(BRD_USER_BUTTON_EXTI_IRQn);

	s4433912_hal_iss_init();
	s4433912_hal_iss_delay_timer_ctrl(XENON_SOURCE, XENON_DELAY);
	s4433912_hal_iss_delay_timer_ctrl(PB_SOURCE, PB_DELAY);

}

//Override default mapping of this handler to Default_Handler
void EXTI15_10_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(BRD_USER_BUTTON_PIN);
}

/** @brief Onboard Pushbutton Interrupt service routine. 
 * 	Must be called from the corresponding GPIO HAL interrupt service routine.
 *  @param dir: LEDs direction
 *  @param event_cnt: event counter*/
extern void s4433912_hal_pb_on_isr(uint16_t* dir, uint32_t* event_cnt) {
	if (s4433912_hal_iss_synchroniser(PB_SOURCE)) {
		*dir = 1 - *dir;
		*event_cnt = s4433912_hal_iss_eventcounter_read(PB_SOURCE);

	}
}

/** @brief Disabled the ISS pin source.*/
extern void s4433912_hal_pb_on_deinit(void) {
	HAL_GPIO_DeInit(BRD_A0_GPIO_PORT, &GPIO_isr_InitStructure);
}

/** @brief Enabled the ISS pin source, e.g. enables GPIO input and interrupt.*/
extern void s4433912_hal_pb_isr_init(void) {
  	__BRD_A0_GPIO_CLK();

	GPIO_isr_InitStructure.Mode = GPIO_MODE_IT_RISING;
  	GPIO_isr_InitStructure.Pull = GPIO_NOPULL;			//Enable Pull up, down or no pull resister
	GPIO_isr_InitStructure.Pin = BRD_A0_PIN;				//Pin
	GPIO_isr_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
  	HAL_GPIO_Init(BRD_A0_GPIO_PORT, &GPIO_isr_InitStructure);	//Initialise Pin

	HAL_NVIC_SetPriority(BRD_A0_EXTI_IRQ, 9, 0);
	HAL_NVIC_EnableIRQ(BRD_A0_EXTI_IRQ);
}

void EXTI3_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(BRD_A0_PIN);
}

/**
 * @brief ISS pin source Interrupt service routine. 
 * 		Must be called from the corresponding GPIO HAL interrupt service routine.
 * @param sts: debounce status pointer
 * @param event_cnt: event counter
*/
extern void s4433912_hal_pb_iss_isr(uint16_t* sts, uint32_t* event_cnt) {
	if (s4433912_hal_iss_synchroniser(XENON_SOURCE)) {
		*sts = 1;
		*event_cnt = s4433912_hal_iss_eventcounter_read(XENON_SOURCE);
	} else {
		*sts = 0;
	}
}

/**
 * @brief Disabled the onboard pushbutton source.*/
extern void s4433912_hal_pb_isr_deinit(void) {
	HAL_GPIO_DeInit(BRD_USER_BUTTON_GPIO_PORT, &GPIO_InitStructure);
}
