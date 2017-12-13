/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
#ifndef __CPU_H_
#define __CPU_H_

#include <stdint.h>

//	Initialize the processor
extern uint32_t cpu_init(void);

//	Shutdown processor
extern void cpu_fini(void);

#endif  //	__CPU_H_