/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
#include "pic.h"
#include "asm.h"

void init_pic() {
	//	Initialize and remap PIC
	pic_remap(0x20, 0x28);  //	Master to 0x20, Slave to 0x28

	//_outb(MPIC_DTA, 0b11111101);  // Only enable keyboard (irc 1)
	irq_clear_mask(1);
	_outb(SPIC_DTA, 0b11111111);  // Don't enable any interrupts on slave pic (irc 8-15)
	sti();
}

void disable_pic(uint8_t is_slave) {
	uint16_t port = is_slave * 0x80 + 0x21;
	_outb(port, 0xFF);
	// asm("mov ax, 0xFF"
	//     "out %0, ax" ::"r"(port));
}

void irq_set_line(uint8_t irq_line) {
	uint16_t port;
	uint8_t value;

	if (irq_line < 8) {
		port = MPIC_DTA;
	} else {
		port = SPIC_DTA;
		irq_line -= 8;
	}

	value = _inb(port) | (1 << irq_line);
	_outb(port, value);
}

void irq_clear_mask(uint8_t irq_line) {
	uint16_t port;
	uint8_t value;

	if (irq_line < 8) {
		port = MPIC_DTA;
	} else {
		port = SPIC_DTA;
		irq_line -= 8;
	}

	value = _inb(port) & ~(1 << irq_line);
	_outb(port, value);
}

/*
arguments:
	offset1 - vector offset for master PIC
			  vectors on the master become offset1..offset1+7
	offset2 - same for slave PIC: offset2..offset2+7
*/
void pic_remap(uint32_t offset1, uint32_t offset2) {
	uint8_t a1, a2;

	a1 = _inb(MPIC_DTA);  // save masks
	a2 = _inb(SPIC_CMD);

	_outb(MPIC_CMD, ICW1_INIT + ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	io_wait();
	_outb(SPIC_CMD, ICW1_INIT + ICW1_ICW4);
	io_wait();
	_outb(MPIC_DTA, offset1);  // ICW2: Master PIC vector offset
	io_wait();
	_outb(SPIC_CMD, offset2);  // ICW2: Slave PIC vector offset
	io_wait();
	_outb(MPIC_DTA, 4);  // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	io_wait();
	_outb(SPIC_CMD, 2);  // ICW3: tell Slave PIC its cascade identity (0000 0010)
	io_wait();

	_outb(MPIC_DTA, ICW4_8086);
	io_wait();
	_outb(SPIC_CMD, ICW4_8086);
	io_wait();

	_outb(MPIC_DTA, a1);  // restore saved masks.
	_outb(SPIC_CMD, a2);
}

void pic_acknowledge(uint8_t irq) {
	if (irq > 8) {
		_outb(SPIC_CMD, PIC_EOI);
	}

	_outb(MPIC_CMD, PIC_EOI);
}