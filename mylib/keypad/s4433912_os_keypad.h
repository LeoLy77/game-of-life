/**
 ***************************************************************
 * @file mylib/keypad/s4433912_os_keypad.h
 * @author Leo Ly - 44339124
 * @date 05/05/2020
 * @brief keypad os driver
 * REFERENCE: csse3010_mylib_os_keypad.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 * s4433912_os_KeypadTaskDeInit(void) - Deinitiialise all running tasks, queue and eventgroup
 * s4433912_os_TaskKeypadInit(void) - Initialise hal_keypad and eventgroup
 * s4433912_os_TaskKeypadBitReading(void) - Initialise s4433912_QueueKeypadRead, read from eventgroup and send to the queue
 ***************************************************************
*/

#ifndef S4433912_OS_KEYPAD_H
#define S4433912_OS_KEYPAD_H

#include "board.h"
#include "processor_hal.h"
#include "debug_printf.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "task.h"
#include "queue.h"

#include "s4433912_hal_keypad.h"

/* Definitions for the event bits in the event group. */
#define KEYPAD_KEY_1 (1UL << 0x01UL) 
#define KEYPAD_KEY_2 (1UL << 0x02UL)
#define KEYPAD_KEY_3 (1UL << 0x03UL)
#define KEYPAD_KEY_A (1UL << 0x0AUL) 

#define KEYPAD_KEY_4 (1UL << 0x04UL) 
#define KEYPAD_KEY_5 (1UL << 0x05UL)
#define KEYPAD_KEY_6 (1UL << 0x06UL)
#define KEYPAD_KEY_B (1UL << 0x0BUL)

#define KEYPAD_KEY_7 (1UL << 0x07UL)
#define KEYPAD_KEY_8 (1UL << 0x08UL) 
#define KEYPAD_KEY_9 (1UL << 0x09UL)
#define KEYPAD_KEY_C (1UL << 0x0CUL)

#define KEYPAD_KEY_0 (1UL << 0x00UL)
#define KEYPAD_KEY_F (1UL << 0x0FUL) 
#define KEYPAD_KEY_E (1UL << 0x0EUL)
#define KEYPAD_KEY_D (1UL << 0x0DUL)

QueueHandle_t s4433912_QueueKeypadRead;
TaskHandle_t s4433912_TaskKeypadInit;

extern void s4433912_os_KeypadTaskDeInit(void);
extern void s4433912_os_TaskKeypadInit(void);

#endif