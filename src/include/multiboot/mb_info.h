/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
#ifndef __MULTIBOOT_H_
#define __MULTIBOOT_H_

#include <attribs.h>
#include <elf32.h>
#include <kbool.h>
#include <multiboot/multiboot2.h>
#include <stdint.h>
#include <sys/types.h>

typedef struct {
	paddr_t addr;
	uint32_t magic;
} mbi_t;

typedef struct {
	bool (*is_loaded)(void);
	bool (*verify_magic)(uint32_t magic);
	mb_tag_meminfo_t *(*get_meminfo)(void);
	mb_tag_memmap_entry_t *(*get_mmap)(void);
	mb_tag_module_t *(*enum_module_info)(void);
	void (*load_mb2i)(uint32_t mb2_addr);
	void (*print_mb2i)(char *buffer);
	elf32_section_header_t *(*get_elf_sec_hdr)(const char *sec_name);
} mbiapi_t;

mbiapi_t *mbi_api(void) __early;
void mbi_init(mbiapi_t *api) __early;
bool mbi_is_init(void) __early;

#endif  //	__MULTIBOOT_H_