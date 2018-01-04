/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
#include <asm.h>
#include <cpu.h>
#include "gdt.h"
#include "idt.h"

//	Initialize CPU resources
uint32_t cpu_init() {
	gdt_init();
	idt_init(0x8);

	return 0;
}

//	Nothing to do at the moment
void cpu_fini() {
}

static __inline__ void native_cpuid(uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
	/* ecx is often an input as well as an output. */
	_asm("cpuid\n"
	     : "=a"(*eax),
	       "=b"(*ebx),
	       "=c"(*ecx),
	       "=d"(*edx)
	     : "0"(*eax), "2"(*ecx));
}

//	Returns CPU's vender name
const char *cpu_get_vendor() {
	static char vender[32] = {0};
	uint32_t eax, ebx, ecx, edx;
	eax = 0;
	native_cpuid(&eax, &ebx, &ecx, &edx);

	uint32_t *char_as_uint = (uint32_t *)vender;
	*(char_as_uint++) = ebx;
	*(char_as_uint++) = edx;
	*(char_as_uint++) = ecx;

	return vender;
}