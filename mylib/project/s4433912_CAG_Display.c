/**
 ***************************************************************
 * @file mylib/project/CAG_Display.c
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
/* Includes ------------------------------------------------------------------*/
#include "s4433912_CAG_Display.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SET_CELL_COLOUR(fg, bg) debug_printf("\e[7;%d;%dm",fg,bg)
//Cursor control functions
#define SCREEN_CLEAR()      debug_printf("\e[2J")
#define SCREEN_HOME()       debug_printf("\e[H")
#define SCREEN_ORIGIN()     debug_printf("\e[f")
#define SCREEN_UP(value)    debug_printf("\e[%dA", value)
#define SCREEN_DOWN(value)  debug_printf("\e[%dB", value)
#define SCREEN_RIGHT(value) debug_printf("\e[%dC", value)
#define SCREEN_LEFT(value)  debug_printf("\e[%dD", value)
#define CURSOR_HIDE()       debug_printf("\e[?25l")
#define CURSOR_SHOW()       debug_printf("\e[?25h")
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
int fgColours[] = {FG_BLACK, FG_RED, FG_GREEN, FG_YELLOW, FG_BLUE, FG_MAGENTA, FG_CYAN, FG_WHITE};
int cellColour = FG_BLUE; //default colour
uint8_t colourChange = pdTRUE; //boolean colour change status
uint8_t grid[GRID_WIDTH][GRID_HEIGHT]; //Grid contains cells
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  draw an empty grid
  * @param  None
  * @retval None
  */
void draw_grid(void) {

    portENTER_CRITICAL();
    //Hide cursor and move it to top left corner of the screen
    CURSOR_HIDE();
    SCREEN_CLEAR();
    SCREEN_HOME();
    SET_CELL_COLOUR(FG_BLACK, BG_WHITE);
    for (int column = 0; column < GRID_HEIGHT; column++) {
        for (int size = 0; size < CELL_HEIGHT; size++) {
            for (int row = 0; row < GRID_WIDTH + 1; row++) {   
                SET_CELL_COLOUR(FG_BLACK, BG_WHITE); //Black grid
                if (row == GRID_WIDTH) {

                    SET_CELL_COLOUR(FG_BLACK, BG_WHITE);
                    //Axis label
                    #if GRID_LABEL
                    if (size == CELL_HEIGHT - 1) {

                        SCREEN_UP(1);
                        debug_printf("%02d", column);
                        SCREEN_DOWN(1);
                    }
                    #endif
                } else {  

                    SET_CELL_COLOUR(FG_BLACK, BG_WHITE);
                    for (int k = 0; k < CELL_WIDTH; k++) {
                        debug_printf(" ");
                    }        
                }
            }
            debug_printf("\n\r");
        }
    }
    //Axis label
    #if GRID_LABEL
    SET_CELL_COLOUR(FG_BLACK, BG_WHITE);
    for (int i = 0; i < GRID_HEIGHT; i++) {
        debug_printf("%02d   ", i);
    }
    #endif
    portEXIT_CRITICAL();
}

/**
  * @brief colour a cell at given position coordinate
  * @param x - x position of cell
  * @param y - y position of cell
  * @param live - current live status of cell to determine its foreground colour
  * @retval None
  */
void draw_cell(int x, int y, int live) {
    //Move cursor to top left corner
    SCREEN_HOME();
    //Get cursor in y position
    for (int i = 0 ; i < y; i++) {
        SCREEN_DOWN(CELL_HEIGHT);
    }    
    //Get cursor in x position
    for (int i = 0; i < x; i++) {
        SCREEN_RIGHT(CELL_WIDTH);
    }
    //Modify cell colour
    live == ALIVE ? SET_CELL_COLOUR(cellColour, BG_WHITE) : SET_CELL_COLOUR(FG_BLACK, BG_WHITE); 
    //Draw cell on top of existing print 
    portENTER_CRITICAL();
    for (int row = 0; row < CELL_HEIGHT; row++) {
        for (int k = 0; k < CELL_WIDTH; k++) {
            debug_printf(" ");
        }
        SCREEN_LEFT(CELL_WIDTH);
        SCREEN_DOWN(CELL_HEIGHT - 1);
    }
    portEXIT_CRITICAL();
    //Set it back to black
    SET_CELL_COLOUR(FG_BLACK, BG_WHITE);
}

/**
  * @brief  Task that receives and executes colour change commands
  * @param  None
  * @retval None
  */
void s4433912_os_TaskCAGDspColourCtrl(void) {

    TickType_t xTicksToWait = 5;
    uint8_t colourSel = 4; //default BLUE
    uint8_t prevColourSel = 4;
    for (;;) {
        if (xQueueReceive(s4433912_QueueDspColourRead, (void*) &colourSel, xTicksToWait) == pdPASS) {
            //Update cell colour to joystick X
            cellColour = fgColours[colourSel];
            if (colourSel == prevColourSel) {

                colourChange = pdFALSE;
            } else {

                colourChange = pdTRUE;
                prevColourSel = colourSel;
            }
        } 
    }
}

/**
  * @brief  Task that receives updated grid from simulation
  * @param  None
  * @retval None
  */
void s4433912_os_TaskCAGDspGridRecv(void) {

    TickType_t xTicksToWait = 5;
    for (;;) {
        //Get updated grid from Sim
        if (s4433912_QueueDspGridRead != NULL) {

            if (xQueueReceive(s4433912_QueueDspGridRead, (void*) &grid, xTicksToWait) == pdPASS) {
                //Only need this if statement to catch the Queue
            } 
        }
        vTaskDelay(xTicksToWait);
    } 
}

/**
  * @brief  Task that updates and redraws the grid
  * @param  None
  * @retval None
  */
void s4433912_os_TaskCAGDspDrawCells(void) {

    TickType_t xTicksToWait = pdMS_TO_TICKS(100);
    //Preset clear grid with no alive cells
    for (int i = 0; i < GRID_WIDTH; i++) {
        memset(grid[i], DEAD, sizeof(grid[i]));
    }
    //Set up the grid
    draw_grid();
    for (;;) {
        //Redraw cells with change bit on, or colour change
        for (int x = 0; x < GRID_WIDTH; x++) {
            for (int y = 0; y < GRID_HEIGHT; y++) {
                if (grid[x][y] & (1 << CHANGE) || 

                    (colourChange == pdTRUE && (grid[x][y] & (1 << LIVE)) == ALIVE)) {
                    draw_cell(x, y, (grid[x][y] & (1 << LIVE)));
                }
            }
        }
        vTaskDelay(xTicksToWait);
    }
}

/**
  * @brief  Creates display tasks
  * @param  None
  * @retval None
  */
extern void s4433912_os_CAGDspTasks_init(void) {

    xTaskCreate( (void*) &s4433912_os_TaskCAGDspColourCtrl, (const signed char *) "DSP_COLR", mainSINGLE_STACK_SIZE*2, NULL, mainLOWTASK_PRIORITY, &s4433912_TaskCAGDspColourCtrl);
    xTaskCreate( (void*) &s4433912_os_TaskCAGDspGridRecv, (const signed char *) "DSP_COLR", mainSINGLE_STACK_SIZE*2, NULL, mainLOWTASK_PRIORITY, &s4433912_TaskCAGDspGridRecv);
    xTaskCreate( (void*) &s4433912_os_TaskCAGDspDrawCells, (const signed char *) "DSP_GRID", mainTRIPLE_STACK_SIZE*2, NULL, mainMEDIUMTASK_PRIORITY, &s4433912_TaskCAGDspDrawCells);
}

