/**
 ***************************************************************
 * @file mylib/project/CAG_keypad_grid.c
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
/* Includes ------------------------------------------------------------------*/
#include "s4433912_CAG_keypad_grid.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
position_t origin; //current subgrid starting position 
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  reads key presses and moves cell to its positions
  *         also set event group bits
  * @param  bits - received even group bits
  * @retval EventGroupSet - to disingtuish if a number key or char key was pressed
  */
uint8_t EventGroup_grid_handler(EventBits_t bits, caMessage_t* cell) {

    uint8_t EventGroupSet = 0;

    if ((bits & KEYPAD_KEY_1) == KEYPAD_KEY_1) {

        cell->cell_x = 0 + origin.x;
        cell->cell_y = 0 + origin.y;
    } else if ((bits & KEYPAD_KEY_2) == KEYPAD_KEY_2) {

        cell->cell_x = 1 + origin.x;
        cell->cell_y = 0 + origin.y;
    } else if ((bits & KEYPAD_KEY_3) == KEYPAD_KEY_3) {

        cell->cell_x = 2 + origin.x;
        cell->cell_y = 0 + origin.y;
    } else if ((bits & KEYPAD_KEY_A) == KEYPAD_KEY_A) {

        xEventGroupSetBits(s4433912_EventGroupSimKPFncs, (1 << START_SIM));
        EventGroupSet = 1;
    } else if ((bits & KEYPAD_KEY_4) == KEYPAD_KEY_4) {

        cell->cell_x = 0 + origin.x;
        cell->cell_y = 1 + origin.y;
    } else if ((bits & KEYPAD_KEY_5) == KEYPAD_KEY_5) {

        cell->cell_x = 1 + origin.x;
        cell->cell_y = 1 + origin.y;
    } else if ((bits & KEYPAD_KEY_6) == KEYPAD_KEY_6) {

        cell->cell_x = 2 + origin.x;
        cell->cell_y = 1 + origin.y;
    } else if ((bits & KEYPAD_KEY_B) == KEYPAD_KEY_B) {

        xEventGroupSetBits(s4433912_EventGroupSimKPFncs, (1 << STOP_SIM));
        EventGroupSet = 1;
    } else if ((bits & KEYPAD_KEY_7) == KEYPAD_KEY_7) {

        cell->cell_x = 0 + origin.x;
        cell->cell_y = 2 + origin.y;
    } else if ((bits & KEYPAD_KEY_8) == KEYPAD_KEY_8) {

        cell->cell_x = 1 + origin.x;
        cell->cell_y = 2 + origin.y;
    } else if ((bits & KEYPAD_KEY_9) == KEYPAD_KEY_9) {

        cell->cell_x = 2 + origin.x;
        cell->cell_y = 2 + origin.y;
    } else if ((bits & KEYPAD_KEY_C) == KEYPAD_KEY_C) {

        xEventGroupSetBits(s4433912_EventGroupSimKPFncs, (1 << CLEAR_GRID));
        EventGroupSet = 1;
    } else if ((bits & KEYPAD_KEY_0) == KEYPAD_KEY_0) {

        origin.x += SUBGRID_SIZE;
        if (origin.x >= GRID_WIDTH) {

            origin.x = 0;
        }
        EventGroupSet = 1;
    } else if ((bits & KEYPAD_KEY_F) == KEYPAD_KEY_F) {

        origin.y += SUBGRID_SIZE;
        if (origin.y >= GRID_HEIGHT) {
            origin.y = 0;
        }
        EventGroupSet = 1;
    } else if (((bits & KEYPAD_KEY_E) == KEYPAD_KEY_E) ||
                (bits & KEYPAD_KEY_D) == KEYPAD_KEY_D) {

        EventGroupSet = 1;
    }
    return EventGroupSet;
}

/**
  * @brief  Task that compiles caMessage to send to simulation and updates subgrid position leds
  * @param  None
  * @retval None
  */
extern void s4433912_os_KPgrid_TaskGridSend(void) {

    TickType_t xGenTicksToWait = 10;
    TickType_t xTicksToWait = 5;
    EventBits_t xEventGroupRecv;
    uint8_t EventGroupSet = 0;
    uint8_t leds = 0; //subgrid (0 ,0)
    origin.x = 0;
    origin.y = 0;
    
    for (;;) {
        // Only runs if has keypad semaphore
        if (s4433912_keypad_mode == KP_GRD) {
            //new cell everytime 
            caMessage_t msg; 
            if (s4433912_QueueLedMsgSend != NULL) {
                //Display on the first 6 leds, division to scale down grid size
                leds = (((origin.x/SUBGRID_SIZE) << 3) | ((origin.y/SUBGRID_SIZE) << 0));
                xQueueSendToFront(s4433912_QueueLedMsgSend, (void*) &leds, xGenTicksToWait);
            }
            if (s4433912_QueueKeypadRead != NULL) {

                if (uxQueueMessagesWaiting(s4433912_QueueKeypadRead) != 0) {
                    //Queue should not be full
                }
                if (xQueueReceive(s4433912_QueueKeypadRead, (void*) &xEventGroupRecv, xTicksToWait) == pdPASS) {

                    msg.type = ((CELL << UPPER_NIBBLE_SHIFT));
                    EventGroupSet = EventGroup_grid_handler(xEventGroupRecv, &msg);
                    //EventGroupSet not set meaning cell was set
                    if (s4433912_QueueCAGSimMsg != NULL && EventGroupSet == 0 && s4433912_QueueLta1000gReadMsg != NULL) {

                        xQueueSendToFront(s4433912_QueueCAGSimMsg, (void*) &msg, xGenTicksToWait);
                    }
                } else {
                    //Failed to receive
                }
            }
        }
        vTaskDelay(xTicksToWait);
    }
}
