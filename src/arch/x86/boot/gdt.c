#include "gdt.h"
#include "../asm.h"

extern void _gdt_flush(void *, uint32_t d);  // definition in func.asm
extern void _tss_flush(void);                // definition in func.asm

const uint16_t NULL_SELECTOR = 0x0;
const uint16_t KCODE_SELECTOR = sizeof(gdt_entry_t) * KCODE_INDEX;
const uint16_t KDATA_SELECTOR = sizeof(gdt_entry_t) * KDATA_INDEX;
const uint16_t UCODE_SELECTOR = sizeof(gdt_entry_t) * UCODE_INDEX;
const uint16_t UDATA_SELECTOR = sizeof(gdt_entry_t) * UDATA_INDEX;
const uint16_t TSS_SELECTOR = sizeof(gdt_entry_t) * TSS_INDEX;

void init_tss(void *kernel_stack_bottom, size_t stack_size) {
	uint32_t tss_base = (uint32_t)&tss_entry;
	uint32_t tss_limit = sizeof(tss_entry);
	gdt_entry[TSS_INDEX].limit_low = tss_limit & 0xFFFF;
	gdt_entry[TSS_INDEX].base_low = tss_base & 0xFFFF;
	gdt_entry[TSS_INDEX].base_middle = (tss_base & 0xFF0000) >> 16;
	gdt_entry[TSS_INDEX].access_byte = 0b11101001;
	gdt_entry[TSS_INDEX].flags_n_limits = tss_limit & 0xF0000;
	gdt_entry[TSS_INDEX].base_high = (tss_base & 0xFF000000) >> 24;

	tss_entry.ss0 = KDATA_SELECTOR;
	tss_entry.esp0 = ((uint32_t)&kernel_stack_bottom + stack_size) - 4;

	_tss_flush();
}

void early_init_gdt(void) {
	//	Setup the GDT pointer and limit, always (sizeof(GDTENTRY) * entry count) - 1
	gdt_ptr.limit = sizeof(gdt_entry) - 1;
	gdt_ptr.base_ptr = (uint32_t)gdt_entry;

	//	NULL Descriptor, not use, but has to be here.
	gdt_entry[NULL_DESCRIPTOR].limit_low = 0;
	gdt_entry[NULL_DESCRIPTOR].base_low = 0;
	gdt_entry[NULL_DESCRIPTOR].base_middle = 0;
	gdt_entry[NULL_DESCRIPTOR].access_byte = 0;
	gdt_entry[NULL_DESCRIPTOR].flags_n_limits = 0;
	gdt_entry[NULL_DESCRIPTOR].base_high = 0;

	//	0x08 - The second entry is our Code Segment. The base address
	//	is 0, the limit is 4GBytes, it uses 4KByte granularity,
	//	uses 32-bit opcodes, and is a Code Segment descriptor.
	//	Please check the table above in the tutorial in order
	//	to see exactly what each value means
	gdt_entry[KCODE_INDEX].limit_low = 0xFFFF;
	gdt_entry[KCODE_INDEX].base_low = 0x0000;
	gdt_entry[KCODE_INDEX].base_middle = 0x00;
	gdt_entry[KCODE_INDEX].access_byte = 0b10011010;
	gdt_entry[KCODE_INDEX].flags_n_limits = 0xCF;
	gdt_entry[KCODE_INDEX].base_high = 0x00;

	//	0x10 - The third entry is our Data Segment. It's EXACTLY the
	//	same as our code segment, but the descriptor type in
	//	this entry's access byte says it's a Data Segment
	gdt_entry[KDATA_INDEX].limit_low = 0xFFFF;
	gdt_entry[KDATA_INDEX].base_low = 0x0000;
	gdt_entry[KDATA_INDEX].base_middle = 0x00;
	gdt_entry[KDATA_INDEX].access_byte = 0b10010010;
	gdt_entry[KDATA_INDEX].flags_n_limits = 0xCF;
	gdt_entry[KDATA_INDEX].base_high = 0x00;

	//	0x18 - User mode code
	gdt_entry[UCODE_INDEX].limit_low = 0xFFFF;
	gdt_entry[UCODE_INDEX].base_low = 0x0000;
	gdt_entry[UCODE_INDEX].base_middle = 0x00;
	gdt_entry[UCODE_INDEX].access_byte = 0b11111010;
	gdt_entry[UCODE_INDEX].flags_n_limits = 0xCF;
	gdt_entry[UCODE_INDEX].base_high = 0x00;

	//	0x20 - User mode data
	gdt_entry[UDATA_INDEX].limit_low = 0xFFFF;
	gdt_entry[UDATA_INDEX].base_low = 0x0000;
	gdt_entry[UDATA_INDEX].base_middle = 0x00;
	gdt_entry[UDATA_INDEX].access_byte = 0b11110010;
	gdt_entry[UDATA_INDEX].flags_n_limits = 0xCF;
	gdt_entry[UDATA_INDEX].base_high = 0x00;

	/* Flush out the old GDT and install the new changes! */
	_gdt_flush(&gdt_ptr, 0x10);
}