/**
 ***************************************************************
 * @file mylib/project/CAG_keypad_mnemonic.h
 * @author Leo Ly - 44339124
 * @date 07/05/2020
 * @brief Cellular Automation Graphics (CAG) keypad control os driver -  Control the CAGDisplay grid 
 * using the commands entered via the keypad
 * REFERENCE: CSSE3010 Project.pdf - Design Task 4: CAGKeypad - Mnemonic Mode
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 * s4433912_os_CAGMnc_TaskSendToSim - Task that receives key presses from the keypad,
 *         interprets the key presses in alphanumeric telephone behaviour,
 *         indentifies matching simulation commands and sends it to the simulation
 ***************************************************************
*/

#ifndef S4433912_OS_CAG_KEYPAD_MNEMONIC_H
#define S4433912_OS_CAG_KEYPAD_MNEMONIC_H
#include "s4433912_os_keypad.h"
#include "s4433912_os_lta1000g.h"
#include "s4433912_CAG_share.h"

#include <string.h>
#include <stdio.h>

#include "board.h"
#include "processor_hal.h"
#include "debug_printf.h"

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "queue.h"
#include "semphr.h"
TaskHandle_t s4433912_TaskCAGMncSend;
extern void s4433912_os_CAGMnc_TaskSendToSim(void);

#endif