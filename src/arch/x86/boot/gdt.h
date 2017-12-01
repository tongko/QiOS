#ifndef __GDT_H_
#define __GDT_H_

#include <stdint.h>
#define _need_SIZE_T
#include "ktypedef.h"

// A struct describing a Task State Segment.
typedef struct tss_entry {
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
} __attribute__((packed)) tss_entry_t;

//	Defines a GDT entry. Use packed, because it prevents the compiler from doing things that it thinks
//	is best: Prevent compiler "optimization" by packing
/*	Access Byte:
*	   7     |                        0
*   --------------------------------------
*   | Pr | Privl | 1 | Ex | DC | RW | Ac |
*	--------------------------------------
*	Pr   : Present bit. This must be 1 for all valid selectors.
*	Privl: Privilege, 2 bits. Contains the ring level, 00 = Highest (kernel), 03 = lowest (user application)
*	Ex   : Executable bit. If 1 code in this segment can be executed, ie. a code selector. If 0 it is a data selector.
*	DC   : Direction bit/Conforming bit.
*			- If 1 code in this segment can be executed from an equal or lower privilege level. For example
*				code in ring 3 can far-jump to conforming code in a ring 2 segment. The privl-bits represent
*				the highest privilege level that is allowed to execute the segment. For example, code in ring
*				0 cannot far-jump to a conforming code segment with privl=0x2, while code in ring 2 and 3 can.
*				Note that the privilege level remains the same, ie. a far-jump from ring 3 to a privl==2-segment
*				remains in ring 3 after the jump.
*			- If 0 code in this segment can only be executed from the ring set in privl.
*	RW   : Readable bit/Writable bit.
*			- Readable bit for code selectors: Whether read access for this segment is allowed. Write access is
*			  never allowed for code segments.
*			- Writable bit for data selectors: Whether write access for this segment is allowed. Read acces is
*			  always allowed for data segments.
*	Ac   : Accessed bit. Just set to 0. The CPU sets this to 1 when the segment is accessed.
*
*	Flags:
*	   7             4
*   ---------------------
*   | Gr | Sz | 0  | 0  |
*	---------------------
*	Gr   : Granularity bit. If 0 the limit is in 1 B blocks (byte granularity), if 1 then the limit is in 4 KB
*		   blocks (page granularity).
*	Sz   : Size bit. If 0 the selector defines 16 bit protected mode. If 1 it defines 32 bit protected mode.
*		   You can have both 16 bit and 32 bit selectors at once.
*/
typedef struct gdt_entry {
	uint16_t limit_low;   //	Bits 0-15 (Bits 0-15 of 20 bits)
	uint16_t base_low;    //	Bits 16-31 (Bits 0-15 of 32 bits)
	uint8_t base_middle;  //	Bits 32-39 (Bits 16-23 of 32 bits)
	uint8_t access_byte;  //	Bits 40-47
	//uint8_t		LimitHigh;					//	Bits 48-51 (Bits 16-19 of 20 bits)
	uint8_t flags_n_limits;  //	Bits 48-55
	uint8_t base_high;       //	Bits 56-63 (Bits 24-31 of 32 bits)
} __attribute__((packed)) gdt_entry_t;
//	Special pointer which includes the limit: The max bytes taken up by the GDT, minus 1. Again, this
//	NEEDS to be packed
typedef struct gdt {
	uint16_t limit;
	uint32_t base_ptr;
} __attribute__((packed)) gdt_t;

// __attribute__((aligned(2), section(".bootdata"))) const uint16_t NULL_SELECTOR;
// __attribute__((aligned(2), section(".bootdata"))) const uint16_t KCODE_SELECTOR;
// __attribute__((aligned(2), section(".bootdata"))) const uint16_t KDATA_SELECTOR;
__attribute__((aligned(8), section(".bootdata"))) gdt_entry_t gdt_entry[6];
__attribute__((aligned(8), section(".bootdata"))) gdt_t gdt_ptr;
__attribute__((aligned(128), section(".bootdata"))) tss_entry_t tss_entry;

enum segment_selector_t {
	NULL_DESCRIPTOR,  // Not used but has to be here
	KCODE_INDEX,      // Offset 0x8
	KDATA_INDEX,      // Offset 0x10
	UCODE_INDEX,      // Offset 0x18
	UDATA_INDEX,      // Offset 0x20
	TSS_INDEX         // Offset 0x28
};

//	Initialize GDT.
__attribute__((section(".boot"))) void early_init_gdt(void);
//	Initialize TSS.
void init_tss(void *kernel_stack_bottom, size_t stack_size);

#endif  //	__GDT_H_