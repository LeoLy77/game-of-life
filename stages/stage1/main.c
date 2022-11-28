/**
  ******************************************************************************
  * @file    ex_HAL/main.c
  * @author  MDS
  * @date    27112019
  * @brief   HAL template.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "processor_hal.h"
#include "debug_printf.h"

#include "s4433912_hal_pb.h"
#include "s4433912_hal_lta1000g.h"


#include <string.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define XENON_PIN BRD_A0_PIN
#define PB_PIN GPIO_PIN_13

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* LEDs Direction: 1 right, 0 left*/
uint16_t dir = 1;
/*Event counters*/
uint32_t pb_event_cnt = 0;
uint32_t xenon_event_cnt = 0;
/* Private function prototypes -----------------------------------------------*/
void Hardware_init(void);

/**
  * @brief  Main program - flashes onboard LEDs
  * @param  None
  * @retval None
  */
void main(void)  {

	BRD_init();			//Initalise Board
	Hardware_init();	//Initalise hardware modules

	/* Main processing loop */
	
	int write_bits;

	if (dir == 0) {
		write_bits = 0b1100000000;
	} else {
		write_bits = 0b0000000011;
	}


    while (1) {		

		s4433912_hal_lta1000g_write(write_bits);
		debug_printf("LEDs Pattern: 0x%04x\n\r", write_bits);
		debug_printf("XENON cnt: %d  PB cnt: %d\n\r", xenon_event_cnt, pb_event_cnt);

		if (dir == 0) {
			write_bits = write_bits >> 1;
		} else {
			write_bits = write_bits << 1;
		}

		if (dir == 0 && write_bits == 0) {
			write_bits = 0b1100000000;
		} else if (dir == 1 && write_bits == 0b110000000000) {
			write_bits = 0b0000000011;
		}

		HAL_Delay(200);
  	}
}

/**
  * @brief  Initialise Hardware
  * @param  None
  * @retval None
  */
void Hardware_init(void) {
	s4433912_hal_lta1000g_init();
	s4433912_hal_pb_on_init();
	s4433912_hal_pb_isr_init();

	BRD_LEDInit();	
	/* Turn off LEDs */
	BRD_LEDRedOff();
	BRD_LEDGreenOff();
	BRD_LEDBlueOff();
}

/**
 * @brief EXTI line detection callback
 * @param GPIO_Pin: Specifies the pins connected EXTI line
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{	
	uint16_t xenon_sts = 0;
	if (GPIO_Pin == XENON_PIN) {
		s4433912_hal_pb_iss_isr(&xenon_sts, &xenon_event_cnt);
		if (xenon_sts) {
			BRD_LEDToggle(BRD_RED_LEDMASK);
		}


	} else if (GPIO_Pin == PB_PIN) {
		s4433912_hal_pb_on_isr(&dir, &pb_event_cnt);

	}  
}

