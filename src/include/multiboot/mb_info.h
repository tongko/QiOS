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

#include <elf32.h>
#include <multiboot/multiboot2.h>
#include <stdint.h>
#include <sys/types.h>

typedef struct {
	multiboot_tag_mmap_t *(*get_mmap)(void);
	void (*load_mb2i)(uint32_t mb2_addr);
	void (*print_mb2i)(char *buffer);
	elf32_section_header_t *(*get_elf_sec_hdr)(const char *sec_name);
} mbiapi_t;

mbiapi_t *mbi_info_api(void);
void init_mb_info(mbiapi_t *api);

#endif  //	__MULTIBOOT_H_