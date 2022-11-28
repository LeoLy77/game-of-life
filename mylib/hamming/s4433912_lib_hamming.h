/**
 ***************************************************************
 * @file mylib/keypad/s4433912_hal_hamming.h
 * @author Leo Ly - 44339124
 * @date 07/04/2020
 * @brief hamming driver
 * REFERENCE: csse3010_mylib_hal_hamming.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 * s4433912_lib_hamming_byte_encode() -
 * s4433912_lib_hamming_byte_decode() -
 * hamming_hbyte_encode() -
 * s4433912_lib_hamming_parity_error(uint8_t byte) - 
 ***************************************************************
*/
#include "board.h"
#include "processor_hal.h"
#include <stdint.h>

#ifndef S4433912_LIB_HAMMING_H
#define S4433912_LIB_HAMMING_H
// uint8_t hamming_hbyte_encode(uint8_t);
uint16_t s4433912_lib_hamming_byte_encode(uint8_t);
uint8_t s4433912_lib_hamming_byte_decode(uint8_t);
uint8_t s4433912_lib_hamming_parity_error(uint8_t byte);



#endif