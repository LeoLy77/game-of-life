/**
 ***************************************************************
 * @file mylib/project/CAG_joystick.h
 * @author Leo Ly - 44339124
 * @date 27/05/2020
 * @brief Cellular Automation Graphics (CAG) joystick os driver.
 * REFERENCE: CSSE3010 Project.pdf - Design Task 5: CAGJoystick
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 * s4433912_os_CAG_Joystick_Z_callback(void) - extended joystick Z interrupt callback
 * s4433912_os_JoystickTasksInit(void) - Initialise all joystick related tasks
 ***************************************************************
*/

#ifndef S4433912_OS_CAG_JOYSTICK_H
#define S4433912_OS_CAG_JOYSTICK_H

#include "board.h"
#include "processor_hal.h"
#include "debug_printf.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"
#include "semphr.h"
#include "timers.h"

#include "s4433912_CAG_share.h"
#include "s4433912_os_joystick.h"

#define JOYSTICK_Z_PIN OS_JOYSTICK_BUTTON_PIN

TaskHandle_t s4433912_TaskJoystickReadZ;
TaskHandle_t s4433912_TaskJoystickReadX;
TaskHandle_t s4433912_TaskJoystickReadY;

extern void s4433912_os_CAG_Joystick_Z_callback(void);
extern void s4433912_os_JoystickTasksInit(void);
#endif
