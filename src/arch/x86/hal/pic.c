/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
#include "pic.h"
#include <hal.h>
#include "../asm.h"

#define MPIC 0x20 /* IO base address for master PIC */
#define SPIC 0xA0 /* IO base address for slave PIC */
#define MPIC_CMD MPIC
#define MPIC_DTA (MPIC + 1)
#define SPIC_CMD SPIC
#define SPIC_DTA (SPIC + 1)
#define PIC_EOI 0x20 /* End-of-interrupt command code */

#define I86_PIC_ICW4_MASK_UPM 0x1       //00000001
#define I386_PIC_ICW1_ICW4_EXPECT 0x01  //1
#define I386_PIC_ICW1_INIT_YES 0x10     //10000
#define I386_PIC_ICW1_INIT_NO 0x00      //00000
#define I86_PIC_ICW4_UPM_86MODE 0x01    //1

void pic_init(uint8_t base0, uint8_t base1) {
	uint8_t icw = 0;

	// disable all hardware interrupt before begin
	disable_intr();

	//	Initialize and remap PIC
	icw = (icw & ~I386_PIC_ICW1_INIT) | I386_PIC_ICW1_INIT;
	icw = (icw & ~I386_PIC_ICW1_ICW4) | I386_PIC_ICW1_ICW4_EXPECT;

	pic_send_command(icw, 0);
	pic_send_command(icw, 1);

	//	Send initialization control word 2. This is the base addresses of the irq's
	pic_send_byte(base0, 0);
	pic_send_byte(base1, 1);

	//	Send initialization control word 3. This is the connection between master and slave.
	//	ICW3 for master PIC is the IR that connects to secondary pic in binary format
	//	ICW3 for secondary PIC is the IR that connects to master pic in decimal format
	pic_send_byte(0x04, 0);
	pic_send_byte(0x02, 1);

	//	Send Initialization control word 4. Enables i86 mode
	icw = (icw & ~I86_PIC_ICW4_MASK_UPM) | I86_PIC_ICW4_UPM_86MODE;

	pic_send_byte(icw, 0);
	pic_send_byte(icw, 1);
}

__inline__ uint8_t pic_read_byte(uint8_t pic_num) {
	if (pic_num > 1) {
		return 0;
	}

	uint8_t reg = (pic_num == 1) ? SPIC_DTA : MPIC_DTA;
	return _inb(reg);
}

__inline__ void pic_send_byte(uint8_t data, uint8_t pic_num) {
	if (pic_num > 1) {
		return;
	}

	uint8_t reg = (pic_num == 1) ? SPIC_DTA : MPIC_DTA;
	_outb(reg, data);
}

__inline__ void pic_send_command(uint8_t cmd, uint8_t pic_num) {
	if (pic_num > 1) {
		return;
	}

	uint8_t reg = (pic_num == 1) ? SPIC_CMD : MPIC_CMD;
	_outb(reg, cmd);
}

// void disable_pic(uint8_t is_slave) {
// 	uint16_t port = is_slave * 0x80 + 0x21;
// 	_outb(port, 0xFF);
// 	// _asm("mov ax, 0xFF"
// 	//     "out %0, ax" ::"r"(port));
// }

// void irq_set_line(uint8_t irq_line) {
// 	uint16_t port;
// 	uint8_t value;

// 	if (irq_line < 8) {
// 		port = MPIC_DTA;
// 	} else {
// 		port = SPIC_DTA;
// 		irq_line -= 8;
// 	}

// 	value = _inb(port) | (1 << irq_line);
// 	_outb(port, value);
// }

// void irq_clear_mask(uint8_t irq_line) {
// 	uint16_t port;
// 	uint8_t value;

// 	if (irq_line < 8) {
// 		port = MPIC_DTA;
// 	} else {
// 		port = SPIC_DTA;
// 		irq_line -= 8;
// 	}

// 	value = _inb(port) & ~(1 << irq_line);
// 	_outb(port, value);
// }

// /*
// arguments:
// 	offset1 - vector offset for master PIC
// 			  vectors on the master become offset1..offset1+7
// 	offset2 - same for slave PIC: offset2..offset2+7
// */
// void pic_remap(uint32_t offset1, uint32_t offset2) {
// 	uint8_t a1, a2;

// 	a1 = _inb(MPIC_DTA);  // save masks
// 	a2 = _inb(SPIC_CMD);

// 	_outb(MPIC_CMD, ICW1_INIT + ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
// 	io_wait();
// 	_outb(SPIC_CMD, ICW1_INIT + ICW1_ICW4);
// 	io_wait();
// 	_outb(MPIC_DTA, offset1);  // ICW2: Master PIC vector offset
// 	io_wait();
// 	_outb(SPIC_CMD, offset2);  // ICW2: Slave PIC vector offset
// 	io_wait();
// 	_outb(MPIC_DTA, 4);  // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
// 	io_wait();
// 	_outb(SPIC_CMD, 2);  // ICW3: tell Slave PIC its cascade identity (0000 0010)
// 	io_wait();

// 	_outb(MPIC_DTA, ICW4_8086);
// 	io_wait();
// 	_outb(SPIC_CMD, ICW4_8086);
// 	io_wait();

// 	_outb(MPIC_DTA, a1);  // restore saved masks.
// 	_outb(SPIC_CMD, a2);
// }

// void pic_acknowledge(uint8_t irq) {
// 	if (irq > 8) {
// 		_outb(SPIC_CMD, PIC_EOI);
// 	}

// 	_outb(MPIC_CMD, PIC_EOI);
// }

// GLOBAL _geninterrupt
// 		align	8
// _geninterrupt:
// 		enter	0, 0

// 		mov		eax, [ebp+8]
// 		mov		byte [.intr+1], al
// .intr:
// 		int		0
// .leave:	leave
// 		ret
// .end:
