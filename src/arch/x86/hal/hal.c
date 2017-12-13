/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/

#ifndef __i386__
#error "[hal.c for i386] requires i386 architecture. Define __i386__"
#endif

/*    IMPLEMENTATION HEADERS												  */

#include <hal.h>
#include "../asm.h"
#include "cpu.h"

//	Initializing HAL
uint32_t hal_init(void) {
	cpu_init();
	return 0;
}

//	Shutting down HAL
uint32_t hal_fini(void) {
	cpu_fini();
	return 0;
}

//	generates interrupt
void geninterrupt(int n) {
	asm("mov al, byte %0\n"
	    "mov byte [1F+1], al\n"
	    "1:"
	    "int 0" ::"r"(n));
}
