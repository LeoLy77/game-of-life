/**
 ***************************************************************
 * @file mylib/project/CAG_keypad_mnemonic.c
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

/* Includes ------------------------------------------------------------------*/
#include "s4433912_CAG_keypad_mnemonic.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define KEYPAD_TIMEOUT 600
//KEYPAD FSM
#define DEINIT_STATE 0
#define INIT_STATE 1
#define INCREASE_STATE 2
#define KEY_79_STATE 9
//COMMAND DETECTION FUNCTION POINTER COUNT
#define GEN_DETECT 0
#define ONE_DETECT 3
#define TWO_DETECT 4
#define THREE_DETECT 5

#define COMMAND_NUM 5
#define CHAR_BUFFER 6
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//Telephone keypad LUT
const char keypadLUT[12][5] = {"0", "1", "2ABC", "3DEF", "4GHI", "5JKL", "6MNO", "7PQRS", "8TUV", "9WXYZ", "*", "#"};
//3-char commands
const char commandLUT[COMMAND_NUM][4] = {"STL", "OSC", "GLD", "DLT", "CRE"};
//Received pressed key buffer
char recvChars[CHAR_BUFFER] = "\0";
uint8_t recvCharCnt = 0;
//Count how many time the same is pressed
uint8_t press_cnt = 0;
//When a command is detected, char received is forwarded as parameters for the command
uint8_t commandFsmState;
//Previously available bit in EventGroup
char prevBit;
//Time interval between presses
TickType_t interval = 0;
//fsm state
uint8_t KeypadFsmCurrentState = DEINIT_STATE;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  find if set bit in keypad eventgroup is the key 
  *         that is used in this mode
  * @param  bits - received keypad event group
  * @retval boolean
  */
uint8_t single_char_key(EventBits_t bits) {

    return ((bits & KEYPAD_KEY_0) == KEYPAD_KEY_0 ||
                (bits & KEYPAD_KEY_1) == KEYPAD_KEY_1 ||
                (bits & KEYPAD_KEY_F) == KEYPAD_KEY_F || 
                (bits & KEYPAD_KEY_E) == KEYPAD_KEY_E ||
                (bits & KEYPAD_KEY_A) == KEYPAD_KEY_A ||
                (bits & KEYPAD_KEY_B) == KEYPAD_KEY_B ||
                (bits & KEYPAD_KEY_C) == KEYPAD_KEY_C ||
                (bits & KEYPAD_KEY_D) == KEYPAD_KEY_D );
}

/**
  * @brief  mnemonic keypad fsm
  *         interprets key presses to characters following 
            the alphanumeric telephone keypad behaviour
  * @param  bits - received keypad event group
  * @retval None
  */
void keypad_fsm(EventBits_t bits) {

    switch (KeypadFsmCurrentState) {

        case INIT_STATE:

            if (single_char_key(bits)) {

                KeypadFsmCurrentState = INIT_STATE;
            } else {

                if ((bits & KEYPAD_KEY_9) == KEYPAD_KEY_9 ||
                    (bits & KEYPAD_KEY_7) == KEYPAD_KEY_7) {

                    KeypadFsmCurrentState = KEY_79_STATE;
                } else {

                    KeypadFsmCurrentState = INCREASE_STATE;
                }
            }
            press_cnt = 0;
            break;

        case INCREASE_STATE:

            if (single_char_key(bits)) {

                KeypadFsmCurrentState = INIT_STATE;
                press_cnt = 0;
            } else {
                
                if ((bits & KEYPAD_KEY_9) == KEYPAD_KEY_9 ||
                    (bits & KEYPAD_KEY_7) == KEYPAD_KEY_7) {

                    KeypadFsmCurrentState = KEY_79_STATE;
                    press_cnt = 0;
                //These keys only have 4 characters
                } else if (press_cnt == 3 || (bits & prevBit) != prevBit ||
                    interval > pdMS_TO_TICKS(KEYPAD_TIMEOUT)) {

                    KeypadFsmCurrentState = INCREASE_STATE;
                    press_cnt = 0;
                } else {

                    KeypadFsmCurrentState = INCREASE_STATE;
                    press_cnt++;
                }
            }
            break;

        case KEY_79_STATE:
            //Go back to the first character on timeout
            if (interval > pdMS_TO_TICKS(KEYPAD_TIMEOUT)) {

                KeypadFsmCurrentState = KEY_79_STATE;
                press_cnt = 0;      
            } else if (single_char_key(bits)){
                
                KeypadFsmCurrentState = INIT_STATE;
                press_cnt = 0;
            } else {

                if ((bits & KEYPAD_KEY_9) == KEYPAD_KEY_9 ||
                    (bits & KEYPAD_KEY_7) == KEYPAD_KEY_7) {

                    KeypadFsmCurrentState = KEY_79_STATE;
                    //Key 9 has 5 characters
                    if (press_cnt == 4) {

                        press_cnt = 0;
                    } else {

                        press_cnt++;
                    }
                } else {

                    KeypadFsmCurrentState = INCREASE_STATE;
                    press_cnt++;
                }
            }
            break;

        case DEINIT_STATE:

            KeypadFsmCurrentState = DEINIT_STATE;
            press_cnt = 0;
            break;

        default:
            break;
    }
    prevBit = (bits & prevBit);
}

/**
  * @brief  interprets key pressed from keypad eventgroup
  * @param  bits - received keypad event group
  * @retval key pressed
  */
char EventGroup_mnc_handler(EventBits_t bits) {

    char kp_out;
    keypad_fsm(bits);

    if ((bits & KEYPAD_KEY_2) == KEYPAD_KEY_2) {

        kp_out = keypadLUT[2][press_cnt];
    } else if ((bits & KEYPAD_KEY_3) == KEYPAD_KEY_3) {
        
        kp_out = keypadLUT[3][press_cnt];
    } else if ((bits & KEYPAD_KEY_4) == KEYPAD_KEY_4) {

        kp_out = keypadLUT[4][press_cnt];
    } else if ((bits & KEYPAD_KEY_5) == KEYPAD_KEY_5) {

        kp_out = keypadLUT[5][press_cnt];
    } else if ((bits & KEYPAD_KEY_6) == KEYPAD_KEY_6) {

        kp_out = keypadLUT[6][press_cnt];
    } else if ((bits & KEYPAD_KEY_7) == KEYPAD_KEY_7) {

        kp_out = keypadLUT[7][press_cnt];
    } else if ((bits & KEYPAD_KEY_8) == KEYPAD_KEY_8) {

        kp_out = keypadLUT[8][press_cnt];
    } else if ((bits & KEYPAD_KEY_9) == KEYPAD_KEY_9) {

        kp_out = keypadLUT[9][press_cnt];
    } else if ((bits & KEYPAD_KEY_0) == KEYPAD_KEY_0) {

        kp_out = keypadLUT[0][0];
    } else if ((bits & KEYPAD_KEY_F) == KEYPAD_KEY_F) {

        kp_out = keypadLUT[10][0];
    } else if ((bits & KEYPAD_KEY_D) == KEYPAD_KEY_D) {

        kp_out = keypadLUT[11][0];
    } else if ((bits & KEYPAD_KEY_1) == KEYPAD_KEY_1) {

        kp_out = keypadLUT[1][0];
    } else {

        kp_out = '?';
    }
    
    return kp_out;
}

/**
 * @brief  Shift the received sequence of characters left by {bit} characters 
 * @param  bit - how many bits you want to shift, max {CHAR_BUFFER}
 * @retval None
 */
void shift_left_str(int bit) {

    while(bit > 0) {

        recvCharCnt--;
        recvChars[bit - 1] = recvChars[bit];
        recvChars[bit] = recvChars[bit + 1];
        recvChars[bit + 1] = '\0'; //invalid char
        bit--;
    }
}

/**
 * @brief  tailor caMessage_t and send to Simulation queue, reset fsm and clear buffer on the way out 
 * @param  None
 * @retval None
 */
void send_command(void) {

    caMessage_t msg;
    int typeBit, pos_x, pos_y;
    msg.type = 0;
    //Identify commmand by the first char
    if (recvChars[0] == 'S' || recvChars[0] == 'O') {
        //Translate subgrid pos to cell pos, place cell top left of the subgrid
        pos_x = (recvChars[4] - '0');
        msg.cell_x = pos_x*SUBGRID_SIZE;
        pos_y = (recvChars[5] - '0');
        msg.cell_y = pos_y*SUBGRID_SIZE;
        if (recvChars[0] == 'S') {

            msg.type |= (STILL_LIFE << UPPER_NIBBLE_SHIFT);
        } else if (recvChars[0] == 'O') {

            msg.type |= (OSCILLATOR << UPPER_NIBBLE_SHIFT);
        }
        msg.type |= ((recvChars[3] - '0') & LOWER_NIBBLE); //Type selection

    } else if (recvChars[0] == 'G') {
        //There is only 1 type of space ship
        pos_x = (recvChars[3] - '0');
        pos_y = (recvChars[4] - '0');
        msg.type |= (SPACE_SHIP << UPPER_NIBBLE_SHIFT);
        msg.cell_x = pos_x*SUBGRID_SIZE;
        msg.cell_y = pos_y*SUBGRID_SIZE;
    }

    if (s4433912_QueueCAGSimMsg != NULL) {

        xQueueSendToFront(s4433912_QueueCAGSimMsg, (void*) &msg, 10);
    }
    //Clear buffer, ready for next command
    KeypadFsmCurrentState = INIT_STATE;
    recvCharCnt = 0;
    for (int i = 0; i < CHAR_BUFFER; i++) {
        recvChars[i] = '\0';
    }
}


/**
 * @brief  detect single character commands
 * @param  in - input character registered from keypad
 * @retval None
 */
void detect_short_commands(char in) {

    if (in == '#' || (in == '0' && commandFsmState == GEN_DETECT) || in == '*') {

        if (in == '#') {

            xEventGroupSetBits(s4433912_EventGroupSimKPFncs, (1 << STOP_SIM));
        } else if (in == '0') {

            xEventGroupSetBits(s4433912_EventGroupSimKPFncs, (1 << CLEAR_GRID));
        } else if (in == '*') {

            xEventGroupSetBits(s4433912_EventGroupSimKPFncs, (1 << START_SIM));
        }
        //Clear buffer, ready for next command
        KeypadFsmCurrentState = INIT_STATE;
        recvCharCnt = 0;
        for (int i = 0; i < CHAR_BUFFER; i++) {
            recvChars[i] = '\0';
        }
    } else {

        recvChars[recvCharCnt] = in;
        recvCharCnt++;
    }
    s4433912_grid_printf(0, "Recv %s", recvChars);
}


/**
 * STL | OSC | GLD | DLT | CRE
 * @brief  detect commands with 3 characters in buffer, 
 *         and direct fsm to the next state depending on how many input parameters that command takes
 * @param  in - input character registered from keypad
 * @retval None
 */
void detect_long_commands(char in) {

    uint8_t match = 0; //matching command from buffer flag
    if (recvCharCnt == 3) {

        for (int i = 0; i < COMMAND_NUM; i++) {
            //Check if string matches any commands
            if (strncmp(commandLUT[i], recvChars, sizeof(char)*3) == 0) {

                s4433912_grid_printf(0, "Matched command %c%c%c", recvChars[0], recvChars[1], recvChars[2]);
                //Commands that take 3 parameters
                if (i == 0 || i == 1) { 

                    commandFsmState = THREE_DETECT;
                //Commands that take 2 parameters
                } else if (i == 2) {

                    commandFsmState = TWO_DETECT;
                //Commands that take 1 parameter
                } else {

                    commandFsmState = ONE_DETECT;
                }
                match = 1;
                break;
            }
        }
        if (!match) {
            //search sequentially, shift left by 1 making room for the next char
            shift_left_str(1);
        }
    } 
}   

/**
  * @brief  handle parameter input for one-parameter commands - DLT and CRE
  * @param  in - input character registered from keypad
  * @retval None
  */
void one_para_command(char in) {
    //DLT and CRE only take either 0 or 1 as parameter
    if (in != '0' && in != '1') {

        shift_left_str(1);
        commandFsmState = GEN_DETECT;
    } else {
        //Send to command queue
        //DLT
        if (recvChars[0] == 'D') {

            if (in == '0') {

                xEventGroupSetBits(s4433912_EventGroupSimKPFncs, (1 << DLT_SIM));
            } else if (in == '1') {

                xEventGroupSetBits(s4433912_EventGroupSimKPFncs, (1 << DLT_JSK));
            }
        //CRE
        } else if (recvChars[0] == 'C') {

            if (in == '0') {

                xEventGroupSetBits(s4433912_EventGroupSimKPFncs, (1 << CRE_SIM));
            } else if (in == '1') {

                xEventGroupSetBits(s4433912_EventGroupSimKPFncs, (1 << CRE_JSK));
            }
        }
        commandFsmState = GEN_DETECT;
        //Clear buffer, ready for next command
        recvCharCnt = 0;
        for (int i = 0; i < CHAR_BUFFER; i++) {
            recvChars[i] = '\0';
        }
    }
}

/**
  * @brief  handle parameter input for two-parameter commands - GLD
  * @param  in - input character registered from keypad
  * @retval None
  */
void two_para_command(char in) {
    //Convert char to an int
    int recvNum = recvChars[recvCharCnt - 1] - '0';

    if (recvNum <= 9) {

        if (recvNum >= SUBGRID_SIZE) {

            shift_left_str(1);
            commandFsmState = GEN_DETECT;
        } else {

            if (((recvChars[0] == 'S' || recvChars[0] == 'O') && recvCharCnt == THREE_DETECT + 1) ||
                (recvChars[0] == 'G' && recvCharCnt == TWO_DETECT + 1)) {
                //Send to command queue
                send_command();
                commandFsmState = GEN_DETECT;
            }
        }
    } else {

        shift_left_str(1);
        commandFsmState = GEN_DETECT;
    }
}

/**
  * @brief  handle parameter input for three-parameter commands - STL and OSC
  * @param  in - input character registered from keypad
  * @retval None
  */
void thr_para_command(char in) {
    //Convert char to an int
    int recvNum = recvChars[recvCharCnt - 1] - '0';

    //See if it's STL or OSC
    if (recvNum <= 9 & ((recvChars[0] == 'S' && recvNum <= 3) ||
        (recvChars[0] == 'O' && recvNum <= 2))) {

        commandFsmState = TWO_DETECT;
    } else {

        shift_left_str(2);
        commandFsmState = GEN_DETECT;
    }
}

/**
  * @brief  identifies commands from key pressed 
  *         in alphanumeric telephone keypad manners
  *         and sends it to the simulation
  * @param  in - input character registered from keypad
  * @retval None
  */
void command_fsm(char in) {

    detect_short_commands(in);
    switch (commandFsmState) {

        case GEN_DETECT:

            detect_long_commands(in);
            break;

        case ONE_DETECT:

            one_para_command(in);
            break;

        case TWO_DETECT:

            two_para_command(in);
            break;
        
        case THREE_DETECT:

            thr_para_command(in);
            break;

        default:
            break;

    }
}


/**
  * @brief  Task that receives key presses from the keypad,
  *         interprets the key presses in alphanumeric telephone behaviour,
  *         indentifies matching simulation commands and sends it to the simulation
  * @param  None
  * @retval None
  */
extern void s4433912_os_CAGMnc_TaskSendToSim(void) {

    EventBits_t xEventGroupRecv;
    TickType_t xTicksToWait = 5;
    //delay variable between presses
    portTickType last_tick = 0;
    portTickType wait_ready = 0;
    uint8_t new_output = 0; //new key press registered
    char kp_out = '\0';
    KeypadFsmCurrentState = INIT_STATE;
    commandFsmState = GEN_DETECT;

    for (;;) {
        //Only runs if has keypad semaphore
        if (s4433912_keypad_mode == KP_MNC) {

            if (s4433912_QueueKeypadRead != NULL && s4433912_EventGroupSimKPFncs != NULL) {

                if (uxQueueMessagesWaiting(s4433912_QueueKeypadRead) != 0) {
                    //Queue should not be full
                }
                //Check the queue for key press
                if (xQueueReceive(s4433912_QueueKeypadRead, (void*) &xEventGroupRecv, 10) == pdPASS) {
                    //Give some time in between key presses to select character on the same key
                    interval = (xTaskGetTickCount() - last_tick);
                    kp_out = EventGroup_mnc_handler(xEventGroupRecv);

                    last_tick = xTaskGetTickCount();
                    wait_ready = xTaskGetTickCount();
                    new_output = 1;
                } else {
                    //Failed to receive
                }
                //Lock in character after a short time of no key press
                if (new_output == 1 && (xTaskGetTickCount() - wait_ready > KEYPAD_TIMEOUT)) {
                    
                    command_fsm(kp_out);
                    wait_ready = xTaskGetTickCount();
                    new_output = 0; //reset
                }
            }        
        }
        vTaskDelay(xTicksToWait);
    }
}
