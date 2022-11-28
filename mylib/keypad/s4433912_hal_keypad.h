/**
 ***************************************************************
 * @file mylib/keypad/s4433912_hal_keypad.h
 * @author Leo Ly - 44339124
 * @date 31/03/2020
 * @brief keypad driver
 * REFERENCE: csse3010_mylib_hal_keypad.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 * s4433912_hal_keypad_init() - Initialise keypad FSM
 * s4433912_hal_keypad_deinit() - Deinitialise keypad FSM
 * keypad_gpio_init() - Initialise keypad GPIOs
 * s4433912_hal_keypad_fsmprocessing() - keypad FSM
 * keypad_writecol(uint8_t colval) - column control
 * keypad_readrow() - read row pins
 * s4433912_hal_keypad_read_status() - get keypad status
 * s4433912_hal_keypad_read_key() - get pressed key
 * s4433912_hal_keypad_read_ascii() - get pressed key in ascii
 ***************************************************************
*/


#ifndef S4433912_HAL_KEYPAD_H
#define S4433912_HAL_KEYPAD_H

#ifdef S4433912_MYLIB_CONFIGURE_INC
#include "s4433912_hal_mylib_configure.h"
#else 
//Column parameters
#define S4433912_HAL_KEYPAD_COL1PIN D0
#define S4433912_HAL_KEYPAD_COL2PIN D0
#define S4433912_HAL_KEYPAD_COL3PIN D0
#define S4433912_HAL_KEYPAD_COL4PIN D0

#define S4433912_HAL_KEYPAD_COL1PINPORT BRD_D0_GPIO_PORT
#define S4433912_HAL_KEYPAD_COL2PINPORT BRD_D0_GPIO_PORT
#define S4433912_HAL_KEYPAD_COL3PINPORT BRD_D0_GPIO_PORT
#define S4433912_HAL_KEYPAD_COL4PINPORT BRD_D0_GPIO_PORT

#define S4433912_HAL_KEYPAD_COL1PINCLK __BRD_D0_GPIO_CLK()
#define S4433912_HAL_KEYPAD_COL2PINCLK __BRD_D0_GPIO_CLK()
#define S4433912_HAL_KEYPAD_COL3PINCLK __BRD_D0_GPIO_CLK()
#define S4433912_HAL_KEYPAD_COL4PINCLK __BRD_D0_GPIO_CLK()

//Row parameters

#define S4433912_HAL_KEYPAD_ROW1PIN D0
#define S4433912_HAL_KEYPAD_ROW2PIN D0
#define S4433912_HAL_KEYPAD_ROW3PIN D0
#define S4433912_HAL_KEYPAD_ROW4PIN D0

#define S4433912_HAL_KEYPAD_ROW1PINPORT BRD_D0_GPIO_PORT
#define S4433912_HAL_KEYPAD_ROW2PINPORT BRD_D0_GPIO_PORT
#define S4433912_HAL_KEYPAD_ROW3PINPORT BRD_D0_GPIO_PORT
#define S4433912_HAL_KEYPAD_ROW4PINPORT BRD_D0_GPIO_PORT

#define S4433912_HAL_KEYPAD_ROW1PINCLK __BRD_D0_GPIO_CLK()
#define S4433912_HAL_KEYPAD_ROW2PINCLK __BRD_D0_GPIO_CLK()
#define S4433912_HAL_KEYPAD_ROW3PINCLK __BRD_D0_GPIO_CLK()
#define S4433912_HAL_KEYPAD_ROW4PINCLK __BRD_D0_GPIO_CLK()


#endif

void s4433912_hal_keypad_init();
void s4433912_hal_keypad_deinit();
void s4433912_hal_keypad_fsmprocessing();
uint8_t s4433912_hal_keypad_read_status();
unsigned char s4433912_hal_keypad_read_key(); 
char s4433912_hal_keypad_read_ascii();

#endif