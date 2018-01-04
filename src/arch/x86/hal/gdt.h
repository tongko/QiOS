/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
#ifndef __GDT_H_
#define __GDT_H_

/*		Interface Required Haders		*/
#include <attribs.h>
#include <stddef.h>
#include <stdint.h>

/*		Definitions, Enumerations, Type Definition	*/

//	Maximum descriptors allowed
#define MAX_DESCRIPTORS 5

//	Set access bit
#define I386_GDT_DESC_ACCESS 0x0001

//	Descriptor is readable and writable.
#define I386_GDT_DESC_READWRITE 0x0002

//	Set expansion direction bit
#define I386_GDT_DESC_EXPANSION 0x0004

//	Executable code segment
#define I386_GDT_DESC_EXEC_CODE 0x0008

//	Set code or data descriptor
#define I386_GDT_DESC_CODEDATA 0x0010

//	Set dpl bit
#define I386_GDT_DESC_DPL 0x0060

//	Set in memory bit
#define I386_GDT_DESC_MEMORY 0x0080

//	Masks out limit_hi (Hi 4 Bits)
#define I386_GDT_GRAND_LIMITHI_MASK 0X0F

//	Set OS defined bit
#define I386_GDT_GRAND_OS 0x10

//	Set if 32 bits.
#define I386_GDT_GRAND_32BIT 0x40

//	4K Granularity.
#define I386_GDT_GRAND_4K 0X80

typedef struct {
	uint16_t limit;    //	Bits 00-15 (Bits 0-15 of 20 bits (Segment limits))
	uint16_t base_lo;  //	Bits 16-31 (Bits 0-15 of 32 bits (base address))
	uint8_t base_mid;  //	Bits 32-39 (Bits 16-23 of 32 bits (base address))
	uint8_t flags;     //	Bits 40-47 (Descriptor access flags)
	uint8_t granu;     //	Bits 48-55
	uint8_t base_hi;   //	Bits 56-63 (Bits 24-31 of 32 bits)
} __packed gdt_desc_t;

// A struct describing a Task State Segment.
typedef struct {
	uint32_t prev_tss;  // The previous TSS - if we used hardware task switching this would form a linked list.
	uint32_t esp0;      // The stack pointer to load when we change to kernel mode.
	uint32_t ss0;       // The stack segment to load when we change to kernel mode.
	uint32_t esp1;      // everything below here is unusued now..
	uint32_t ss1;
	uint32_t esp2;
	uint32_t ss2;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t es;
	uint32_t cs;
	uint32_t ss;
	uint32_t ds;
	uint32_t fs;
	uint32_t gs;
	uint32_t ldt;
	uint16_t trap;
	uint16_t iomap_base;
} __packed tss_entry_t;

extern uint16_t NULL_SELECTOR;
extern uint16_t KCODE_SELECTOR;
extern uint16_t KDATA_SELECTOR;
extern uint16_t UCODE_SELECTOR;
extern uint16_t UDATA_SELECTOR;
extern uint16_t TSS_SELECTOR;

enum segment_selector_t {
	NULL_DESCRIPTOR,  // Not used but has to be here
	KCODE_INDEX,      // Offset 0x8
	KDATA_INDEX,      // Offset 0x10
	UCODE_INDEX,      // Offset 0x18
	UDATA_INDEX,      // Offset 0x20
	TSS_INDEX         // Offset 0x28
};

/*		Interface Function Prototypes	*/

//	Initialize GDT.
extern void gdt_init(void);

//	Retrieve descriptor
extern gdt_desc_t *gdt_get_descriptor(uint32_t i);

extern void gdt_set_descriptor(uint32_t i, uint64_t base, uint64_t limit, uint8_t access, uint8_t granularity);

//	Initialize TSS.
extern void init_tss(void *kernel_stack_bottom, size_t stack_size);

#endif  //	__GDT_H_