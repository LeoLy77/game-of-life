/**
 ***************************************************************
 * @file mylib/project/CAG_keypad_grid.h
 * @author Leo Ly - 44339124
 * @date 06/05/2020
 * @brief Cellular Automation Graphics (CAG) keypad control os driver -  Control the CAGDisplay grid via the keypad.
 * REFERENCE: CSSE3010 Project.pdf - Design Task 3: CAGKeypad - Grid Mode
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 * s4433912_os_KPgrid_TaskGridSend(void) - Task that compiles caMessage to send to simulation and updates subgrid position leds
 ***************************************************************
*/

#ifndef S4433912_OS_CAG_KEYPAD_GRID_H
#define S4433912_OS_CAG_KEYPAD_GRID_H

#include "s4433912_os_keypad.h"
#include "s4433912_os_lta1000g.h"

#include "s4433912_CAG_share.h"

#include "board.h"
#include "processor_hal.h"
#include "debug_printf.h"

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "queue.h"
#include "semphr.h"

TaskHandle_t s4433912_TaskCAGGridSend;
extern void s4433912_os_KPgrid_TaskGridSend(void);
#endif