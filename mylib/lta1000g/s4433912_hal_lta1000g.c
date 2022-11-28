/**
 ***************************************************************
 * @file mylib/ledbar/ s4433912_hal_ledbar .c
 * @author Leo Ly - 44339124
 * @date 29/02/2020
 * @brief LED Light Bar peripheral driver
 * REFERENCE: DON’T JUST COPY THIS BLINDLY.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 * s4433912_hal_lta1000g_init () - intialise LED bar
 * s4433912_hal_lta1000g_write () - set LED bar value
 ***************************************************************
*/

#include "s4433912_hal_lta1000g.h"


#include "processor_hal.h"
#include "board.h"

GPIO_TypeDef* ports[] = {BRD_D16_GPIO_PORT, BRD_D17_GPIO_PORT, BRD_D18_GPIO_PORT, BRD_D19_GPIO_PORT, BRD_D20_GPIO_PORT, 
    BRD_D21_GPIO_PORT, BRD_D22_GPIO_PORT, BRD_D23_GPIO_PORT, BRD_D24_GPIO_PORT, BRD_D25_GPIO_PORT};

uint32_t pins[10] = {BRD_D16_PIN, BRD_D17_PIN, BRD_D18_PIN, BRD_D19_PIN, BRD_D20_PIN, BRD_D21_PIN, 
		BRD_D22_PIN, BRD_D23_PIN, BRD_D24_PIN, BRD_D25_PIN};


extern void s4433912_hal_lta1000g_init(void) {
    GPIO_InitTypeDef  GPIO_InitStructure;

  	__BRD_D16_GPIO_CLK();
	__BRD_D17_GPIO_CLK();
	__BRD_D18_GPIO_CLK();
	__BRD_D19_GPIO_CLK();
	__BRD_D20_GPIO_CLK();
	__BRD_D21_GPIO_CLK();
	__BRD_D22_GPIO_CLK();
	__BRD_D23_GPIO_CLK();
	__BRD_D24_GPIO_CLK();
	__BRD_D25_GPIO_CLK();

  	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;		//Output Mode
  	GPIO_InitStructure.Pull = GPIO_PULLDOWN;			//Enable Pull up, down or no pull resister
  	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;			//Pin latency

	GPIO_InitStructure.Pin = BRD_D16_PIN;				//Pin
  	HAL_GPIO_Init(BRD_D16_GPIO_PORT, &GPIO_InitStructure);	//Initialise Pin

	GPIO_InitStructure.Pin = BRD_D17_PIN;				//Pin
  	HAL_GPIO_Init(BRD_D17_GPIO_PORT, &GPIO_InitStructure);	//Initialise Pin

	GPIO_InitStructure.Pin = BRD_D18_PIN;				//Pin
  	HAL_GPIO_Init(BRD_D18_GPIO_PORT, &GPIO_InitStructure);	//Initialise Pin

	GPIO_InitStructure.Pin = BRD_D19_PIN;				//Pin
  	HAL_GPIO_Init(BRD_D19_GPIO_PORT, &GPIO_InitStructure);	//Initialise Pin

	GPIO_InitStructure.Pin = BRD_D20_PIN;				//Pin
  	HAL_GPIO_Init(BRD_D20_GPIO_PORT, &GPIO_InitStructure);	//Initialise Pin

	GPIO_InitStructure.Pin = BRD_D21_PIN;				//Pin
  	HAL_GPIO_Init(BRD_D21_GPIO_PORT, &GPIO_InitStructure);	//Initialise Pin

	GPIO_InitStructure.Pin = BRD_D22_PIN;				//Pin
  	HAL_GPIO_Init(BRD_D22_GPIO_PORT, &GPIO_InitStructure);	//Initialise Pin

	GPIO_InitStructure.Pin = BRD_D23_PIN;				//Pin
  	HAL_GPIO_Init(BRD_D23_GPIO_PORT, &GPIO_InitStructure);	//Initialise Pin

	GPIO_InitStructure.Pin = BRD_D24_PIN;				//Pin
  	HAL_GPIO_Init(BRD_D24_GPIO_PORT, &GPIO_InitStructure);	//Initialise Pin

	GPIO_InitStructure.Pin = BRD_D25_PIN;				//Pin
  	HAL_GPIO_Init(BRD_D25_GPIO_PORT, &GPIO_InitStructure);	//Initialise Pin

}

void lta1000g_seg_set(int segment, unsigned char segmentvalue) {
    /***Set the value (0 or 1) for the corresponding
        LED Bar segment GPIO pin. (Internal
        function)*/
    
    HAL_GPIO_WritePin(ports[segment], pins[segment], segmentvalue);

}

extern void s4433912_hal_lta1000g_write(unsigned short value) {
    for (int i = 0 ; i < 10; i++) {
		// HAL_GPIO_WritePin(ports[i], pins[i], (value >> i) & 1);
		lta1000g_seg_set(i, (value >> i) & 1);
	}
}