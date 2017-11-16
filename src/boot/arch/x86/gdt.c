#include "gdt.h"


void create_entry(uint32_t index, uint32_t base, uint32_t limit, uint16_t flag) {
    uint64_t descriptor;
 
    // Create the high 32 bit segment
    descriptor  =  limit       & 0x000F0000;         // set limit bits 19:16
    descriptor |= (flag <<  8) & 0x00F0FF00;         // set type, p, dpl, s, g, d/b, l and avl fields
    descriptor |= (base >> 16) & 0x000000FF;         // set base bits 23:16
    descriptor |=  base        & 0xFF000000;         // set base bits 31:24
 
    // Shift by 32 to allow for low part of segment
    descriptor <<= 32;
 
    // Create the low 32 bit segment
    descriptor |= base  << 16;                       // set base bits 15:0
    descriptor |= limit  & 0x0000FFFF;               // set limit bits 15:0

	_gdtEntry[index] = descriptor;
}

// void gdt_set_gate(uint32_t dwIndex, uint32_t lpBase, uint32_t dwLimit, uint8_t bAccess, uint8_t bGran) {
//     /* Setup the descriptor base address */
// 	_gdtEntry[num].BaseLow = (lpBase & 0xFFFF);
// 	_gdtEntry[num].BaseMiddle = (lpBase >> 16) & 0xFF;
// 	_gdtEntry[num].BaseHigh = (lpBase >> 24) & 0xFF;

// 	/* Setup the descriptor limits */
// 	_gdtEntry[num].LimitLow = (limit & 0xFFFF);
// 	_gdtEntry[num].Granularity = ((limit >> 16) & 0x0F);

// 	/* Finally, set up the granularity and access flags */
// 	_gdtEntry[num].Granularity |= (gran & 0xF0);
// 	_gdtEntry[num].Access = access;
// }

void _gdt_init(void) {
    //	Setup the GDT pointer and limit, always (sizeof(GDTENTRY) * entry count) - 1
	_ptrGdt.Limit = (sizeof(GDTENTRY) * 6) - 1;
	_ptrGdt.BasePtr = (uint32_t) &_gdtEntry;

	//	Our NULL descriptor
	//gdt_set_gate(0, 0, 0, 0, 0);
	create_entry(0, 0, 0, 0);

	//	0x08 - The second entry is our Code Segment. The base address
	//	is 0, the limit is 4GBytes, it uses 4KByte granularity,
	//	uses 32-bit opcodes, and is a Code Segment descriptor.
	//	Please check the table above in the tutorial in order
	//	to see exactly what each value means
	//gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
	create_entry(1, 0, 0x000FFFFF, (GDT_CODE_PL0));

	//	0x10 - The third entry is our Data Segment. It's EXACTLY the
	//	same as our code segment, but the descriptor type in
	//	this entry's access byte says it's a Data Segment
	//gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
	create_entry(2, 0, 0x000FFFFF, (GDT_DATA_PL0));

	//	0x18 - User mode code
	//gdt_set_gate(3, 0, 0xFFFFFFFF, 0x92, 0xCF);
	create_entry(3, 0, 0x000FFFFF, (GDT_CODE_PL3));

	//	0x20 - User mode data
	//gdt_set_gate(4, 0, 0xFFFFFFFF, 0xFA, 0xCF);
	create_entry(4, 0, 0x000FFFFF, (GDT_DATA_PL3));

	//	0x28 - Make room for TSS
	//gdt_set_gate(5, 0, 0xFFFFFFFF, 0xC2, 0xCF);
	//create_entry(5, 0, 0x000FFFFF,)

	/* Flush out the old GDT and install the new changes! */
	_gdt_flush((PGDT)&_ptrGdt);
}