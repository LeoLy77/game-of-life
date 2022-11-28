/**
 ***************************************************************
 * @file mylib/project/CAG_Simulator.h
 * @author Leo Ly - 44339124
 * @date 06/05/2020
 * @brief Cellular Automation Graphics (CAG) simulator os driver - Control the cellular automation.
 * REFERENCE: CSSE3010 Project.pdf - Design Task 1: CAGSimulator
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 * s4433912_os_SimTasks_init(void) - initialises all simulation tasks
 ***************************************************************
*/

#ifndef S4433912_OS_CAG_SIMULATOR_H
#define S4433912_OS_CAG_SIMULATOR_H

#include <string.h>
#include "s4433912_CAG_share.h"
#include "board.h"
#include "processor_hal.h"
#include "debug_printf.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "s4433912_os_joystick.h"

TaskHandle_t s4433912_TaskCAGSimRecvGrid;
TaskHandle_t s4433912_TaskSimUpdate;
TaskHandle_t s4433912_TaskSimDpSend;
TaskHandle_t s4433912_TaskSimLeds;
TaskHandle_t s4433912_TaskSimKpRecv;

extern void s4433912_os_SimTasks_init(void);
#endif