/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 * ****************************************************************************/
#include <elf32.h>
#include <kstr.h>
#include <mem/memory.h>
#include <mem/pmm.h>
#include <mem/vmm.h>
#include <multiboot/mb_info.h>
#include <sys/panic.h>
#include "../ktypedef.h"

#ifndef ROUNDUP
#define ROUNDUP(x, y) (x % y ? x + (y - (x % y)) : x)
#endif

typedef struct {
	paddr_t start;
	paddr_t end;
} module_t;

static module_t _modules[64];

static bool addr_in_module(multiboot_uint64_t addr) {
	bool ret = false;
	for (int i = 0; i < 64; i++) {
		if (_modules[i].start == 0) {
			break;
		}

		if (addr >= _modules[i].start && addr <= _modules[i].end) {
			ret = true;
			break;
		}
	}

	return ret;
}

/******************************************************************************
 * The physical memory should look something like the following:
 *	0 - 0xFFFF : Occupied by BIOS and such
 *  0x10000 - kernel_end : Kernel image
 *	kernel_end - ? : physical mem table
 *	? - mem_size : free to use memory
 *****************************************************************************/
static void init_physical_mem(mbiapi_t *api, setup_info_t *setup_info) {
	mb_tag_meminfo_t *mem_info = api->get_meminfo();
	if (!mem_info) {
		kernel_panic("Invalid multiboot2 information, no basic memory tag.");
	}

	mb_tag_memmap_t *mem_map = api->get_mmap();
	if (!mem_info) {
		kernel_panic("Invalid multiboot2 information, no memory map tag.");
	}

	//	enumerate all the module address
	mb_tag_module_t *mod;
	int i = 0;
	for (mod = api->enum_module_info();
	     mod != NULL;
	     mod = api->enum_module_info()) {
		_modules[i].start = mod->mod_start;
		_modules[i++].end = mod->mod_end;
		_modules[i].start = 0;
		_modules[i].end = 0;
	}

	// Set the bitmap table to after kernel image, aligned to 4096
	uint32_t kern_end = ROUNDUP(setup_info->kernel_phys_end, 4096);
	pmm_init(mem_info->mem_upper, (paddr_t)kern_end);

	//	Set available memory blocks. This exclude those occupied by
	//	kernel and system data.
	mb_tag_memmap_entry_t *entry;
	for (entry = mem_map->entries;
	     (multiboot_uint8_t *)entry < (multiboot_uint8_t *)mem_map + mem_map->size;
	     entry = (mb_tag_memmap_entry_t *)((multiboot_uint32_t)entry + ((mb_tag_memmap_t *)mem_map)->entry_size)) {
		//	Only unset block if base above 1MiB and not in kernel space + 4096 and is type 1
		if (entry->type == 1 && entry->addr >= (kern_end + 4096) && !addr_in_module(entry->addr)) {
			//	Available memory
			pmm_init_region((paddr_t)entry->addr, (size_t)entry->len, false);
		}
	}
}

static void call_mod(mb_tag_module_t *mod) {
	printf("\nModule name: %s", mod->cmdline);

	elf32_ehdr_t *ehdr = (elf32_ehdr_t *)mod->mod_start;
	char *phdr_base;
	int err;
	phdr_base = (char *)mod->mod_start + ehdr->e_phoff;
#define phdr(i) ((elf32_phdr_t *)(phdr_base + (i) + ehdr->e_phentsize))
}

void mm_init(setup_info_t *setup_info) {
	mbiapi_t *api = mbi_api();

	init_physical_mem(api, setup_info);
	vmm_init();
}

uint32_t mm_total_physical() {
	return pmm_get_block_count() * PMM_BLOCK_SIZE * PMM_BLOCKS_PER_BYTE;
}

uint32_t mm_used_physical() {
	return pmm_get_used_block() * PMM_BLOCK_SIZE * PMM_BLOCKS_PER_BYTE;
}