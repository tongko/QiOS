/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 * ****************************************************************************/
#include "paging.h"
#include "../asm.h"

uint32_t __earlydata _magic;
uint32_t __earlydata _addr;
uint64_t __earlydata *last_page_dir;
uint64_t __earlydata *last_page_tab;
uint64_t __earlydata page_dir_ptr_tab[4];
uint64_t __align(0x1000) __earlydata page_dir[ENTRY_SIZE * 4];  // must be aligned to page boundry
uint64_t __align(0x1000) __earlydata page_tab[ENTRY_SIZE * ENTRY_SIZE * 4];

#define ROUNDUP(x, y) (x % y ? x + (y - (x % y)) : x)
#define ROUNDDW(x, y) x - (x % y)

#define __to_entry(x) ((uint64_t)((uint32_t)x & ENTRY_MASK))
#define __tabn(x) (uint32_t)(((x & PT_MASK) >> 9) & ~3)
#define __dirn(x) (uint32_t)(((x & PD_MASK) >> 18) & ~3)
#define __ptrn(x) (uint32_t)((x & PDPTR_MASK) >> 30)
#define __pfn(x) (uint32_t)(x & 0xFFF)
#define __getd(x, y) (uint64_t *)(uint32_t)((x & ENTRY_MASK) | __dirn(y))
#define __gett(x, y) (uint64_t *)(uint32_t)((x & ENTRY_MASK) | __tabn(y))
#define __getp(x, y) (paddr_t)((x & ENTRY_MASK) | __pfn(y))

__early void map_page(vaddr_t from, size_t count, paddr_t physical) {
	size_t c = count;
	// Page schema for 32 bits PAE set, PSE unset:
	//	2 | 9 | 9 | 12
	for (; c > 0; from += 0x1000, physical += 0x1000, c -= 0x1000) {
		uint32_t p = (from & PDPTR_MASK) >> 30;  // _ptrn(from & PDPTR_MASK);
		uint64_t pdpte_i = page_dir_ptr_tab[p];
		// check PDPTEi, if P flag is not 1, or pdpte is zero
		if (!(pdpte_i & 1) || !(pdpte_i & ENTRY_MASK)) {
			pdpte_i = page_dir_ptr_tab[p] = __to_entry(last_page_dir) | 1;
			last_page_dir += ENTRY_SIZE;
		}
		//	retrieve page dir entry base on dir first element and [from]
		uint64_t *pg_dir_entry = __getd(pdpte_i, from);
		//	Page table should not be zero
		if (!(*pg_dir_entry & 1) || !(*pg_dir_entry & ENTRY_MASK)) {
			*pg_dir_entry = __to_entry(last_page_tab) | 3;
			last_page_tab += ENTRY_SIZE;
		}
		uint64_t *pg_tab_entry = __gett(*pg_dir_entry, from);
		*pg_tab_entry = __to_entry(physical) | 3;
	}
}

static __early paddr_t virt_to_phys(vaddr_t vaddr) {
	uint64_t pdptr_e = page_dir_ptr_tab[__ptrn(vaddr)];
	if ((pdptr_e & 1) == 0) {  // directory not present
		return 0;
	}
	uint64_t *pg_dir = __getd(pdptr_e, vaddr);
	if ((*pg_dir & 1) == 0) {
		return 0;
	}
	uint64_t *pg_tab = __gett(*pg_dir, vaddr);
	if ((*pg_tab & 1) == 0) {
		return 0;
	}
	paddr_t ret = __getp(*pg_tab, vaddr);
	return ret;
}

__early void early_init_paging(kernel_mem_info_t kmem_info, uint32_t mb2_addr) {
	last_page_dir = page_dir;
	last_page_tab = page_tab;

	uint32_t ksize = kmem_info.physical_end - kmem_info.physical_start;
	// identity map first 1 MiB + kernel size.
	// round up to nearest page boundry
	uint32_t offset = ROUNDUP(0x100000 + ksize, 4096);
	map_page((vaddr_t)0, offset, (paddr_t)0);
	if (virt_to_phys(0x130000) != 0x130000) {
		while (1) {
		}  // error
	}

	//	identity map multiboot info loaded address.
	uint32_t mb2_loaded = ROUNDDW(mb2_addr, 4096);
	uint32_t size = *((uint32_t *)((void *)mb2_addr));
	offset = ROUNDUP(size, 4096);
	map_page((vaddr_t)mb2_loaded, offset, (paddr_t)mb2_loaded);

	//	map higher half to page, start from 0xC0100000
	offset = ROUNDUP((kmem_info.physical_end - kmem_info.physical_start) + 0xB00000, 4096);  // + 0x100000;
	vaddr_t kvb = (vaddr_t)ROUNDUP(KERNEL_VIRTUAL_BASE, 4096);
	paddr_t pss = (paddr_t)ROUNDUP(kmem_info.physical_start, 4096);
	map_page(kvb, offset, pss);
	if (virt_to_phys(KERNEL_VIRTUAL_BASE) != kmem_info.physical_start) {
		while (1) {
		}  //error
	}

	if (virt_to_phys(0xc0403c4c) != 0x00403c4c) {
		while (1) {
		}  // error
	}

	// move PDPTR to CR3
	set_cr3((uint32_t)&page_dir_ptr_tab);

	// set CR4.PAE bit
	uint32_t cr4 = get_cr4();
	cr4 |= 0x20;
	set_cr4(cr4);

	// set CR0.PG bit
	uint32_t cr0 = get_cr0();
	cr0 |= 0x80000000;
	set_cr0(cr0);
}

// __earlydata uint32_t efer;
// uint32_t __early debug(uint32_t eflags, uint32_t cpuid_enabled) {
// 	if (!cpuid_enabled) {
// 		return eflags;
// 	}

// 	_asm(
// 	    "mov eax, 0x00;\n"
// 	    "cpuid;\n" ::);
// 	_asm(
// 	    "mov eax, 0x80000008;\n"
// 	    "cpuid;\n" ::);
// 	efer = 0;
// 	_asm(
// 	    "mov ecx, 0xC0000080;\n"
// 	    "rdmsr;\n"
// 	    : "=r"(efer)
// 	    :);
// 	uint32_t efer_lma = ~(1U << 10);
// 	_asm(
// 	    "and eax, %0;\n"
// 	    "wrmsr"
// 	    :
// 	    : "r"(efer_lma)
// 	    :);
// 	// disable EFER.LMA
// 	_asm("nop;\n" ::);

// 	return eflags;
// }