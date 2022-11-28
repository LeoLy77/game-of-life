/**
  ******************************************************************************
  * @file    main.c
  * @author  44339124 - Leo Ly
  * @date    04/04/2020
  * @brief   stage3 main.c
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "processor_hal.h"
#include "debug_printf.h"

#ifdef S4433912_MYLIB_CONFIGURE_INC
#include "s4433912_hal_mylib_configure.h"
#endif
#include "s4433912_lib_hamming.h"
#include "s4433912_hal_keypad.h"
#include "s4433912_hal_atimer.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define IDLE 0
#define ENCODE 1
#define DECODE 2
/* Private variables ---------------------------------------------------------*/
uint8_t hammingCodeInCnt;
uint8_t hammingKpVal[2];
uint8_t hammingFsmState = IDLE;
char first_press, second_press;
/* Private function prototypes -----------------------------------------------*/
void Hardware_init(void);
void hamming_fsm();

/**
  * @brief  Main program - flashes onboard LEDs
  * @param  None
  * @retval None
  */
void main(void)  {
  BRD_init();			//Initalise Board
  Hardware_init();	//Initalise hardware modules

  HAL_Delay(20);
  /* Main processing loop */

  while (1) {
  }
}

/**
  * @brief  Initialise Hardware
  * @param  None
  * @retval None
  */
void Hardware_init(void) {

  BRD_LEDInit();	
  /* Turn off LEDs */
  BRD_LEDRedOff();
  BRD_LEDGreenOff();
  BRD_LEDBlueOff();

  s4433912_hal_atimer_init();
  s4433912_hal_atimer_init_pin();
  s4433912_hal_atimer_period_set(50); //50ms
  s4433912_hal_keypad_init();
}


/**
  * @brief  hamming cyclic executive fsm, called every time a key pressed
  * @param  None
  * @retval None
  */
void hamming_fsm() {
  uint8_t first_hamming, second_hamming;
  uint16_t hamming_out;

  // debug_printf("hamming called %d cnt %d\n\r", hammingFsmState, hammingCodeInCnt);

  switch (hammingFsmState) {
    case IDLE:
      hammingCodeInCnt = 0;
      if (first_press == 'E' && second_press == 'E') {
        hammingFsmState = ENCODE;
        
      } else if (first_press == 'D' && second_press == 'D') {
        hammingFsmState = DECODE;

      }
      break;
    
    case ENCODE:
      if (first_press == 'D' && second_press == 'D') {
        hammingFsmState = DECODE;
        hammingCodeInCnt = 0;

      } else if (first_press == 'F' && second_press == 'F') {
        hammingFsmState = IDLE;

      } else if (first_press == 'E' && second_press == 'E') {
        hammingFsmState = ENCODE;
        hammingCodeInCnt = 0;

      } else {
        hammingFsmState = ENCODE;
        hammingKpVal[hammingCodeInCnt] = (uint8_t) s4433912_hal_keypad_read_key();
        hammingCodeInCnt++;

        if (hammingCodeInCnt == 2) {
          hamming_out = s4433912_lib_hamming_byte_encode(hammingKpVal[0] << 4 | hammingKpVal[1]);
          // //Extract 3 hamming bits from the second position of encoded bits
          // first_hamming = (((1 << 3) - 1) & (hamming_out >> (2 - 1)));
          // second_hamming = (((1 << 3) - 1) & (hamming_out >> (9 - 1)));
          // debug_printf("HE %x%x%x%x\n\r", hammingKpVal[0], second_hamming, hammingKpVal[1], first_hamming);

          debug_printf("HE %x\n\r", hamming_out);
          hammingCodeInCnt = 0;
        }
      }

      break;

    case DECODE:
      if (first_press == 'F' && second_press == 'F') {
        hammingFsmState = IDLE;

      } else if (first_press == 'E' && second_press == 'E') {
        hammingFsmState = ENCODE;
        hammingCodeInCnt = 0;

      } else if (first_press == 'D' && second_press == 'D') {
        hammingFsmState = DECODE;
        hammingCodeInCnt = 0;

      } else {
        hammingFsmState = DECODE;
        hammingKpVal[hammingCodeInCnt] = (uint8_t) s4433912_hal_keypad_read_key();
        hammingCodeInCnt++;

        if (hammingCodeInCnt == 2) {
          hamming_out = s4433912_lib_hamming_byte_decode(hammingKpVal[0] << 4 | hammingKpVal[1]);
          debug_printf("HD %x P%d\n\r", hamming_out, s4433912_lib_hamming_parity_error(hammingKpVal[0] << 4 | hammingKpVal[1]));
          hammingCodeInCnt = 0;
        }

      }

      break;

    default:
      break;
  }
}

/**
  * @brief  Timer interrupt handler
  * @param  htim - timer called
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM3) {
    atimer_isr_callback();
    s4433912_hal_keypad_fsmprocessing();

    if (s4433912_hal_keypad_read_status()) {
      first_press = second_press;
      second_press = s4433912_hal_keypad_read_ascii();
      debug_printf("<%c><%c>\n\r", first_press, second_press);

      hamming_fsm();
    }
  }
}




