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
#include "asm.h"

uint64_t __earlydata *last_page_dir;
uint64_t __earlydata *last_page_tab;
uint64_t __align(0x20) __earlydata page_dir_ptr_tab[4];
uint64_t __align(0x4000) __earlydata page_dir[ENTRY_SIZE];  // must be aligned to page boundry
uint64_t __align(0x200000) __earlydata page_tab[ENTRY_SIZE];

#define __to_entry(x) ((uint64_t)((uint32_t)x & ENTRY_MASK))
#define __tabn(x) (uint32_t)(((x & PT_MASK) >> 9) & ~3)
#define __dirn(x) (uint32_t)(((x & PD_MASK) >> 18) & ~3)
#define __ptrn(x) (uint32_t)((x & PDPTR_MASK) >> 30)
#define __pfn(x) (uint32_t)(x & 0xFFF)
#define __getd(x, y) (uint64_t *)(uint32_t)((x & ENTRY_MASK) | __dirn(y))
#define __gett(x, y) (uint64_t *)(uint32_t)((x & ENTRY_MASK) | __tabn(y))
#define __getp(x, y) (paddr_t)((x & ENTRY_MASK) | __pfn(y))

void map_page(vaddr_t from, size_t count, paddr_t physical) {
	// Page schema for 32 bits PAE set, PSE unset:
	//	2 | 9 | 9 | 12
	for (; count > 0; from += 0x1000, physical += 0x1000, count -= 0x1000) {
		uint32_t p = (from & PDPTR_MASK) >> 30;
		// check PDPTEi
		uint64_t pdpte = page_dir_ptr_tab[p];
		if (!(pdpte & ENTRY_MASK)) {
			page_dir_ptr_tab[p] = __to_entry(last_page_dir) | 1;
			last_page_dir += ENTRY_SIZE;
		}
		uint64_t *pg_dir_entry = __getd(page_dir_ptr_tab[p], from);
		if (((uint64_t)*pg_dir_entry & ENTRY_MASK) == 0) {  //	page_table should not be place at 0
			*pg_dir_entry = __to_entry(last_page_tab) | 3;
			last_page_tab += ENTRY_SIZE;
		}
		uint64_t *pg_tab_entry = __gett(*pg_dir_entry, from);
		if (((uint64_t)*pg_tab_entry & ENTRY_MASK) == 0) {  //	page_tab should not be place at 0
			*pg_tab_entry = __to_entry(physical) | 1;
		}
	}
}

static paddr_t __early virt_to_phys(vaddr_t vaddr) {
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

void early_init_paging(kernel_mem_info_t kmem_info) {
	last_page_dir = page_dir;
	last_page_tab = page_tab;

	// identity map first 4MiB
	uint32_t offset = kmem_info.physical_end - kmem_info.physical_start;
	map_page((vaddr_t)0, offset + 0x100000, (paddr_t)0);
	if (virt_to_phys(0x180000) != 0x180000) {
		while (1) {
		}  // error
	}

	// uint64_t *pg_dir = (uint64_t *)(uint32_t)(page_dir_ptr_tab[3] & ~1);  // get the page directory (you should 'and' the flags away)
	// pg_dir[511] = (uint64_t)(uint32_t)page_dir;                           // map pd to itself
	// pg_dir[510] = page_dir_ptr_tab[2];                                    // map pd3 to it
	// pg_dir[509] = page_dir_ptr_tab[1];                                    // map pd2 to it
	// pg_dir[508] = page_dir_ptr_tab[0];                                    // map pd1 to it
	// pg_dir[507] = (uint64_t)(uint32_t)&page_dir_ptr_tab;                  // map the PDPT to the directory

	//	map higher half to page, start from 0xC0100000
	map_page((vaddr_t)KERNEL_VIRTUAL_BASE, offset, kmem_info.physical_start);
	if (virt_to_phys(KERNEL_VIRTUAL_BASE) != kmem_info.physical_start) {
		while (1) {
		}  //error
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

__attribute__((section(".bootdata"))) uint32_t efer;
uint32_t debug(uint32_t eflags, uint32_t cpuid_enabled) {
	if (!cpuid_enabled) {
		return eflags;
	}

	asm("mov eax, 0x00;\n"
	    "cpuid;\n" ::);
	asm("mov eax, 0x80000008;\n"
	    "cpuid;\n" ::);
	efer = 0;
	asm("mov ecx, 0xC0000080;\n"
	    "rdmsr;\n"
	    : "=r"(efer)
	    :);
	uint32_t efer_lma = ~(1U << 10);
	asm("and eax, %0;\n"
	    "wrmsr"
	    :
	    : "r"(efer_lma)
	    :);
	// disable EFER.LMA
	asm("nop;\n" ::);

	return eflags;
}