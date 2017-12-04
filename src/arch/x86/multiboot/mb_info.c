/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
#include <multiboot/mb_info.h>

static void *mb2_info;

static struct multiboot_tag *get_multiboot_tag(uint32_t type) {
	struct multiboot_tag *tag;
	for (tag = (struct multiboot_tag *)(mb2_info + 8);
	     tag->type != MULTIBOOT_TAG_TYPE_END;
	     tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7))) {
		if (tag->type == type) {
			return tag;
		}
	}

	return NULL;
}

void init_mb_info(mbiapi_t *api) {
	if (!api) {
		__mbiapi = {
			.get_mmap = get_mmap,
			.load_mb2i = load_mb2i,
			.print_mb2i = print_mb2i,
			.get_elf_sec_hdr = get_elf_sec_hdr
		}
	} else {
		__mbiapi.get_mmap = api->get_mmap;
		__mbiapi.load_mb2i = api->load_mb2i;
		__mbiapi.print_mb2i = api->print_mb2i;
		__mbiapi.get_elf_sec_hdr = api->get_elf_sec_hdr;
	}
}

void load_mb2i(uint32_t mbi_addr) {
	if (mbi_addr) {
		mb2_info = (void *)mbi_addr;
	}
}

multiboot_tag_mmap get_mmap(void) {
	return (multiboot_tag_mmap_t *)get_multiboot_tag(MULTIBOOT_TAG_TYPE_MMAP);
}

void print_mb2i(sysstr_t buffer) {
}

elf32_section_header_t get_elf_sec_hdr(sysstr_t sec_name) {
	struct multiboot_tag_elf_sections *elf =
	    (struct multiboot_tag_elf_sections *)get_multiboot_tag(MULTIBOOT_TAG_TYPE_ELF_SECTIONS);
	if (!elf) {
		return NULL;
	}

	elf32_section_header_t *section_headers = (elf32_section_header_t *)elf->sections;
	uint32_t string_table_start = physical_to_virtual(section_headers[elf->shndx].sh_addr);
	for (uint32_t i = 0; i < elf->num; i++) {
		uint32_t sh_name = section_headers[i].sh_name;
		if (sh_name) {
			char *name = (char *)(string_table_start + sh_name);
			if (_strcmp(section_name, name) == 0) {
				return section_headers + i;
			}
		}
	}

	return NULL;
}