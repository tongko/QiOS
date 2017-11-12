#include "boot.h"

void gdt_set_gate(uint32_t num, uint32_t base, uint32_t limit, unsigned char access, unsigned char gran) {
    /* Setup the descriptor base address */
	gdt[num].base_low = (base & 0xFFFF);
	gdt[num].base_middle = (base >> 16) & 0xFF;
	gdt[num].base_high = (base >> 24) & 0xFF;

	/* Setup the descriptor limits */
	gdt[num].limit_low = (limit & 0xFFFF);
	gdt[num].granularity = ((limit >> 16) & 0x0F);

	/* Finally, set up the granularity and access flags */
	gdt[num].granularity |= (gran & 0xF0);
	gdt[num].access = access;
}

void gdt_init(void) {
    //	Setup the GDT pointer and limit
	gp.limit = (sizeof(gdt_entry) * 6) - 1;
	gp.base = (uint32_t) &gdt;

	//	Our NULL descriptor
	gdt_set_gate(0, 0, 0, 0, 0);

	//	0x08 - The second entry is our Code Segment. The base address
	//	is 0, the limit is 4GBytes, it uses 4KByte granularity,
	//	uses 32-bit opcodes, and is a Code Segment descriptor.
	//	Please check the table above in the tutorial in order
	//	to see exactly what each value means
	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

	//	0x10 - The third entry is our Data Segment. It's EXACTLY the
	//	same as our code segment, but the descriptor type in
	//	this entry's access byte says it's a Data Segment
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

	//	0x18 - Make room for TSS
	gdt_set_gate(3, 0, 0xFFFFFFFF, 0x92, 0xCF);

	//	0x20 - User mode code
	gdt_set_gate(4, 0, 0xFFFFFFFF, 0xFA, 0xCF);

	//	0x28 - User mode data
	gdt_set_gate(5, 0, 0xFFFFFFFF, 0xC2, 0xCF);

	/* Flush out the old GDT and install the new changes! */
	gdt_flush((gdt_ptr*)&gp);
}