/**
 ***************************************************************
 * @file mylib/project/CAG_Display.h
 * @author Leo Ly - 44339124
 * @date 09/05/2020
 * @brief Cellular Automation Graphics (CAG) CAGDisplay: Display the output of the CAG
 * using the commands entered via the keypad
 * REFERENCE: CSSE3010 Project.pdf - Design Task 2: CAGDisplay
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 *s4433912_os_CAGDspTasks_init(void) - Creates display tasks
 ***************************************************************
*/
#ifndef S4433912_CAG_DISPLAY
#define S4433912_CAG_DISPLAY

#include <string.h>
#include "board.h"
#include "debug_printf.h"
#include "processor_hal.h"

#include "s4433912_CAG_share.h"

#include "FreeRTOS.h"
#include "event_groups.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
//Foreground colours
#define FG_BLACK 	30
#define FG_RED		31
#define FG_GREEN	32
#define FG_YELLOW	33
#define FG_BLUE		34
#define FG_MAGENTA	35
#define FG_CYAN		36
#define FG_WHITE	37
//Background colours
#define BG_BLACK	40
#define BG_RED		41
#define BG_GREEN	42
#define BG_YELLOW	43
#define BG_BLUE		44
#define BG_MAGENTA	45
#define BG_CYAN		46
#define BG_WHITE	47
//Cell colours
#define CELL_BLACK "\e[7;30;40m"
#define CELL_RED	"\e[7;31;41m"
#define CELL_GREEN	"\e[7;32;42m"
#define CELL_YELLOW		"\e[7;33;43m"
#define CELL_BLUE		"\e[7;34;44m"
#define CELL_MAGENTA	"\e[7;35;45m"
#define CELL_CYAN		"\e[7;36;46m"
#define CELL_WHITE 		"\e[7;37;47m"
//Cell sizes
#define CELL_HEIGHT 2
#define CELL_WIDTH 5
#define COLOUR_RANGE 8
TaskHandle_t s4433912_TaskCAGDspColourCtrl;
TaskHandle_t s4433912_TaskCAGDspGridRecv;
TaskHandle_t s4433912_TaskCAGDspDrawCells;

extern void s4433912_os_CAGDspTasks_init(void);
#endif
