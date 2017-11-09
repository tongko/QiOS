#ifndef		__X86_GDT_H_
#define		__X86_GDT_H_

#include	<stdint.h>

//	Defines a GDT entry. Use packed, because it prevents the compiler from doing things that it thinks
//	is best: Prevent compiler "optimization" by packing
typedef struct _gdt_entry {
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_middle;
	uint8_t access;
	uint8_t granularity;
	uint8_t base_high;
} gdt_entry __attribute__((packed));

//	Special pointer which includes the limit: The max bytes taken up by the GDT, minus 1. Again, this
//	NEEDS to be packed
typedef struct _gdt_ptr
{
	unsigned short limit;
	unsigned int base;
} gdt_ptr __attribute__((packed));

//	Our GDT, with 3 entries, and finally our special GDT pointer
gdt_entry gdt[4];
gdt_ptr gp;

//	This will be a function in multiboot.asm. We use this to properly reload the new segment registers
extern void gdt_flush(gdt_ptr* ptr);

//	Setup a descriptor in the GDT.
void gdt_set_gate(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);
//	Initialize GDT.
void gdt_init(void);

#endif		//	__X86_GDT_H_