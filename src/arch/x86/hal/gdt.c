/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
#include "gdt.h"
#include <string.h>
#include "../asm.h"
#include "../func.h"

//	Defines a GDT entry. Use packed, because it prevents the compiler from doing things that it thinks
//	is best: Prevent compiler "optimization" by packing
//	Special pointer which includes the limit: The max bytes taken up by the GDT, minus 1. Again, this
//	NEEDS to be packed
typedef struct {
	uint16_t limit;
	uint32_t base_ptr;
} __packed gdtr_t;

static gdt_desc_t __align(8) _gdt_entry[MAX_DESCRIPTORS];
static gdtr_t __align(8) _gdt_ptr;
static tss_entry_t __align(128) _tss_entry;

uint16_t NULL_SELECTOR = 0x0;
uint16_t KCODE_SELECTOR = sizeof(gdt_desc_t) * KCODE_INDEX;
uint16_t KDATA_SELECTOR = sizeof(gdt_desc_t) * KDATA_INDEX;
uint16_t UCODE_SELECTOR = sizeof(gdt_desc_t) * UCODE_INDEX;
uint16_t UDATA_SELECTOR = sizeof(gdt_desc_t) * UDATA_INDEX;
uint16_t TSS_SELECTOR = sizeof(gdt_desc_t) * TSS_INDEX;

void init_tss(void *kernel_stack_bottom, size_t stack_size) {
	uint32_t tss_base = (uint32_t)&_tss_entry;
	uint32_t tss_limit = sizeof(_tss_entry);
	_gdt_entry[TSS_INDEX].limit = tss_limit & 0xFFFF;
	_gdt_entry[TSS_INDEX].base_lo = tss_base & 0xFFFF;
	_gdt_entry[TSS_INDEX].base_mid = (tss_base & 0xFF0000) >> 16;
	_gdt_entry[TSS_INDEX].flags = 0b11101001;
	_gdt_entry[TSS_INDEX].granu = tss_limit & 0xF0000;
	_gdt_entry[TSS_INDEX].base_hi = (tss_base & 0xFF000000) >> 24;

	_tss_entry.ss0 = KDATA_SELECTOR;
	_tss_entry.esp0 = ((uint32_t)&kernel_stack_bottom + stack_size) - 4;

	_tss_flush();
}

void gdt_set_descriptor(uint32_t i, uint64_t base, uint64_t limit, uint8_t access, uint8_t granularity) {
	if (i > MAX_DESCRIPTORS) {
		return;
	}

	//	Initialize descriptor
	memset((void *)&_gdt_entry[i], 0, sizeof(gdt_desc_t));

	//	Set limit and base address
	_gdt_entry[i].base_lo = base & 0xFFFF;
	_gdt_entry[i].base_mid = (base >> 16) & 0xFF;
	_gdt_entry[i].base_hi = (base >> 24) & 0xFF;
	_gdt_entry[i].limit = limit & 0xFFFF;

	//	Set flags and granularity
	_gdt_entry[i].flags = access;
	_gdt_entry[i].granu = (limit >> 16) & 0x0F;
	_gdt_entry[i].granu |= granularity & 0xF0;
}

gdt_desc_t *gdt_get_descriptor(uint32_t i) {
	if (i > MAX_DESCRIPTORS) {
		return 0;
	}

	return &_gdt_entry[i];
}

void gdt_init(void) {
	//	Setup the GDT pointer and limit, always (sizeof(GDTENTRY) * entry count) - 1
	_gdt_ptr.limit = (sizeof(gdt_desc_t) * MAX_DESCRIPTORS) - 1;
	_gdt_ptr.base_ptr = (uint32_t)&_gdt_entry[0];

	//	NULL Descriptor, not use, but has to be here.
	gdt_set_descriptor(0, 0, 0, 0, 0);

	//	0x08 - The second entry is our Code Segment. The base address
	//	is 0, the limit is 4GBytes, it uses 4KByte granularity,
	//	uses 32-bit opcodes, and is a Code Segment descriptor.
	//	Please check the table above in the tutorial in order
	//	to see exactly what each value means
	gdt_set_descriptor(KCODE_INDEX, 0, 0xFFFFFFFF,
	                   I386_GDT_DESC_READWRITE | I386_GDT_DESC_EXEC_CODE | I386_GDT_DESC_CODEDATA | I386_GDT_DESC_MEMORY,
	                   I386_GDT_GRAND_4K | I386_GDT_GRAND_32BIT | I386_GDT_GRAND_LIMITHI_MASK);

	//	0x10 - The third entry is our Data Segment. It's EXACTLY the
	//	same as our code segment, but the descriptor type in
	//	this entry's access byte says it's a Data Segment
	gdt_set_descriptor(KDATA_INDEX, 0, 0xFFFFFFFF,
	                   I386_GDT_DESC_READWRITE | I386_GDT_DESC_CODEDATA | I386_GDT_DESC_MEMORY,
	                   I386_GDT_GRAND_4K | I386_GDT_GRAND_32BIT | I386_GDT_GRAND_LIMITHI_MASK);

	//	0x18 - User mode code
	gdt_set_descriptor(UCODE_INDEX, 0, 0xFFFFFFFF,
	                   I386_GDT_DESC_READWRITE | I386_GDT_DESC_EXEC_CODE | I386_GDT_DESC_CODEDATA | I386_GDT_DESC_DPL | I386_GDT_DESC_MEMORY,
	                   I386_GDT_GRAND_4K | I386_GDT_GRAND_32BIT | I386_GDT_GRAND_LIMITHI_MASK);

	//	0x20 - User mode data
	gdt_set_descriptor(UDATA_INDEX, 0, 0xFFFFFFFF,
	                   I386_GDT_DESC_READWRITE | I386_GDT_DESC_CODEDATA | I386_GDT_DESC_DPL | I386_GDT_DESC_MEMORY,
	                   I386_GDT_GRAND_4K | I386_GDT_GRAND_32BIT | I386_GDT_GRAND_LIMITHI_MASK);

	/* Flush out the old GDT and install the new changes! */
	_gdt_flush(&_gdt_ptr, 0x10);
}