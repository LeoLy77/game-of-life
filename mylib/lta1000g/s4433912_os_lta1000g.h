/**
 ***************************************************************
 * @file mylib/lta1000g/s4433912_os_lta1000g.h
 * @author Leo Ly - 44339124
 * @date 23/04/2020
 * @brief LED Light Bar OS driver
 * REFERENCE: csse3010_mylib_os_lta1000g.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 * s4433912_os_TaskJoystickDeinit(void) - Deinitialise os functions
 * s4433912_os_TaskJoystickInit(void) - Initialises os and hal functions
 ***************************************************************
*/

#ifndef S4433912_OS_LTA1000G_H
#define S4433912_OS_LTA1000G_H

#include "s4433912_hal_lta1000g.h"
#include "board.h"
#include "debug_printf.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define LTA1000G_QUEUE_LENGTH 3

QueueHandle_t s4433912_QueueLta1000gReadMsg;
TaskHandle_t s4433912_TaskLta100g;

extern void s4433912_os_TaskLta1000gInit(void);
extern void s4433912_os_Lta1000gDeInit(void);

#endif
