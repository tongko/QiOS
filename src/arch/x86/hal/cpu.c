/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
#include <cpu.h>
#include "gdt.h"
#include "idt.h"

uint32_t cpu_init() {
	gdt_init();
	idt_init();

	return 0;
}

void cpu_fini() {
}