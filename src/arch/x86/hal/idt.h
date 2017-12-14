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

/*	Definition, type definition	*/

#define I386_MAX_INTERRUPT 256  // 0-255

#define I386_IDT_DESC_BIT16 0x06    //	00000110
#define I386_IDT_DESC_BIT32 0x0E    //	00001110
#define I386_IDT_DESC_RING1 0x40    //	01000000
#define I386_IDT_DESC_RING2 0x20    //	00100000
#define I386_IDT_DESC_RING3 0x60    //	01100000
#define I386_IDT_DESC_PRESENT 0x80  //	10000000

// interrupt handler w/o error code
// Note: interrupt handlers are called by the processor. The stack setup may change
// so we leave it up to the interrupts' implimentation to handle it and properly return
typedef void (*I386_IRQ_HANDLER)(void);

// a pointer to the interrupt descriptor table
// passed by reference to the LIDT instruction
typedef struct {
	uint16_t limit;  // in bytes
	uint32_t base;
} __packed idt_ptr_t;

// See http://wiki.osdev.org/Interrupt_Descriptor_Table#Structure_IA-32
typedef struct {
	//	bits 15:0 of interrupt routing (ir) address
	uint16_t offset_lo;
	//	code selector in gdt
	uint16_t selector;
	//	reserved, must be zero
	uint8_t reserved;
	//	Attributes
	uint8_t flags;
	//	bits 16:31 of ir address
	uint16_t offset_hi;
} __packed idt_desc_t;

//	Initialize basic idt
extern void idt_init(void);

//	Gets interrupt descriptor
extern idt_desc_t *idt_get_intr_desc(uint32_t index);

//
extern void idt_set_gate(uint32_t idx, uint8_t flags, uint16_t selector, I386_IRQ_HANDLER irq_handler);

#endif  // __IDT_H_