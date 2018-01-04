/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
#include <kio.h>
#include <kstr.h>
#include <multiboot/mb_info.h>
#include <stddef.h>

static void *_mb2_info;
static mbiapi_t _api;

static struct multiboot_tag *get_multiboot_tag(uint32_t type) {
	if (!_mb2_info) {
		return NULL;
	}

	struct multiboot_tag *tag;
	for (tag = (struct multiboot_tag *)((uint32_t)_mb2_info + 8);
	     tag->type != MULTIBOOT_TAG_TYPE_END;
	     tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7))) {
		if (tag->type == type) {
			return tag;
		}
	}

	return NULL;
}

static void load_mb2i(uint32_t mbi_addr) {
	if (mbi_addr) {
		if (mbi_addr & 7) {
			printf("Unaligned MBI: 0x%x.8\n", mbi_addr);
			while (1) {
			}
		}

		_mb2_info = (void *)mbi_addr;
	}
}

static mb_tag_memmap_t *get_mmap(void) {
	return (mb_tag_memmap_t *)get_multiboot_tag(MULTIBOOT_TAG_TYPE_MMAP);
}

static mb_tag_meminfo_t *get_meminfo(void) {
	return (mb_tag_meminfo_t *)get_multiboot_tag(MULTIBOOT_TAG_TYPE_BASIC_MEMINFO);
}

static mb_tag_module_t *enum_module_info(void) {
	static mb_tag_module_t *ptr;
	struct multiboot_tag *tag;
	if (!ptr) {
		tag = (struct multiboot_tag *)((uint32_t)_mb2_info + 8);
		while (tag && tag->type != MULTIBOOT_TAG_TYPE_END) {
			if (tag->type == MULTIBOOT_TAG_TYPE_MODULE) {
				ptr = (mb_tag_module_t *)tag;
				break;
			}

			tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7));
		}
	} else {
		tag = (struct multiboot_tag *)ptr;
		tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7));
		ptr = tag->type != MULTIBOOT_TAG_TYPE_MODULE ? NULL : (mb_tag_module_t *)tag;
	}

	return ptr;
}

static void print_mb2i(char *buffer) {
	if (!buffer) {
		return;
	}

	if (!_mb2_info) {
		return;
	}

	char *p = buffer;
	uint32_t size = *(uint32_t *)_mb2_info;
	size_t total_printed = sprintf(p, "Multiboot Info loaded at: %#.8x\n", (uint32_t)_mb2_info);
	while (*p++) {
	}
	p--;
	total_printed = sprintf(p, "Announced mbi size %d Bytes.\n", size);
	while (*p++) {
	}
	p--;

	struct multiboot_tag *tag;
	for (tag = (struct multiboot_tag *)((uint32_t)_mb2_info + 8);
	     tag->type != MULTIBOOT_TAG_TYPE_END;
	     tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7))) {
		total_printed += sprintf(p, "Tag %#.2x, Size %#.16x\n", tag->type, tag->size);
		while (*p++) {
		}
		p--;
		switch (tag->type) {
			case MULTIBOOT_TAG_TYPE_CMDLINE:
				total_printed += sprintf(p, "-> Command line = %s\n",
				                         ((struct multiboot_tag_string *)tag)->string);
				while (*p++) {
				}
				p--;
				break;
			case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
				total_printed += sprintf(p, "-> Boot loader name = %s\n",
				                         ((struct multiboot_tag_string *)tag)->string);
				while (*p++) {
				}
				p--;
				break;
			case MULTIBOOT_TAG_TYPE_MODULE:
				total_printed += sprintf(p, "-> Module at %#.16x-%#.16x. Command line %s\n",
				                         ((mb_tag_module_t *)tag)->mod_start,
				                         ((mb_tag_module_t *)tag)->mod_end,
				                         ((mb_tag_module_t *)tag)->cmdline);
				while (*p++) {
				}
				p--;
				break;
			case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
				total_printed += sprintf(p, "-> mem_lower = %uKiB, mem_upper = %uKiB\n",
				                         ((mb_tag_meminfo_t *)tag)->mem_lower,
				                         ((mb_tag_meminfo_t *)tag)->mem_upper);
				while (*p++) {
				}
				p--;
				break;
			case MULTIBOOT_TAG_TYPE_BOOTDEV:
				total_printed += sprintf(p, "-> Boot device %#.4x, slice: %u, part: %u\n",
				                         ((struct multiboot_tag_bootdev *)tag)->biosdev,
				                         ((struct multiboot_tag_bootdev *)tag)->slice,
				                         ((struct multiboot_tag_bootdev *)tag)->part);
				while (*p++) {
				}
				p--;
				break;
			case MULTIBOOT_TAG_TYPE_MMAP: {
				mb_tag_memmap_entry_t *mmap;

				total_printed += sprintf(p, "-> Memory Map\n");
				while (*p++) {
				}
				p--;
				// int32_t count = 0;
				for (mmap = ((mb_tag_memmap_t *)tag)->entries;
				     (multiboot_uint8_t *)mmap < (multiboot_uint8_t *)tag + tag->size;
				     mmap = (mb_tag_memmap_entry_t *)((unsigned long)mmap + ((mb_tag_memmap_t *)tag)->entry_size)) {
					total_printed += sprintf(p,
					                         //	For 32 bits only 32 bit address exists.
					                         //"\tbase_addr = 0x%x%x, length = 0x%x%x, type = 0x%x\n",
					                         "\tbase_addr = %#.16x, length=%#.16x, type = %#.2x\n",
					                         (uint32_t)(mmap->addr & 0xffffffff),
					                         (uint32_t)(mmap->len & 0xffffffff),
					                         (uint32_t)mmap->type);
					while (*p++) {
					}
					p--;
				}
				break;
			}
			// case MULTIBOOT_TAG_TYPE_VBE: {
			// 	struct multiboot_tag_vbe *vbe = (struct multiboot_tag_vbe *)tag;
			// }

			case MULTIBOOT_TAG_TYPE_FRAMEBUFFER: {
				multiboot_uint32_t color;
				uint32_t i;
				struct multiboot_tag_framebuffer *tagfb = (struct multiboot_tag_framebuffer *)tag;
				void *fb = (void *)(unsigned long)tagfb->common.framebuffer_addr;

				switch (tagfb->common.framebuffer_type) {
					case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED: {
						unsigned best_distance, distance;
						struct multiboot_color *palette;

						palette = tagfb->framebuffer_palette;

						color = 0;
						best_distance = 4 * 256 * 256;

						for (i = 0; i < tagfb->framebuffer_palette_num_colors; i++) {
							distance = (0xff - palette[i].blue) * (0xff - palette[i].blue) + palette[i].red * palette[i].red + palette[i].green * palette[i].green;
							if (distance < best_distance) {
								color = i;
								best_distance = distance;
							}
						}
					} break;

					case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:
						color = ((1 << tagfb->framebuffer_blue_mask_size) - 1)
						        << tagfb->framebuffer_blue_field_position;
						break;

					case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:
						color = '\\' | 0x0100;
						break;

					default:
						color = 0xffffffff;
						break;

						for (i = 0; i < tagfb->common.framebuffer_width && i < tagfb->common.framebuffer_height; i++) {
							switch (tagfb->common.framebuffer_bpp) {
								case 8: {
									multiboot_uint8_t *pixel = (multiboot_uint8_t *)fb + tagfb->common.framebuffer_pitch * i + i;
									*pixel = color;
								} break;
								case 15:
								case 16: {
									multiboot_uint16_t *pixel = (multiboot_uint16_t *)fb + tagfb->common.framebuffer_pitch * i + 2 * i;
									*pixel = color;
								} break;
								case 24: {
									multiboot_uint32_t *pixel = (multiboot_uint32_t *)fb + tagfb->common.framebuffer_pitch * i + 3 * i;
									*pixel = (color & 0xffffff) | (*pixel & 0xff000000);
								} break;

								case 32: {
									multiboot_uint32_t *pixel = (multiboot_uint32_t *)fb + tagfb->common.framebuffer_pitch * i + 4 * i;
									*pixel = color;
								} break;
							}
						}
						break;
				}
			}
		}
	}

	tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7));
	total_printed += sprintf(p, "Total mbi size %u\n", (unsigned)tag - ((uint32_t)_mb2_info));
	while (*p++) {
	}
	p--;
}

static elf32_section_header_t *get_elf_sec_hdr(const char *sec_name) {
	if (!_mb2_info) {
		return NULL;
	}

	struct multiboot_tag_elf_sections *elf =
	    (struct multiboot_tag_elf_sections *)get_multiboot_tag(MULTIBOOT_TAG_TYPE_ELF_SECTIONS);
	if (!elf) {
		return NULL;
	}

	elf32_section_header_t *section_headers = (elf32_section_header_t *)elf->sections;
	uint32_t string_table_start = section_headers[elf->shndx].sh_addr;
	for (uint32_t i = 0; i < elf->num; i++) {
		uint32_t sh_name = section_headers[i].sh_name;
		if (sh_name) {
			char *name = (char *)(string_table_start + sh_name);
			if (strcmp(sec_name, name) == 0) {
				return section_headers + i;
			}
		}
	}

	return NULL;
}

static bool is_loaded() {
	return (uint32_t)_mb2_info;
}

mbiapi_t *mbi_api() {
	return &_api;
}

void mbi_init(mbiapi_t *api) {
	if (api == NULL) {
		_api.get_mmap = get_mmap;
		_api.load_mb2i = load_mb2i;
		_api.print_mb2i = print_mb2i;
		_api.get_elf_sec_hdr = get_elf_sec_hdr;
		_api.get_meminfo = get_meminfo;
		_api.enum_module_info = enum_module_info;
		_api.is_loaded = is_loaded;
		return;
	}

	_api.get_mmap = api->get_mmap;
	_api.load_mb2i = api->load_mb2i;
	_api.print_mb2i = api->print_mb2i;
	_api.get_elf_sec_hdr = api->get_elf_sec_hdr;
	_api.get_meminfo = api->get_meminfo;
	_api.enum_module_info = api->enum_module_info;
	_api.is_loaded = api->is_loaded;
}