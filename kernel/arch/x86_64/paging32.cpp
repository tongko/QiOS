/*== QiOS =====================================================================+
|                                                                              |
|    Copyright (C) 2017 - 2020 Liew Tze Wei                                    |
|                                                                              |
|    This file is part of the QiOS kernel, and is made available under the     |
|    terms of The Unlicense (That means just do whatever you want with the     |
|    code base).                                                               |
|                                                                              |
+=============================================================================*/
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//   paging32.cpp                                                             //
//   -  Provides pre-long mode paging setup. This will be compile with        //
//      32-bits flag (-m32) since it is being executed before long mode was   //
//		enabled.                                                              //                                                      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#if !(__x86_64__)
#error "[QiOS (64 bits) - paging32.cpp] requires x86_64 architecture. Please use the right compiler options."
#endif

#if !(__is_kernel__)
#error "[QiOS (64 bits) - paging32.cpp] requires __IS_KERNEL, define 1."
#endif

////////////////////////////////////////////////////////////////////////////////
//    IMPLEMENTATION HEADERS                                                  //
////////////////////////////////////////////////////////////////////////////////

#include <stddef.h>
#include <stdint.h>
#include <sys/attribs.h>

////////////////////////////////////////////////////////////////////////////////
//    PRIVATE DEFINES AND TYPES                                               //
////////////////////////////////////////////////////////////////////////////////

#define ROUNDUP(x, y) (x % y ? x + (y - (x % y)) : x)
#define ROUNDDW(x, y) x - (x % y)

#define ENTRY_MASK 0x7FFFFFFFFF000UL
#define PML_OFFSET_MASK 0xFF8000000000LLU //‭111111111000000000000000000000000000000000000000‬b
#define PDP_OFFSET_MASK 0x7FC0000000LLU
#define PDE_OFFSET_MASK 0x3FE00000
#define PTE_OFFSET_MASK 0x1FF000
#define KERNEL_VIRTUAL_BASE 0xC0100000

#define PML_OFFSET(x) (x & PML_OFFSET_MASK) >> 37
#define PDP_OFFSET(x) (x & PDP_OFFSET_MASK) >> 28
#define PDE_OFFSET(x) (x & PDE_OFFSET_MASK) >> 21
#define PTE_OFFSET(x) (x & PTE_OFFSET_MASK) >> 12

////////////////////////////////////////////////////////////////////////////////
//    PRIVATE MEMBER                                                          //
////////////////////////////////////////////////////////////////////////////////

//	PML4 table, this will be place to CR3, each of this entry is pointing to
//	a PDPTR table, which each table hold 512 entries.
static _ALIGNED(0x1000) _EARLYDATA uint64_t *_pml4; // 512 entry

static void mem_set(void *dst, uint8_t value, size_t size)
{
	uint8_t *d = (uint8_t *)dst;
	for (; size > 0; size--)
	{
		*d = value;
	}
}

static void mem_cpy(void *dst, void *src, size_t size)
{
	uint8_t *d = (uint8_t *)dst;
	uint8_t *s = (uint8_t *)src;

	for (; size-- > 0; d++, s++)
	{
		*d = *s;
	}
}

////////////////////////////////////////////////////////////////////////////////
//    INTERFACE IMPLEMENTATIONS                                               //
////////////////////////////////////////////////////////////////////////////////

_EARLY void init_paging_32(
	uint32_t physical_start,
	uint32_t physical_end,
	uint32_t mb2_addr)
{
	//	First 2 MiB are meant for BIOS "low memory" staff + kernel.
	//	2 MiB onwards upto 6 MiB are reserved for the PML4.
	// identity map first 6 MiB.
	// round up to nearest page boundry
	_pml4 = (uint64_t *)0x200000;
	mem_set(_pml4, 0, 0x400000); //	Nullify _pml4
	uint64_t last_used = (uint64_t)_pml4;
	uint64_t virt = 0;
	size_t i = 0;
	size_t size = 0x600000; //	6 MiB
	for (size_t c = size; c > 0; c -= 0x1000, virt += 0x1000)
	{
		i = PML_OFFSET(virt);
		uint64_t *p3_ptr = (uint64_t *)(_pml4[i] & ENTRY_MASK);
		if (!p3_ptr)
		{
			last_used += 0x1000;
			_pml4[i] = last_used + 3;
			p3_ptr = (uint64_t *)last_used;
		}

		i = PDP_OFFSET(virt);
		uint64_t *p2_ptr = (uint64_t *)(p3_ptr[i] & ENTRY_MASK);
		if (!p2_ptr)
		{
			last_used += 0x1000;
			p3_ptr[i] = last_used + 3;
			p2_ptr = (uint64_t *)last_used;
		}

		i = PDE_OFFSET(virt);
		uint64_t *p1_ptr = (uint64_t *)(p2_ptr[i] & ENTRY_MASK);
		if (!p1_ptr)
		{
			last_used += 0x1000;
			p2_ptr[i] = last_used + 3;
			p1_ptr = (uint64_t *)last_used;
		}

		i = PTE_OFFSET(virt);
		uint64_t *p0_ptr = (uint64_t *)(p1_ptr[i] & ENTRY_MASK);
		if (!p0_ptr)
		{
			last_used += 0x1000;
			p1_ptr[i] = last_used + 3;
			p0_ptr = (uint64_t *)last_used;
		}

		p0_ptr[i] = (virt & ENTRY_MASK) + 3;
	}

	//	Identity map multiboot info
	size = ROUNDUP(*(size_t *)mb2_addr, 4096); //	get the length
	//	if mb2_addr fall within 6 MiB range, than we need to move it
	//	somewhere else;
	if (mb2_addr < 0x600000 && mb2_addr > 0x100000)
	{
		mem_cpy((void *)0x600000, (void *)mb2_addr, size);
		mb2_addr = 0x600000;
	}

	virt = ROUNDDW(mb2_addr, 4096);
	for (size_t c = size; c > 0; c -= 0x1000, virt += 0x1000)
	{
		i = PML_OFFSET(virt);
		uint64_t *p3_ptr = (uint64_t *)(_pml4[i] & ENTRY_MASK);
		if (!p3_ptr)
		{
			last_used += 0x1000;
			_pml4[i] = last_used + 3;
			p3_ptr = (uint64_t *)last_used;
		}

		i = PDP_OFFSET(virt);
		uint64_t *p2_ptr = (uint64_t *)(p3_ptr[i] & ENTRY_MASK);
		if (!p2_ptr)
		{
			last_used += 0x1000;
			p3_ptr[i] = last_used + 3;
			p2_ptr = (uint64_t *)last_used;
		}

		i = PDE_OFFSET(virt);
		uint64_t *p1_ptr = (uint64_t *)(p2_ptr[i] & ENTRY_MASK);
		if (!p1_ptr)
		{
			last_used += 0x1000;
			p2_ptr[i] = last_used + 3;
			p1_ptr = (uint64_t *)last_used;
		}

		i = PTE_OFFSET(virt);
		uint64_t *p0_ptr = (uint64_t *)(p1_ptr[i] & ENTRY_MASK);
		if (!p0_ptr)
		{
			last_used += 0x1000;
			p1_ptr[i] = last_used + 3;
			p0_ptr = (uint64_t *)last_used;
		}

		p0_ptr[i] = (virt & ENTRY_MASK) + 3;
	}

	//	map kernel higher half to first 1 MiB + Kernel Size
	size = ROUNDUP(physical_end - physical_start, 4096);
	uint64_t kstart = ROUNDDW(physical_start, 4096);
	virt = ROUNDDW(KERNEL_VIRTUAL_BASE, 4096);

	for (size_t c = size; c > 0; c -= 0x1000, virt += 0x1000, kstart += 0x1000)
	{
		i = PML_OFFSET(virt);
		uint64_t *p3_ptr = (uint64_t *)(_pml4[i] & ENTRY_MASK);
		if (!p3_ptr)
		{
			last_used += 0x1000;
			_pml4[i] = last_used + 3;
			p3_ptr = (uint64_t *)last_used;
		}

		i = PDP_OFFSET(virt);
		uint64_t *p2_ptr = (uint64_t *)(p3_ptr[i] & ENTRY_MASK);
		if (!p2_ptr)
		{
			last_used += 0x1000;
			p3_ptr[i] = last_used + 3;
			p2_ptr = (uint64_t *)last_used;
		}

		i = PDE_OFFSET(virt);
		uint64_t *p1_ptr = (uint64_t *)(p2_ptr[i] & ENTRY_MASK);
		if (!p1_ptr)
		{
			last_used += 0x1000;
			p2_ptr[i] = last_used + 3;
			p1_ptr = (uint64_t *)last_used;
		}

		i = PTE_OFFSET(virt);
		uint64_t *p0_ptr = (uint64_t *)(p1_ptr[i] & ENTRY_MASK);
		if (!p0_ptr)
		{
			last_used += 0x1000;
			p1_ptr[i] = last_used + 3;
			p0_ptr = (uint64_t *)last_used;
		}

		p0_ptr[i] = (kstart & ENTRY_MASK) + 3;
	}
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//    End paging32.cpp                                                        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
