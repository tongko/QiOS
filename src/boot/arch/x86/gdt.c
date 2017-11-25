#include "gdt.h"
#include "multiboot.h"

extern void _gdt_flush(void *);
extern void _tss_flush(void);

const uint16_t NULL_SELECTOR = 0x0;
const uint16_t KCODE_SELECTOR = sizeof(GDT_ENTRY) * KCODE_INDEX;
const uint16_t KDATA_SELECTOR = sizeof(GDT_ENTRY) * KDATA_INDEX;
const uint16_t UCODE_SELECTOR = sizeof(GDT_ENTRY) * UCODE_INDEX;
const uint16_t UDATA_SELECTOR = sizeof(GDT_ENTRY) * UDATA_INDEX;
const uint16_t TSS_SELECTOR = sizeof(GDT_ENTRY) * TSS_INDEX;

TSS_ENTRY tss_entry;
GDT_ENTRY gdt_entry[6];

void init_tss() {
  uint32_t tss_base = (uint32_t) &tss_entry;
  uint32_t tss_limit = sizeof(tss_entry);
  gdt_entry[TSS_INDEX].LimitLow = tss_limit & 0xFFFF;
  gdt_entry[TSS_INDEX].BaseLow = tss_base & 0xFFFF;
  gdt_entry[TSS_INDEX].BaseMiddle = (tss_base & 0xFF0000) >> 16;
  gdt_entry[TSS_INDEX].AccessByte = 0b11101001;
  gdt_entry[TSS_INDEX].FlagsAndLimit = tss_limit & 0xF0000;
  gdt_entry[TSS_INDEX].BaseHigh = (tss_base & 0xFF000000) >> 24;

  tss_entry.ss0 = KDATA_SELECTOR;
  tss_entry.esp0 = ((uint32_t) &kernel_stack_bottom + STACK_SIZE) - 4;

  _tss_flush();
}

void init_gdt(void) {
    //	Setup the GDT pointer and limit, always (sizeof(GDTENTRY) * entry count) - 1
	GDT.Limit = sizeof(gdt_entry) - 1;
	GDT.BasePtr = (uint32_t) gdt_entry;

	//	NULL Descriptor, not use, but has to be here.
	gdt_entry[NULL_DESCRIPTOR].LimitLow = 0;
	gdt_entry[NULL_DESCRIPTOR].BaseLow = 0;
	gdt_entry[NULL_DESCRIPTOR].BaseMiddle = 0;
	gdt_entry[NULL_DESCRIPTOR].AccessByte = 0;
	gdt_entry[NULL_DESCRIPTOR].FlagsAndLimit = 0;
	gdt_entry[NULL_DESCRIPTOR].BaseHigh = 0;
	
	//	0x08 - The second entry is our Code Segment. The base address
	//	is 0, the limit is 4GBytes, it uses 4KByte granularity,
	//	uses 32-bit opcodes, and is a Code Segment descriptor.
	//	Please check the table above in the tutorial in order
	//	to see exactly what each value means
	gdt_entry[KCODE_INDEX].LimitLow = 0xFFFF;
	gdt_entry[KCODE_INDEX].BaseLow = 0x0000;
	gdt_entry[KCODE_INDEX].BaseMiddle = 0x00;
	gdt_entry[KCODE_INDEX].AccessByte = 0b10011010;
	gdt_entry[KCODE_INDEX].FlagsAndLimit = 0xCF;
	gdt_entry[KCODE_INDEX].BaseHigh = 0x00;

	//	0x10 - The third entry is our Data Segment. It's EXACTLY the
	//	same as our code segment, but the descriptor type in
	//	this entry's access byte says it's a Data Segment
	gdt_entry[KDATA_INDEX].LimitLow = 0xFFFF;
	gdt_entry[KDATA_INDEX].BaseLow = 0x0000;
	gdt_entry[KDATA_INDEX].BaseMiddle = 0x00;
	gdt_entry[KDATA_INDEX].AccessByte = 0b10010010;
	gdt_entry[KDATA_INDEX].FlagsAndLimit = 0xCF;
	gdt_entry[KDATA_INDEX].BaseHigh = 0x00;

	//	0x18 - User mode code
	gdt_entry[UCODE_INDEX].LimitLow = 0xFFFF;
	gdt_entry[UCODE_INDEX].BaseLow = 0x0000;
	gdt_entry[UCODE_INDEX].BaseMiddle = 0x00;
	gdt_entry[UCODE_INDEX].AccessByte = 0b11111010;
	gdt_entry[UCODE_INDEX].FlagsAndLimit = 0xCF;
	gdt_entry[UCODE_INDEX].BaseHigh = 0x00;

	//	0x20 - User mode data
	gdt_entry[UDATA_INDEX].LimitLow = 0xFFFF;
	gdt_entry[UDATA_INDEX].BaseLow = 0x0000;
	gdt_entry[UDATA_INDEX].BaseMiddle = 0x00;
	gdt_entry[UDATA_INDEX].AccessByte = 0b11110010;
	gdt_entry[UDATA_INDEX].FlagsAndLimit = 0xCF;
	gdt_entry[UDATA_INDEX].BaseHigh = 0x00;

	/* Flush out the old GDT and install the new changes! */
	_gdt_flush(&GDT);
}