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
#include "idt.h"
#include "pic.h"
#include "pit.h"

//	Initializing HAL
uint32_t hal_init(void) {
	cpu_init();
	pic_init(0x20, 0x28);
	pit_init();
	return 0;
}

//	Shutting down HAL
uint32_t hal_fini(void) {
	cpu_fini();
	return 0;
}

void raise_intr(uint8_t n) {
	__asm__ __volatile__(
	    "mov byte [intpos + 1], %0\n"
	    "intpos:\n"
	    "int 0"
	    :
	    : "r"(n)
	    : "cc", "memory");
}

__inline__ void intr_done(uint32_t intr_num) {
	//	Assure that num is valid hardware interrupt req
	if (intr_num > 16) {
		return;
	}

	//	Test if we need to send end-of-interrupt to slave PIT
	if (intr_num > 7) {
		pic_send_command(i386_PIC_ICW2_MASK_EOI, 1);
	}

	//	Always send end-of-interrupt to master PIT
	pic_send_command(i386_PIC_ICW2_MASK_EOI, 0);
}

//	Output sound to speaker
void sound(uint32_t frequency) {
	outportb(0x01, 3 | (uint8_t)(frequency << 2));
}

uint8_t inportb(uint16_t port) {
	return _inb(port);
}

void outportb(uint16_t port, uint8_t value) {
	_outb(port, value);
}

void enable_intr() {
	sti();
}

void disable_intr() {
	cli();
}

void intr_set_vect(uint8_t intr_num, void *vect) {
	//	Set interrupt handler, this overwrites previous interrupt descriptor
	idt_set_gate(intr_num, I386_IDT_DESC_PRESENT | I386_IDT_DESC_BIT32,
	             0x08, (I386_IRQ_HANDLER)vect);
}

void *intr_get_vect(uint8_t intr_num) {
	idt_desc_t *desc = idt_get_intr_desc(intr_num);
	if (!desc) {
		return 0;
	}

	//	Get address of interript handler
	uint32_t addr = desc->offset_lo | (desc->offset_hi << 16);

	//	Return interrupt handler
	return (I386_IRQ_HANDLER)addr;
}

const char *get_cpu_vender() {
	return cpu_get_vendor();
}

uint32_t get_tick_count() {
	return pit_get_tick_count();
}