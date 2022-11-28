/**
 ***************************************************************
 * @file mylib/lta1000g/s4433912_hal_lta1000g.h
 * @author Leo Ly - 44339124
 * @date 29/02/2020
 * @brief LED Light Bar peripheral driver
 * REFERENCE: csse3010_mylib_hal_lta1000g.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 * s4433912_hal_lta1000g_init () - intialise LED bar
 * s4433912_hal_lta1000g_write () - set LED bar value
 ***************************************************************
*/

#ifndef S4433912_HAL_LTA1000G_H
#define S4433912_HAL_LTA1000G_H


extern void s4433912_hal_lta1000g_init(void);
extern void s4433912_hal_lta1000g_write(unsigned short);

#endif
