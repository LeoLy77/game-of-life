/**
 ***************************************************************
 * @file mylib/project/CAG_Simulator.c
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

/* Includes ------------------------------------------------------------------*/
#include "s4433912_CAG_Simulator.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define NEIGHBOUR_NUM 8
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define SIM_TASK_BIT (1 << 0)
#define LED_TASK_BIT (1 << 1)
//Calibration periods, manual adjustments
#define TICK_CAL_0_5 35
#define TICK_CAL_1 65
#define TICK_CAL_2 125
#define TICK_CAL_5 290
#define TICK_CAL_10 550
/* Private variables ---------------------------------------------------------*/
EventGroupHandle_t EventGroupSync; //to sync internal tasks
uint8_t cellCollection[GRID_WIDTH][GRID_HEIGHT]; //keep track of cell lives
uint16_t updatePeriod = 0; //simulation update time interval (ms)
uint16_t updatePeriodToSet = 0; //simulation update time interval (ms) received from EventGroup
uint8_t update = SIM_OFF; //simulation update status
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void s4433912_os_SimTasks_deinit(void);
extern void s4433912_os_SimTasks_init(void);
extern void s4433912_os_TaskSimUpdate(void);
extern void s4433912_os_TaskSimDpSend(void);
void s4433912_os_TaskSimLeds(void);
void s4433912_os_TaskSimKpRecv(void);
void s4433912_os_CAGSim_TaskCmdRecv(void);

/**
  * @brief  Check if the given position is in the grid
  * @param  x - x position
  * @param  y - y position
  * @retval None
  */
uint8_t valid_position(int x, int y) {

    return (x >= 0 && x < GRID_WIDTH &&
            y >= 0 && y < GRID_HEIGHT);
}

/**
  * @brief  run a cell through the Game of Life algorithm
  * @param  x - x position of the cell
  * @param  y - y position of the cell
  * @param  collection - a copy of cellCollection
  * @retval (LIVE | CHANGE) bits of the cell in cellCollection, 
  *         these bits determine if the cell life changes through the test
  */
uint8_t cell_survival_test(uint8_t x, uint8_t y, uint8_t collection[GRID_WIDTH][GRID_HEIGHT]) {

    uint8_t numLiveNeighbours = 0;
    position_t neighbour;
    uint8_t live = DEAD, change = 0;
    neighbour.x = 0;
    neighbour.y = 0;
    //Count the number of alive neighbours
    for (int i = 0; i < NEIGHBOUR_NUM; i++) {
        //Corresponding neighbour coordinates
        switch (i) {

            case 0:

                neighbour.x = x - 1;
                neighbour.y = y - 1;
                break;

            case 1:

                neighbour.x = x - 1;
                neighbour.y = y;
                break;

            case 2:

                neighbour.x = x - 1;
                neighbour.y = y + 1;
                break;

            case 3:

                neighbour.x = x;
                neighbour.y = y + 1;
                break;

            case 4:

                neighbour.x = x + 1;
                neighbour.y = y + 1;
                break;

            case 5:

                neighbour.x = x + 1;
                neighbour.y = y;
                break;

            case 6:

                neighbour.x = x + 1;
                neighbour.y = y - 1;
                break;

            case 7:

                neighbour.x = x;
                neighbour.y = y - 1;
                break;

            default:
                break;

        }
        
        if (valid_position(neighbour.x, neighbour.y) &&
            ((collection[neighbour.x][neighbour.y] & (1 << LIVE)) == ALIVE)) {

            numLiveNeighbours++;
        }
    }
    if (numLiveNeighbours == 3) {

        live = ALIVE; //Ideal population
    } else if (numLiveNeighbours > 3) { 

        live = DEAD; //Overpopulation
    }
    //Special case for alive cell
    if ((collection[x][y] & (1 << LIVE)) == ALIVE) {

        if (numLiveNeighbours < 2 || numLiveNeighbours > 3) {

            live = DEAD; //Live cell with less than 2 neighbours
        } else {

            live = ALIVE;
        }
    }
    //Life changes
    if ((collection[x][y] & (1 << LIVE)) != live) {

        change = 1;
    }

    return ((live << LIVE) | (change << CHANGE));
}

/**
  * @brief  goes through cellCollection and tests every cell of it will survive in the next generation
  * @param  None
  * @retval None
*/
void update_generation(void) {

    uint8_t cellCollectionCp[GRID_WIDTH][GRID_HEIGHT]; 
    //Make a copy of the collection because the real collection is modified on the fly
    for (int i = 0; i < GRID_WIDTH; i++) {
        memcpy(cellCollectionCp[i], cellCollection[i], sizeof(cellCollection[i]));
    }

    for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            cellCollection[x][y] = cell_survival_test(x, y, cellCollectionCp);
        }
    }
}

/**
  * @brief  creates a stilllife and places it in cellCollection based on received caMessage
  * @param  msg - caMessage from keypad
  * @retval None
  */
void create_stilllife(caMessage_t* msg) {

    uint8_t maxNumCells = 7;//Loaf is the largest still life type with 7 blocks
    uint8_t numCells = 0;
    position_t cells[maxNumCells]; 
    //Set everything 0
    for (int i = 0; i < maxNumCells; i++) {
        memset(cells, 0, sizeof(position_t));
    }
    //Upper left most is the anchor point
    if ((msg->type & LOWER_NIBBLE) == BLOCK) {

        numCells = 4;
        cells[0].x = msg->cell_x; //    O1
        cells[0].y = msg->cell_y; //    23
        cells[1].x = msg->cell_x + 1; //
        cells[1].y = msg->cell_y; //
        cells[2].x = msg->cell_x;
        cells[2].y = msg->cell_y + 1;
        cells[3].x = msg->cell_x + 1;
        cells[3].y = msg->cell_y + 1;
    } else if ((msg->type & LOWER_NIBBLE) == BEEHIVE) {

        numCells = 6;
        cells[0].x = msg->cell_x;     //    01
        cells[0].y = msg->cell_y;     //   2  3 
        cells[1].x = msg->cell_x + 1; //    45
        cells[1].y = msg->cell_y;     //
        cells[2].x = msg->cell_x - 1;
        cells[2].y = msg->cell_y + 1;
        cells[3].x = msg->cell_x + 2;
        cells[3].y = msg->cell_y + 1;
        cells[4].x = msg->cell_x;
        cells[4].y = msg->cell_y + 2;
        cells[5].x = msg->cell_x + 1;
        cells[5].y = msg->cell_y + 2;
    } else if ((msg->type & LOWER_NIBBLE) == LOAF) {

        numCells = 7;
        cells[0].x = msg->cell_x;      //  01
        cells[0].y = msg->cell_y;      // 2  3 
        cells[1].x = msg->cell_x + 1;  //  4 5
        cells[1].y = msg->cell_y;      //   6
        cells[2].x = msg->cell_x - 1;
        cells[2].y = msg->cell_y + 1;
        cells[3].x = msg->cell_x + 2;
        cells[3].y = msg->cell_y + 1;
        cells[4].x = msg->cell_x;
        cells[4].y = msg->cell_y + 2;
        cells[5].x = msg->cell_x + 2;
        cells[5].y = msg->cell_y + 2;
        cells[6].x = msg->cell_x + 1;
        cells[6].y = msg->cell_y + 3;
    }
    //Create lifeform
    for (int i = 0; i < numCells; i++) {
        if (valid_position(cells[i].x, cells[i].y)) {

            cellCollection[cells[i].x][cells[i].y] |= (1 << CHANGE);
            cellCollection[cells[i].x][cells[i].y] |= (ALIVE << LIVE);
        }
    }
}

/**
  * @brief  creates an oscillator and places it in cellCollection based on received caMessage
  * @param  msg - caMessage from keypad
  * @retval None
  */
void create_oscillator(caMessage_t* msg) {

    uint8_t maxNumCells = 6;//Beacon is the largest still life type with 7 blocks
    uint8_t numCells = 0;
    position_t cells[maxNumCells]; 
    //Set everything 0
    for (int i = 0; i < maxNumCells; i++) {
        memset(cells, 0, sizeof(position_t));
    }
    //Upper left most is the anchor point
    if ((msg->type & LOWER_NIBBLE) == BLINKER) {

        numCells = 3;
        cells[0].x = msg->cell_x;     //    O
        cells[0].y = msg->cell_y;     //    1
        cells[1].x = msg->cell_x;     //    2 
        cells[1].y = msg->cell_y + 1; //
        cells[2].x = msg->cell_x;
        cells[2].y = msg->cell_y + 2;
    } else if ((msg->type & LOWER_NIBBLE) == TOAD) {

        numCells = 6;
        cells[0].x = msg->cell_x;     //     0
        cells[0].y = msg->cell_y;     //   1  2 
        cells[1].x = msg->cell_x - 2; //   3  4 
        cells[1].y = msg->cell_y + 1; //    5
        cells[2].x = msg->cell_x + 1;
        cells[2].y = msg->cell_y + 1;
        cells[3].x = msg->cell_x - 2;
        cells[3].y = msg->cell_y + 2;
        cells[4].x = msg->cell_x + 1;
        cells[4].y = msg->cell_y + 2;
        cells[5].x = msg->cell_x - 1;
        cells[5].y = msg->cell_y + 3;
    } else if ((msg->type & LOWER_NIBBLE) == BEACON) {

        numCells = 6;
        cells[0].x = msg->cell_x;      // 01
        cells[0].y = msg->cell_y;      // 2  3
        cells[1].x = msg->cell_x + 1;  //   45
        cells[1].y = msg->cell_y;      //
        cells[2].x = msg->cell_x;
        cells[2].y = msg->cell_y + 1;
        cells[3].x = msg->cell_x + 3;
        cells[3].y = msg->cell_y + 2;
        cells[4].x = msg->cell_x + 2;
        cells[4].y = msg->cell_y + 3;
        cells[5].x = msg->cell_x + 3;
        cells[5].y = msg->cell_y + 3;
    }
    //Create lifeform
    for (int i = 0; i < numCells; i++) {
        if (valid_position(cells[i].x, cells[i].y)) {

            cellCollection[cells[i].x][cells[i].y] = ((ALIVE << LIVE) | (1 << CHANGE));
        }
    }
}

/**
  * @brief  creates a spaceship and places it in cellCollection based on received caMessage
  * @param  msg - caMessage from keypad
  * @retval None
  */
void create_spaceship(caMessage_t* msg) {
    uint8_t numCells = 5;
    position_t cells[numCells]; 
    //Set everything 0
    for (int i = 0; i < numCells; i++) {
        memset(cells, 0, sizeof(position_t));
    }
    //Upper left most is the anchor point
    if ((msg->type & LOWER_NIBBLE) == GLIDER) {
        cells[0].x = msg->cell_x;      //    0
        cells[0].y = msg->cell_y;      //     1
        cells[1].x = msg->cell_x + 1;  //   234
        cells[1].y = msg->cell_y + 1;  //
        cells[2].x = msg->cell_x - 1;
        cells[2].y = msg->cell_y + 2;
        cells[3].x = msg->cell_x + 0;
        cells[3].y = msg->cell_y + 2;
        cells[4].x = msg->cell_x + 1;
        cells[4].y = msg->cell_y + 2;
    }

    //Create lifeform
    for (int i = 0; i < numCells; i++) {
        if (valid_position(cells[i].x, cells[i].y)) {
            cellCollection[cells[i].x][cells[i].y] = ((ALIVE << LIVE) | (1 << CHANGE));
        }
    }
}

/**
  * @brief  reads caMessage from keypad and determines an action for the msg
  * @param  msg - caMessage from keypad
  * @retval None
  */
void caMsg_handler(caMessage_t* msg) {

    if ((msg->type & UPPER_NIBBLE) == (CELL << UPPER_NIBBLE_SHIFT)) {
        //toggle cell live status bit and also set change bit to inform display to redraw this cell
        cellCollection[msg->cell_x][msg->cell_y] ^= (1 << LIVE); 
        cellCollection[msg->cell_x][msg->cell_y] |= (1 << CHANGE); 
    } else {
        if ((msg->type & UPPER_NIBBLE) == (STILL_LIFE << UPPER_NIBBLE_SHIFT)) {
            create_stilllife(msg);

        } else if ((msg->type & UPPER_NIBBLE) == (OSCILLATOR << UPPER_NIBBLE_SHIFT)) {
            create_oscillator(msg);

        } else if ((msg->type & UPPER_NIBBLE) == (SPACE_SHIP << UPPER_NIBBLE_SHIFT)) {
            create_spaceship(msg);
        }
    }
}

/**
  * @brief  Deinitialise all Simulation drivers
  * @param  None
  * @retval None
  */
void s4433912_os_SimTasks_deinit(void) {

    if (s4433912_TaskCAGSimRecvGrid != NULL) {

        vTaskDelete(s4433912_TaskCAGSimRecvGrid);
    }
    if (s4433912_TaskSimLeds != NULL) {

        vTaskDelete(s4433912_TaskSimLeds);
    }
    if (s4433912_TaskSimUpdate != NULL) {

        vTaskDelete(s4433912_TaskSimUpdate);
    }
    if (s4433912_TaskSimDpSend != NULL) {

        vTaskDelete(s4433912_TaskSimDpSend);
    }
}

/**
  * @brief  Initialise all Simulation tasks
  * @param  None
  * @retval None
  */
void SimTasks_init(void) {

    s4433912_TaskSimLeds = NULL;
    s4433912_TaskCAGSimRecvGrid = NULL;
    s4433912_TaskSimDpSend = NULL;
    s4433912_TaskSimUpdate = NULL;

    if (s4433912_TaskSimLeds == NULL) {

        xTaskCreate( (void*) &s4433912_os_TaskSimLeds, (const signed char *) "SIM_LED", mainSINGLE_STACK_SIZE, NULL, mainMEDIUMTASK_PRIORITY, &s4433912_TaskSimLeds);
    }
    if (s4433912_TaskCAGSimRecvGrid == NULL) {

        xTaskCreate( (void*) &s4433912_os_CAGSim_TaskCmdRecv, (const signed char *) "SIM", mainSINGLE_STACK_SIZE*5, NULL, mainMEDIUMTASK_PRIORITY, &s4433912_TaskCAGSimRecvGrid);
    }
    if (s4433912_TaskSimDpSend == NULL) {

        xTaskCreate( (void*) &s4433912_os_TaskSimDpSend, (const signed char *) "SIM_DP", mainTRIPLE_STACK_SIZE, NULL, mainMEDIUMTASK_PRIORITY, &s4433912_TaskSimDpSend);
    }
    if (s4433912_TaskSimUpdate == NULL) {

        xTaskCreate( (void*) &s4433912_os_TaskSimUpdate, (const signed char*) "SIM_UD", mainTRIPLE_STACK_SIZE, NULL, mainLOWTASK_PRIORITY, &s4433912_TaskSimUpdate);
    }
}

/**
  * @brief  Initialises all Simulation tasks
  * @param  None
  * @retval None
  */
extern void s4433912_os_SimTasks_init(void) {

    #if TASK_SYNC
    EventGroupSync = xEventGroupCreate(); //sync leds and sim
    #endif
    //Reset collection
    for (int i = 0; i < GRID_WIDTH; i++) {
        memset(cellCollection[i], DEAD, sizeof(cellCollection[i]));
    }

    s4433912_TaskSimKpRecv = NULL;
    if (s4433912_TaskSimKpRecv == NULL) {

        xTaskCreate( (void*) &s4433912_os_TaskSimKpRecv, (const signed char *) "SIM_KP", mainDOUBLE_STACK_SIZE, NULL, mainMEDIUMTASK_PRIORITY, &s4433912_TaskSimKpRecv);
    }
    SimTasks_init();
}

/**
  * @brief  Processes joystick signals and keypad commands sent to sim event group
  * @param  value - received event group
  * @retval None
  */
void EventGroup_handler(EventBits_t value) {

    if (value & (1 << CLEAR_GRID)) {
        //clear grid, kill all cells=
        for (int x = 0; x < GRID_WIDTH; x++) {
            for (int y = 0; y < GRID_HEIGHT; y++) {
                if ((cellCollection[x][y] & (1 << LIVE)) == ALIVE) {

                    cellCollection[x][y] = ((DEAD << LIVE) | (1 << CHANGE));
                }
            }
        }

    } else if (value & (1 << START_SIM)) {

        update = SIM_ON;
    } else if (value & (1 << STOP_SIM)) {

        update = SIM_OFF;
    } else if (value & (1 << SIM_UPDATE_0_5)) {

        updatePeriodToSet = 500 - TICK_CAL_0_5;
    } else if (value & (1 << SIM_UPDATE_1)) {

        updatePeriodToSet = 1000 - TICK_CAL_1;
    } else if (value & (1 << SIM_UPDATE_2)) {

        updatePeriodToSet = 2000 - TICK_CAL_2;
    } else if (value & (1 << SIM_UPDATE_5)) {

        updatePeriodToSet = 5000 - TICK_CAL_5;
    } else if (value & (1 << SIM_UPDATE_10)) {

        updatePeriodToSet = 10000 - TICK_CAL_10;
    } else if (value & (1 << CRE_SIM)) {

        SimTasks_init();
    } else if (value & (1 << CRE_JSK)) {

        xTimerStart(s4433912_TimerJoystickInit, 0);
    } else if (value & (1 << DLT_SIM)) {

        s4433912_os_SimTasks_deinit();
    } else if (value & (1 << DLT_JSK)) {

        xTimerStart(s4433912_TimerJoystickDeinit, 0);
    }
    //Update timer if update period changes
    if (updatePeriodToSet != updatePeriod) {

        updatePeriod = updatePeriodToSet;
        if (update == SIM_ON) {

            xTaskAbortDelay(s4433912_TaskSimUpdate); //Break out of blocked state to a new update time
            xTaskAbortDelay(s4433912_TaskSimLeds);
        }
    }

}

/**
  * @brief  Task that receives and executes commands from keypad
  * @param  None
  * @retval None
  */
void s4433912_os_CAGSim_TaskCmdRecv(void) {

    TickType_t xTicksToWait = 5;
    for (;;) {
        caMessage_t msg;  //new variable every loop
        if (s4433912_QueueCAGSimMsg != NULL) {

            if (uxQueueMessagesWaiting(s4433912_QueueCAGSimMsg) != 0 ) {
                //Queue should not be full
            }
            if (xQueueReceive(s4433912_QueueCAGSimMsg, (void*) &msg, xTicksToWait) == pdPASS) {
                //update cellCollection
                caMsg_handler(&msg);
            }
        }
        vTaskDelay(xTicksToWait);
    }
}

/**
  * @brief  Task that receives eventgroup and executes according set signals
  * @param  None
  * @retval None
  */
void s4433912_os_TaskSimKpRecv(void) {

    TickType_t xTicksToWait = 5;
    EventBits_t xEventGroupValue;
    for(;;) {
        if (s4433912_EventGroupSimKPFncs != NULL) {
            //Wait for all bits and clear on exit
            xEventGroupValue = xEventGroupWaitBits(s4433912_EventGroupSimKPFncs, 0xFFF, pdTRUE, pdFALSE, xTicksToWait);
            EventGroup_handler(xEventGroupValue);
            xTaskNotifyGive(s4433912_TaskSimDpSend);
        }
        vTaskDelay(xTicksToWait);
    }
}

/**
  * @brief  Task that executes simulation features and updates the display
  * @param  None
  * @retval None
  */
void s4433912_os_TaskSimDpSend(void) {

    EventBits_t xEventGroupValue;
    TickType_t xTicksToWait = 5;

    for (;;) {
        //Wait for keypad process first
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (s4433912_QueueDspGridRead != NULL ) {

            if (xQueueSendToFront(s4433912_QueueDspGridRead, (void*) &cellCollection, xTicksToWait) == pdPASS) {

                //Sync with update task and led task
                if (update == SIM_ON) {

                    xTaskNotifyGive(s4433912_TaskSimUpdate);
                    xTaskNotifyGive(s4433912_TaskSimLeds);
                }
                //Zero change bit, ready for next update
                for (int x = 0; x < GRID_WIDTH; x++) {
                    for (int y = 0; y < GRID_HEIGHT; y++) {
                        cellCollection[x][y] |= (0 << CHANGE);
                    }
                }
            }
        }
        vTaskDelay(xTicksToWait);
    }
}

/**
  * @brief  Task that toggles led 9 in sync timely with simulation update
  * @param  None
  * @retval None
  */
void s4433912_os_TaskSimLeds(void) {

    uint8_t led = 0b11000000; //Toggle led, 8th bit on meaning the packet is from Sim
    TickType_t xTicksToWait = 5;

    for (;;) {
        //Extra delay if sim is on
        if (update == SIM_ON) {

            if (s4433912_QueueLedMsgSend != NULL) {

                led ^= 0b01000000; //Toggle led bit
                //Sync with update task
                ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
                xQueueSendToFront(s4433912_QueueLedMsgSend, (void*) &led, xTicksToWait);
            }
            #if TASK_SYNC
            //Sync up with Sim task
            xEventGroupSync(EventGroupSync, LED_TASK_BIT, (SIM_TASK_BIT | LED_TASK_BIT), portMAX_DELAY);
            #endif
            vTaskDelay(pdMS_TO_TICKS(updatePeriod));
        } else {

            vTaskDelay(xTicksToWait);
        }
    }
}

/**
  * @brief  Task that updates the game of life generation
  * @param  None
  * @retval None
  */
extern void s4433912_os_TaskSimUpdate(void) {

    TickType_t xTicksToWait = 5;

    for (;;) {
        //Extra delay if sim is on
        if (update == SIM_ON) {
            //Sync up with SimKP
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
            //Run Game of Life
            update_generation(); 
            #if TASK_SYNC
            //Sync up with led task
            xEventGroupSync(EventGroupSync, SIM_TASK_BIT, (SIM_TASK_BIT | LED_TASK_BIT), portMAX_DELAY);
            #endif
            vTaskDelay(pdMS_TO_TICKS(updatePeriod));
        } else {
            
            vTaskDelay(xTicksToWait);
        }
    }
}

