/**
 ***************************************************************
 * @file mylib/project/s4433912_CAG_share.h
 * @author Leo Ly - 44339124
 * @date 06/05/2020
 * @brief share resources among all other CAG files
 * REFERENCE: None
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 * s4433912_CAGPushbutton_callback(void) - on-board pushbutton interrupt semaphore callback
 * s4433912_CAGPushbutton_init(void) - initialises on-board leds and pushbuttons
 * s4433912_FreeRTOS_init(void) - initialises freertos elements to be used by various tasks
 * s4433912_grid_printf(uint8_t height, const char *fmt, ...) - safe debug print the CAG display
 * s4433912_os_CAG_TaskPbRecv(void) - Task that switches keypad leds
 * s4433912_os_CAG_TaskLedSend(void) - Tasks that interprets keypad_grid subgrid pos and adjusts the leds
 ***************************************************************
*/

#ifndef S4433912_CAG_SHARE_H
#define S4433912_CAG_SHARE_H

#include "s4433912_os_lta1000g.h"

#include "board.h"
#include "processor_hal.h"
#include "debug_printf.h"

#include <stdint.h>
#include <stdarg.h>

#include "FreeRTOS.h"
#include "event_groups.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
/* Task Priorities ------------------------------------------------------------*/
#define mainVERYLOWTASK_PRIORITY					( tskIDLE_PRIORITY + 0 )
#define mainLOWTASK_PRIORITY					    ( tskIDLE_PRIORITY + 1 )
#define mainMEDIUMTASK_PRIORITY					    ( tskIDLE_PRIORITY + 2 )
#define mainHIGHTASK_PRIORITY					    ( tskIDLE_PRIORITY + 3 )
#define mainVERYHIGHTASK_PRIORITY					( tskIDLE_PRIORITY + 4 )

/* Task Stack Allocations -----------------------------------------------------*/
#define mainSINGLE_STACK_SIZE				( configMINIMAL_STACK_SIZE * 1 )
#define mainDOUBLE_STACK_SIZE				( configMINIMAL_STACK_SIZE * 2 )
#define mainTRIPLE_STACK_SIZE				( configMINIMAL_STACK_SIZE * 3 )
//Control macros
#define DEBUG 1 //control grid_printf
#define TASK_SYNC 0 //control task sync in sim
#define GRID_LABEL 0 //control axis labels in display

#define GRID_WIDTH 15
#define GRID_HEIGHT 15
#define SUBGRID_SIZE 3
//SIM functions
#define CLEAR_GRID 0
#define START_SIM 1
#define STOP_SIM 2
#define SIM_UPDATE_0_5 3
#define SIM_UPDATE_1 4
#define SIM_UPDATE_2 5
#define SIM_UPDATE_5 6
#define SIM_UPDATE_10 7
#define DLT_SIM 8
#define DLT_JSK 9
#define CRE_SIM 10
#define CRE_JSK 11
//Bit manipulation of cell collection
#define LIVE 0
#define CHANGE 1
//Cell status
#define DEAD 0
#define ALIVE 1
//Type value
#define STILL_LIFE 2
#define CELL 1
#define OSCILLATOR 3
#define SPACE_SHIP 4

#define UPPER_NIBBLE 0xF0
#define LOWER_NIBBLE 0x0F
#define UPPER_NIBBLE_SHIFT 4

#define BLOCK 0
#define BEEHIVE 1 
#define LOAF 2

#define BLINKER 0
#define TOAD 1
#define BEACON 2

#define GLIDER 0
//KEYPAD modes
#define KP_GRD 0
#define KP_MNC 1

//Sim modes
#define SIM_OFF 0
#define SIM_ON 1

#define COLOUR_RANGE 8
#define RFS_RANGE 5
/* Type - Cell , or Lifeform
x and y positions for cells and lifeforms are the cell x
and y coordinates (in the entire grid)
Alive - cell status (1 alive, 0 dead)
Cell x position or Lifeform x position
Cell y position or Lifeform y position*/
typedef struct caMessage { 
    uint8_t type ; 
    int cell_x ; 
    int cell_y ; 
} caMessage_t ;

typedef struct position {
    uint8_t x;
    uint8_t y;
} position_t;

uint8_t s4433912_keypad_mode;
EventGroupHandle_t s4433912_EventGroupSimKPFncs;
QueueHandle_t s4433912_QueueDspColourRead;
QueueHandle_t s4433912_QueueDspGridRead;
QueueHandle_t s4433912_QueueCAGSimMsg;
QueueHandle_t s4433912_QueueLedMsgSend;
SemaphoreHandle_t s4433912_SemaphoreCAGKeypadMode;
SemaphoreHandle_t s4433912_SemaphoreCAGPushButton;
TaskHandle_t s4433912_TaskCAGPbRecv;
TaskHandle_t s4433912_TaskLedSend;
TaskHandle_t s4433912_TaskJoystickTasksInit;
TaskHandle_t s4433912_TaskJoystickTasksDeinit;
TimerHandle_t s4433912_TimerJoystickInit, s4433912_TimerJoystickDeinit;

extern void s4433912_os_CAG_TaskLedSend(void);
extern void s4433912_CAGPushbutton_callback(void);
extern void s4433912_CAGPushbutton_init(void);
extern void s4433912_os_CAG_TaskPbRecv(void);
extern void s4433912_FreeRTOS_init(void);
extern void s4433912_grid_printf(uint8_t height, const char *fmt, ...);

#endif