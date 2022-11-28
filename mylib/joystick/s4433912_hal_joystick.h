/**
 ***************************************************************
 * @file mylib/joystick/s4433912_hal_joystick.h
 * @author Leo Ly - 44339124
 * @date 12/03/2020
 * @brief Joystick driver
 * REFERENCE: csse3010_mylib_hal_joystick.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 * s4433912_hal_joystick_init() - Initialise GPIO pins and ADC
 * int joystick_readxy(ADC Handler) - Internal generic function to read X or Y joystick value.
 ***************************************************************

The  Joystick  HAL  Driver  processes  the  Joystick  X,  Y  and  Z  inputs. 
Joystick  X  and  Y  areanalog voltage inputs and Z is a digital on/off input.
*/
#include "board.h"
#include "processor_hal.h"
#include "s4433912_hal_iss.h"

#include "debug_printf.h"



#ifndef S4433912_HAL_JOYSTICK_H
#define S4433912_HAL_JOYSTICK_H

#define JOYSTICK_ISS_SOURCE S4433912_HAL_ISS_SOURCE_3
#define JOYSTICK_BUTTON_PIN BRD_A3_PIN

ADC_HandleTypeDef AdcHandleX;
ADC_HandleTypeDef AdcHandleY;

uint32_t joystick_readxy(ADC_HandleTypeDef);

#define S4433912_HAL_JOYSTICK_X_READ() joystick_readxy(AdcHandleX) 
#define S4433912_HAL_JOYSTICK_Y_READ() joystick_readxy(AdcHandleY) 
#define S4433912_HAL_JOYSTICK_Z_READ() s4433912_hal_iss_synchroniser(JOYSTICK_ISS_SOURCE)

void s4433912_hal_joystick_init(void);

#endif
