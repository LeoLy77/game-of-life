/**
 ***************************************************************
 * @file mylib/joystick/s4433912_os_joystick.h
 * @author Leo Ly - 44339124
 * @date 25/04/2020
 * @brief Joystick OS driver
 * REFERENCE: csse3010_mylib_os_joystick.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 * s4433912_os_TaskJoystickDeinit(void) - Deinitialise os functions
 * s4433912_os_TaskJoystickInit(void) - Initialises os and hal functions
 ***************************************************************
*/

#ifndef S4433912_OS_JOYSTICK_H
#define S4433912_OS_JOYSTICK_H

#include "s4433912_hal_joystick.h"
#include "board.h"
#include "debug_printf.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

#define OS_JOYSTICK_BUTTON_PIN JOYSTICK_BUTTON_PIN

SemaphoreHandle_t s4433912_SemaphoreJoystickReadZ;
TaskHandle_t s4433912_TaskJoystick;
QueueHandle_t s4433912_QueueJoystickX;
QueueHandle_t s4433912_QueueJoystickY;

extern void s4433912_os_JoystickDeinit(void);
extern void s4433912_os_TaskJoystickInit(void);
extern void s4433912_os_JoystickZ_EXTI_handler(void);
#endif