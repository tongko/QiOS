/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
/*******************************************************************************
 *  Hardware Abstraction Layer                                                 *
 *                                                                             *
 *	The Hardware Abstraction Layer (HAL) provides an abstract interface		   *
 *	to control the basic motherboard hardware devices. This is accomplished    *
 *	by abstracting hardware dependencies behind this interface.                *
 *																			   *
 *	All routines and types are declared extern and must be defined within      *
 *	external libraries to define specific hal implimentations.                 *
 * ****************************************************************************/
#ifndef __HAL_H_
#define __HAL_H_

#ifndef __i386__
#error "HAL not implimented for this platform"
#endif

/*    INTERFACE REQUIRED HEADERS	*/

#include <stdint.h>

/*    INTERFACE DATA DECLARATIONS											  */

/*    INTERFACE FUNCTION PROTOTYPES											  */

//	Initializing HAL
extern uint32_t hal_init(void);

//	Shutting down HAL
extern uint32_t hal_fini(void);

//	Raise interrupt
extern void raise_intr(uint8_t intr_num);

//	Notify hal interrupt is done
extern void intr_done(uint32_t intr_num);

//	Output sound to speaker
extern void sound(uint32_t frequency);

//	Read byte from device uint mapped IO
extern uint8_t inportb(uint16_t port);

//	Write byte to device through mapped IO
extern void outportb(uint16_t port, uint8_t value);

//	Enables all hardware interrupts
extern void enable_intr(void);

//	Disable all hardware interrupts
extern void disable_intr(void);

//	Sets new interrupt vector
extern void intr_set_vect(uint8_t intr_num, void *vect);

//	Returns current interrupt vector
extern void *intr_get_vect(uint8_t intr_num);

//	Returns CPU Vendor (from cpu string)
extern const char *get_cpu_vender(void);

//	Return current tick count (experimental)
extern uint32_t get_tick_count(void);

#endif  //	__HAL_H_