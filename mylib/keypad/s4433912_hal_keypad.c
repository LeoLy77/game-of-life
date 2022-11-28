/**
 ***************************************************************
 * @file mylib/keypad/s4433912_hal_keypad.c
 * @author Leo Ly - 44339124
 * @date 31/03/2020
 * @brief keypad driver
 * REFERENCE: csse3010_mylib_hal_keypad.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 * s4433912_hal_keypad_init() - Initialise keypad FSM
 * s4433912_hal_keypad_deinit() - Deinitialise keypad FSM
 * keypad_gpio_init() - Initialise keypad GPIOs
 * s4433912_hal_keypad_fsmprocessing() - keypad FSM
 * keypad_writecol(uint8_t colval) - column control
 * keypad_readrow() - read row pins
 * s4433912_hal_keypad_read_status() - get keypad status
 * s4433912_hal_keypad_read_key() - get pressed key
 * s4433912_hal_keypad_read_ascii() - get pressed key in ascii
 ***************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "processor_hal.h"
#include <stdint.h>
#include "debug_printf.h"

#include "s4433912_hal_keypad.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define INIT_STATE 0
#define COL1_STATE 1
#define COL2_STATE 2
#define COL3_STATE 3
#define COL4_STATE 4
#define ROWSCAN_STATE 5
#define DEINIT_STATE 6
/* Private variables ---------------------------------------------------------*/
static uint8_t KeypadFsmCurrentState;
uint8_t currentCol = 0;

uint8_t KeypadStatus;
unsigned char KeypadValue;
unsigned char KeypadValues[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

char AsciiValues[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
/* Private function prototypes -----------------------------------------------*/
#define keypad_col1() keypad_writecol(1)
#define keypad_col2() keypad_writecol(2)
#define keypad_col3() keypad_writecol(3)
#define keypad_col4() keypad_writecol(4)
/* Private functions ---------------------------------------------------------*/
void keypad_writecol(uint8_t colval);
uint8_t keypad_readrow();
void keypad_gpio_init();

/**
  * @brief  set keypad fsm to init state
  * @param  None
  * @retval None
  */
void s4433912_hal_keypad_init() {
  keypad_gpio_init();
  KeypadFsmCurrentState = INIT_STATE;

  HAL_GPIO_WritePin(S4433912_HAL_KEYPAD_COL1PINPORT, S4433912_HAL_KEYPAD_COL1PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(S4433912_HAL_KEYPAD_COL2PINPORT, S4433912_HAL_KEYPAD_COL2PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(S4433912_HAL_KEYPAD_COL3PINPORT, S4433912_HAL_KEYPAD_COL3PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(S4433912_HAL_KEYPAD_COL4PINPORT, S4433912_HAL_KEYPAD_COL4PIN, GPIO_PIN_RESET);
}

/**
  * @brief  set keypad fsm to deinit state
  * @param  None
  * @retval None
  */
void s4433912_hal_keypad_deinit() {

  KeypadFsmCurrentState = DEINIT_STATE;
}


/**
  * @brief  read row outputs and determine key press
  * @param  None
  * @retval None
  */
void row_fsmprocessing() {

  uint8_t rowScan = keypad_readrow() & 0x0f;
  if (currentCol == 1) {

    if (rowScan == 0b1110) {

      KeypadStatus = 1;
      KeypadValue = 0x01;
    } else if (rowScan == 0b1101) {

      KeypadStatus = 1;
      KeypadValue = 0x04;
    } else if (rowScan == 0b1011) {

      KeypadStatus = 1;
      KeypadValue = 0x07;
    } else if (rowScan == 0b0111) {

      KeypadStatus = 1;
      KeypadValue = 0x00;
    } 
    currentCol = 2;
    KeypadFsmCurrentState = COL2_STATE;
  } else if (currentCol == 2) {

    if (rowScan == 0b1110) {
      
      KeypadStatus = 1;
      KeypadValue = 0x02;
    } else if (rowScan == 0b1101) {

      KeypadStatus = 1;
      KeypadValue = 0x05;
    } else if (rowScan == 0b1011) {

      KeypadStatus = 1;
      KeypadValue = 0x08;
    } else if (rowScan == 0b0111) {

      KeypadStatus = 1;
      KeypadValue = 0x0F;
    } 
    currentCol = 3;
    KeypadFsmCurrentState = COL3_STATE;
  } else if (currentCol == 3) {

    if (rowScan == 0b1110) {

      KeypadStatus = 1;
      KeypadValue = 0x03;
    } else if (rowScan == 0b1101) {

      KeypadStatus = 1;
      KeypadValue = 0x06;
    } else if (rowScan == 0b1011) {

      KeypadStatus = 1;
      KeypadValue = 0x09;
    } else if (rowScan == 0b0111) {

      KeypadStatus = 1;
      KeypadValue = 0x0E;
    } 
    currentCol = 4;
    KeypadFsmCurrentState = COL4_STATE;
  } else if (currentCol == 4) {

    if (rowScan == 0b1110) {

      KeypadStatus = 1;
      KeypadValue = 0x0A;
    } else if (rowScan == 0b1101) {

      KeypadStatus = 1;
      KeypadValue = 0x0B;
    } else if (rowScan == 0b1011) {

      KeypadStatus = 1;
      KeypadValue = 0x0C;
    } else if (rowScan == 0b0111) {

      KeypadStatus = 1;
      KeypadValue = 0x0D;
    } 
    currentCol = 1;
    KeypadFsmCurrentState = COL1_STATE;
  }
}

/**
  * @brief  keypad main fsm
  * @param  None
  * @retval None
  */
void s4433912_hal_keypad_fsmprocessing() {
  KeypadStatus = 0;
  switch (KeypadFsmCurrentState) {

    case INIT_STATE:

      currentCol = 1;
      KeypadFsmCurrentState = COL1_STATE;
      break;

    case COL1_STATE:

      keypad_col1();
      KeypadFsmCurrentState = ROWSCAN_STATE;
      break;

    case COL2_STATE:

      keypad_col2();
      KeypadFsmCurrentState = ROWSCAN_STATE;
      break;

    case COL3_STATE:

      keypad_col3();
      KeypadFsmCurrentState = ROWSCAN_STATE;
      break;

    case COL4_STATE:

      keypad_col4();
      KeypadFsmCurrentState = ROWSCAN_STATE;
      break;

    case ROWSCAN_STATE:

      row_fsmprocessing();
      break;

    case DEINIT_STATE:

      KeypadFsmCurrentState = DEINIT_STATE;
      break;
    
    default:
      break;
  }
}

/**
  * @brief  Initialise Joystick
  * @param  None
  * @retval None
  */
void keypad_gpio_init() {

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    
  //Init column pins
  S4433912_HAL_KEYPAD_COL1PINCLK();
  GPIO_InitStructure.Pin  = S4433912_HAL_KEYPAD_COL1PIN;
  HAL_GPIO_Init(S4433912_HAL_KEYPAD_COL1PINPORT, &GPIO_InitStructure);

  S4433912_HAL_KEYPAD_COL2PINCLK();
  GPIO_InitStructure.Pin  = S4433912_HAL_KEYPAD_COL2PIN;
  HAL_GPIO_Init(S4433912_HAL_KEYPAD_COL2PINPORT, &GPIO_InitStructure);

  S4433912_HAL_KEYPAD_COL3PINCLK();
  GPIO_InitStructure.Pin  = S4433912_HAL_KEYPAD_COL3PIN;
  HAL_GPIO_Init(S4433912_HAL_KEYPAD_COL3PINPORT, &GPIO_InitStructure);

  S4433912_HAL_KEYPAD_COL4PINCLK();
  GPIO_InitStructure.Pin  = S4433912_HAL_KEYPAD_COL4PIN;
  HAL_GPIO_Init(S4433912_HAL_KEYPAD_COL4PINPORT, &GPIO_InitStructure);
  //Init row pins

  GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
  GPIO_InitStructure.Pull = GPIO_PULLUP;

  S4433912_HAL_KEYPAD_ROW1PINCLK();
  GPIO_InitStructure.Pin  = S4433912_HAL_KEYPAD_ROW1PIN;
  HAL_GPIO_Init(S4433912_HAL_KEYPAD_ROW1PINPORT, &GPIO_InitStructure);

  S4433912_HAL_KEYPAD_ROW2PINCLK();
  GPIO_InitStructure.Pin  = S4433912_HAL_KEYPAD_ROW2PIN;
  HAL_GPIO_Init(S4433912_HAL_KEYPAD_ROW2PINPORT, &GPIO_InitStructure);

  S4433912_HAL_KEYPAD_ROW3PINCLK();
  GPIO_InitStructure.Pin  = S4433912_HAL_KEYPAD_ROW3PIN;
  HAL_GPIO_Init(S4433912_HAL_KEYPAD_ROW3PINPORT, &GPIO_InitStructure);

  S4433912_HAL_KEYPAD_ROW4PINCLK();
  GPIO_InitStructure.Pin  = S4433912_HAL_KEYPAD_ROW4PIN;
  HAL_GPIO_Init(S4433912_HAL_KEYPAD_ROW4PINPORT, &GPIO_InitStructure);

  KeypadStatus = 0;
}

/**
  * @brief  write selected column pin high
  * @param  colval - selected column
  * @retval None
  */
void keypad_writecol(uint8_t colval) {

  GPIO_TypeDef* port = 0;
  uint32_t pin = 0;

  HAL_GPIO_WritePin(S4433912_HAL_KEYPAD_COL1PINPORT, S4433912_HAL_KEYPAD_COL1PIN, GPIO_PIN_SET);
  HAL_GPIO_WritePin(S4433912_HAL_KEYPAD_COL2PINPORT, S4433912_HAL_KEYPAD_COL2PIN, GPIO_PIN_SET);
  HAL_GPIO_WritePin(S4433912_HAL_KEYPAD_COL3PINPORT, S4433912_HAL_KEYPAD_COL3PIN, GPIO_PIN_SET);
  HAL_GPIO_WritePin(S4433912_HAL_KEYPAD_COL4PINPORT, S4433912_HAL_KEYPAD_COL4PIN, GPIO_PIN_SET);

  switch (colval) {

    case 1:

      pin = S4433912_HAL_KEYPAD_COL1PIN;
      port = S4433912_HAL_KEYPAD_COL1PINPORT;
      break;

    case 2:

      pin = S4433912_HAL_KEYPAD_COL2PIN;
      port = S4433912_HAL_KEYPAD_COL2PINPORT;
      break;

    case 3:

      pin = S4433912_HAL_KEYPAD_COL3PIN;
      port = S4433912_HAL_KEYPAD_COL3PINPORT;
      break;

    case 4:

      pin = S4433912_HAL_KEYPAD_COL4PIN;
      port = S4433912_HAL_KEYPAD_COL4PINPORT;
      break;
    
    default:
      break;

  }
  HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
}

/**
  * @brief  read row gpio pins
  * @param  None
  * @retval bitwise row values
  */
uint8_t keypad_readrow() {

  uint8_t row1 = HAL_GPIO_ReadPin(S4433912_HAL_KEYPAD_ROW1PINPORT, S4433912_HAL_KEYPAD_ROW1PIN);
  uint8_t row2 = HAL_GPIO_ReadPin(S4433912_HAL_KEYPAD_ROW2PINPORT, S4433912_HAL_KEYPAD_ROW2PIN);
  uint8_t row3 = HAL_GPIO_ReadPin(S4433912_HAL_KEYPAD_ROW3PINPORT, S4433912_HAL_KEYPAD_ROW3PIN);
  uint8_t row4 = HAL_GPIO_ReadPin(S4433912_HAL_KEYPAD_ROW4PINPORT, S4433912_HAL_KEYPAD_ROW4PIN);
  return (row4 << 3 | row3 << 2 | row2 << 1 | row1);
}

/**
  * @brief  return keypad status
  * @param  None
  * @retval keypad status
  */
uint8_t s4433912_hal_keypad_read_status() {

  return KeypadStatus;
}

/**
  * @brief  return the current keypad value
  * @param  None
  * @retval keypad value
  */
unsigned char s4433912_hal_keypad_read_key() {

  return KeypadValue;
}

/**
  * @brief  return ascii character of the current keypad value
  * @param  None
  * @retval ascii character
  */
char s4433912_hal_keypad_read_ascii() {

  return AsciiValues[KeypadValue];
}
