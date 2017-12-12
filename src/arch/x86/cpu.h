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

#include <attribs.h>

typedef struct {
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t cr2;
} __packed cpu_state_t;

typedef struct {
	uint32_t error_code;
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
} __packed stack_state_t;

#endif  //	__CPU_H_