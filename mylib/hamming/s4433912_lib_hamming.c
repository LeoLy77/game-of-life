/**
 ***************************************************************
 * @file mylib/keypad/s4433912_hal_hamming.c
 * @author Leo Ly - 44339124
 * @date 07/04/2020
 * @brief hamming driver
 * REFERENCE: csse3010_mylib_hal_hamming.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 * s4433912_lib_hamming_byte_encode() - encode a byte
 * s4433912_lib_hamming_byte_decode() - decode half a byte
 * hamming_hbyte_encode() - encode half a byte
 * s4433912_lib_hamming_parity_error(uint8_t byte) - return error 
 ***************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "debug_printf.h"
#include "s4433912_lib_hamming.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t error = 0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/**
 * 
 * @brief  encode half a byte
 * @param  in - 4-bit message to be encoded
 * @retval 8-bit encoded message
 * 
 * Implement Hamming Code + parity checking
 * Hamming code is based on the following generator and parity check matrices
 * G = [ 0 1 1 | 1 0 0 0 ;
 *       1 0 1 | 0 1 0 0 ;
 *       1 1 0 | 0 0 1 0 ;
 *       1 1 1 | 0 0 0 1 ;
 *
 * hence H =
 * [ 1 0 0 | 0 1 1 1 ;
 *   0 1 0 | 1 0 1 1 ;
 *   0 0 1 | 1 1 0 1 ];
 *
 * y = x * G, syn = H * y'
 *
 *
 * NOTE: !! is used to get 1 out of non zeros
 * 
 *
 */
uint8_t hamming_hbyte_encode(uint8_t in) {
  uint8_t d0, d1, d2, d3;
  uint8_t p0 = 0, h0, h1, h2;
  uint8_t z;
  uint8_t out;

  /* extract bits */
  d0 = !!(in & 0x1);
  d1 = !!(in & 0x2);
  d2 = !!(in & 0x4);
  d3 = !!(in & 0x8);

  /* calculate hamming parity bits */
  h0 = d1 ^ d2 ^ d3;
  h1 = d0 ^ d2 ^ d3;
  h2 = d0 ^ d1 ^ d3;

  /* generate out byte without parity bit P0 */
  out = (h0 << 1) | (h1 << 2) | (h2 << 3) |
    (d0 << 4) | (d1 << 5) | (d2 << 6) | (d3 << 7);

  /* calculate even parity bit */
  for (z = 1; z<8; z++)
    p0 = p0 ^ !!(out & (1 << z));

  out |= p0;

  return(out);
}


/**
  * @brief  hamming encode a byte
  * @param  input - byte to be encoded
  * @retval 16-bit encoded message
  */
uint16_t s4433912_lib_hamming_byte_encode(uint8_t input) {
  uint16_t out;

  /* first encode D0..D3 (first 4 bits),
    * then D4..D7 (second 4 bits).
    */
  out = hamming_hbyte_encode(input & 0xF) |
    (hamming_hbyte_encode(input >> 4) << 8);

  return(out);

}

/**
  * @brief  decode half byte from a byte, errors are detected and corrected
  * @param  in - encoded half byte
  * @retval decoded half byte
  */
uint8_t s4433912_lib_hamming_byte_decode(uint8_t in) {
  uint8_t d0, d1, d2, d3;
  uint8_t p0 = 0, h0, h1, h2;
  uint8_t syndrome, s0, s1, s2;
  uint8_t out, parity;

  //in consists of 8 bits

  p0 = !!((in & 0x01));

  //parity bits

  h0 = (in >> 1) & 0x1;
  h1 = (in >> 2) & 0x1;
  h2 = (in >> 3) & 0x01;

  //data bits
  d3 = !!(in & 0x80);
  d2 = !!(in & 0x40);
  d1 = !!(in & 0x20);
  d0 = !!(in & 0x10);

  //sydrome calculation
  s0 = h0 ^ d1 ^ d2 ^ d3;
  s1 = h1 ^ d0 ^ d2 ^ d3;
  s2 = h2 ^ d0 ^ d1 ^ d3;

  syndrome = (s0 << 0) + (s1 << 1) + (s2 << 2);

  parity = h0 ^ h1 ^ h2 ^ d0 ^ d1 ^ d2 ^ d3; 

  error = 0;
  if (syndrome != 0) {
    error = 1;
    if (parity == p0) {
	    debug_printf("2 bit error \r\n");

    } else {   //correct error bit
      in = in ^ (1 << (syndrome));//flip bit at syndrome position
    }

  }

  //double-check data bits
  d3 = !!(in & 0x80);
  d2 = !!(in & 0x40);
  d1 = !!(in & 0x20);
  d0 = !!(in & 0x10);

  out = (d3 << 3) | (d2 << 2) | (d1 << 1) | (d0);
  return out;
    
}


/**
  * @brief  report even parity error
  * @param  None
  * @retval 1 - error, 0 - no error
  */
uint8_t s4433912_lib_hamming_parity_error(uint8_t byte) {
    return error;
}