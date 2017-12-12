/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
#ifndef __IDT_H_
#define __IDT_H_

#include <attribs.h>
#include <stdint.h>

// a pointer to the interrupt descriptor table
// passed by reference to the LIDT instruction
typedef struct {
	uint16_t limit;  // in bytes
	uint32_t base;
} __packed idt_desc_t;

// See http://wiki.osdev.org/Interrupt_Descriptor_Table#Structure_IA-32
typedef struct {
	union {
		struct {
			uint16_t reserved0;
			uint16_t selector;
			uint8_t reserved1;
			uint8_t type_attr;
			uint16_t reserved2;
		} __packed task_gate;
		struct {
			uint16_t offset_low;
			uint16_t selector;
			uint8_t zero;
			uint8_t type_attr;
			uint16_t offset_high;
		} __packed intr_or_trap;
	};
} idt_desc_tab_t;

void init_idt();
void pic_acknowledge();
void init_pic();

#endif  // __IDT_H_