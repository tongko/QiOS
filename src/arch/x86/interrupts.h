/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
#ifndef __INTERRUPTS_H_
#define __INTERRUPTS_H_

#include <attribs.h>
#include <stdint.h>

#define INT_KEYBOARD 0x00000009
#define INT_GENERAL_PROTECTION_FAULT 0x0000000D
#define INT_PAGE_FAULT 0x0000000E
#define INT_SOFTWARE 0x00000031
#define INT_OUT_OF_MEMORY 0x00000032
#define INT_SYSCALL 0x00000080

typedef struct tagCPU_STATE {
	uint32_t cr2;
	uint32_t ebp;
	uint32_t edi;
	uint32_t esi;
	uint32_t edx;
	uint32_t ecx;
	uint32_t ebx;
	uint32_t eax;
} __packed cpu_state_t;

void enable_keyboard_interrupts();
uint32_t interrupt_handler(cpu_state_t cpu, uint32_t interrupt_number, uint32_t error_code, uint32_t eip);

#endif  //	__INTERRUPTS_H_