/**
 ***************************************************************
 * @file mylib/pb/ s4433912_hal_pb.h
 * @author Leo Ly - 44339124
 * @date 29/02/2020
 * @brief LED Light Bar peripheral driver
 * REFERENCE: DON’T JUST COPY THIS BLINDLY.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************
 * s4433912_hal_pb_on_init(void)
 * s4433912_hal_pb_on_isr(uint16_t*, uint32_t*)
 * s4433912_hal_pb_iss_isr(uint16_t*, uint32_t*)
 * s4433912_hal_pb_isr_init(void)
 * s4433912_hal_pb_isr_deinit(void)
 ***************************************************************
*/

#ifndef S4433912_HAL_PB_H
#define S4433912_HAL_PB_H

extern void s4433912_hal_pb_on_init(void);
extern void s4433912_hal_pb_on_isr(uint16_t*, uint32_t*);
extern void s4433912_hal_pb_iss_isr(uint16_t*, uint32_t*);
extern void s4433912_hal_pb_isr_init(void);
extern void s4433912_hal_pb_isr_deinit(void);

#endif
