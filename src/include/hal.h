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

//	generates interrupt
extern void geninterrupt(int n);

#endif  //	__HAL_H_