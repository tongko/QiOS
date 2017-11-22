#ifndef		__X86_GDT_H_
#define		__X86_GDT_H_

#include	<stdint.h>

// Each define here is for a specific flag in the descriptor.
// Refer to the intel documentation for a description of what each one does.
#define SEG_DESCTYPE(x)	((x) << 0x04)			// Descriptor type (0 for system, 1 for code/data)
#define SEG_PRES(x)		((x) << 0x07)			// Present
#define SEG_SAVL(x)		((x) << 0x0C)			// Available for system use
#define SEG_LONG(x)		((x) << 0x0D)			// Long mode
#define SEG_SIZE(x)		((x) << 0x0E)			// Size (0 for 16-bit, 1 for 32)
#define SEG_GRAN(x)		((x) << 0x0F)			// Granularity (0 for 1B - 1MB, 1 for 4KB - 4GB)
#define SEG_PRIV(x)	 	(((x) &  0x03) << 0x05)	// Set privilege level (0 - 3)
 
#define SEG_DATA_R		0x00 // Read-Only
#define SEG_DATA_RA		0x01 // Read-Only, accessed
#define SEG_DATA_RW		0x02 // Read/Write
#define SEG_DATA_RWA	0x03 // Read/Write, accessed
#define SEG_DATA_RE		0x04 // Read-Only, expand-down
#define SEG_DATA_REA	0x05 // Read-Only, expand-down, accessed
#define SEG_DATA_RWED	0x06 // Read/Write, expand-down
#define SEG_DATA_RWEDA	0x07 // Read/Write, expand-down, accessed
#define SEG_CODE_X		0x08 // Execute-Only
#define SEG_CODE_XA		0x09 // Execute-Only, accessed
#define SEG_CODE_XR		0x0A // Execute/Read
#define SEG_CODE_XRA	0x0B // Execute/Read, accessed
#define SEG_CODE_XC		0x0C // Execute-Only, conforming
#define SEG_CODE_XCA	0x0D // Execute-Only, conforming, accessed
#define SEG_CODE_XRC	0x0E // Execute/Read, conforming
#define SEG_CODE_XRCA	0x0F // Execute/Read, conforming, accessed
 
#define GDT_CODE_PL0 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
					 SEG_LONG(0)	 | SEG_SIZE(1) | SEG_GRAN(1) | \
					 SEG_PRIV(0)	 | SEG_CODE_XR
 
#define GDT_DATA_PL0 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
					 SEG_LONG(0)	 | SEG_SIZE(1) | SEG_GRAN(1) | \
					 SEG_PRIV(0)	 | SEG_DATA_RW
 
#define GDT_CODE_PL3 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
					 SEG_LONG(0)	 | SEG_SIZE(1) | SEG_GRAN(1) | \
					 SEG_PRIV(3)	 | SEG_CODE_XR
 
#define GDT_DATA_PL3 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
					 SEG_LONG(0)	 | SEG_SIZE(1) | SEG_GRAN(1) | \
					 SEG_PRIV(3)	 | SEG_DATA_RW
					 
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
typedef struct tagGDTENTRY {
	uint16_t	LimitLow;					//	Bits 0-15 (Bits 0-15 of 20 bits)
	uint16_t	BaseLow;					//	Bits 16-31 (Bits 0-15 of 32 bits)
	uint8_t		BaseMiddle;					//	Bits 32-39 (Bits 16-23 of 32 bits)
	uint8_t		AccessByte;					//	Bits 40-47
	uint8_t		LimitHigh;					//	Bits 48-51 (Bits 16-19 of 20 bits)
	uint8_t		Flags;						//	Bits 52-55
	uint8_t		BaseHigh;					//	Bits 56-63 (Bits 24-31 of 32 bits)
} __attribute__((packed)) GDTENTRY, *PGDTENTRY;

//	Special pointer which includes the limit: The max bytes taken up by the GDT, minus 1. Again, this
//	NEEDS to be packed
typedef struct tagGDT
{
	uint16_t	Limit;
	uint32_t	BasePtr;
} __attribute__((packed)) GDT, *PGDT;

//	Our GDT, with 6 entries, and finally our special GDT pointer
//GDTENTRY	_gdtEntry[6];
uint64_t	_gdtEntry[5];
GDT			_ptrGdt;

//	This will be a function in multiboot.asm. We use this to properly reload the new segment registers
extern void _gdt_flush(PGDT lpGdt);

// //	Setup a descriptor in the GDT.
// void GdtSetGate(uint32_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity);
//	Initialize GDT.
void _gdt_init(void);

#endif		//	__X86_GDT_H_