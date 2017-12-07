/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
#include <multiboot/mb_info.h>
#include <stddef.h>
#include <stdio.h>

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
		mbiapi.get_mmap = get_mmap;
		mbiapi.load_mb2i = load_mb2i;
		mbiapi.print_mb2i = print_mb2i;
		mbiapi.get_elf_sec_hdr = get_elf_sec_hdr;
		return;
	}

	mbiapi.get_mmap = api->get_mmap;
	mbiapi.load_mb2i = api->load_mb2i;
	mbiapi.print_mb2i = api->print_mb2i;
	mbiapi.get_elf_sec_hdr = api->get_elf_sec_hdr;
}

void load_mb2i(uint32_t mbi_addr) {
	if (mbi_addr) {
		if (mbi_addr & 7) {
			printf("Unaligned MBI: 0x%x.8\n", mbi_addr);
			while (1) {
			}
		}

		mb2_info = (void *)mbi_addr;
	}
}

multiboot_tag_mmap_t *get_mmap(void) {
	return (multiboot_tag_mmap_t *)get_multiboot_tag(MULTIBOOT_TAG_TYPE_MMAP);
}

void print_mb2i(char *buffer) {
	if (!buffer) {
		return;
	}

	uint32_t size = *(uint32_t *)mb2_info;
	sprintf(buffer, "Announced mbi size %10d B\n", size);

	struct multiboot_tag *tag;
	for (tag = (struct multiboot_tag *)(mb2_info + 8);
	     tag->type != MULTIBOOT_TAG_TYPE_END;
	     tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7))) {
		sprintf(buffer, "Tag %#.2x, Size %.16x\n", tag->type, tag->size);
		switch (tag->type) {
			case MULTIBOOT_TAG_TYPE_CMDLINE:
				sprintf(buffer, "-> Command line = %s\n",
				        ((struct multiboot_tag_string *)tag)->string);
				break;
			case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
				sprintf(buffer, "-> Boot loader name = %s\n",
				        ((struct multiboot_tag_string *)tag)->string);
				break;
			case MULTIBOOT_TAG_TYPE_MODULE:
				sprintf(buffer, "-> Module at %#.16x-%#.16x. Command line %s\n",
				        ((struct multiboot_tag_module *)tag)->mod_start,
				        ((struct multiboot_tag_module *)tag)->mod_end,
				        ((struct multiboot_tag_module *)tag)->cmdline);
				break;
			case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
				sprintf(buffer, "-> mem_lower = %uKiB, mem_upper = %uKiB\n",
				        ((struct multiboot_tag_basic_meminfo *)tag)->mem_lower,
				        ((struct multiboot_tag_basic_meminfo *)tag)->mem_upper);
				break;
			case MULTIBOOT_TAG_TYPE_BOOTDEV:
				sprintf(buffer, "-> Boot device %#.4x, slice: %u, part: %u\n",
				        ((struct multiboot_tag_bootdev *)tag)->biosdev,
				        ((struct multiboot_tag_bootdev *)tag)->slice,
				        ((struct multiboot_tag_bootdev *)tag)->part);
				break;
			case MULTIBOOT_TAG_TYPE_MMAP: {
				multiboot_memory_map_t *mmap;

				sprintf(buffer, "-> Memory Map\n");
				// int32_t count = 0;
				for (mmap = ((struct multiboot_tag_mmap *)tag)->entries;
				     (multiboot_uint8_t *)mmap < (multiboot_uint8_t *)tag + tag->size;
				     mmap = (multiboot_memory_map_t *)((unsigned long)mmap + ((struct multiboot_tag_mmap *)tag)->entry_size)) {
					sprintf(buffer,
					        //	For 32 bits only 32 bit address exists.
					        //"\tbase_addr = 0x%x%x, length = 0x%x%x, type = 0x%x\n",
					        "\tbase_addr = %#.16x, length=%#.16x, type = %#.2x\n",
					        (uint32_t)(mmap->addr & 0xffffffff),
					        (uint32_t)(mmap->len & 0xffffffff),
					        (uint32_t)mmap->type);
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
									multiboot_uint8_t *pixel = fb + tagfb->common.framebuffer_pitch * i + i;
									*pixel = color;
								} break;
								case 15:
								case 16: {
									multiboot_uint16_t *pixel = fb + tagfb->common.framebuffer_pitch * i + 2 * i;
									*pixel = color;
								} break;
								case 24: {
									multiboot_uint32_t *pixel = fb + tagfb->common.framebuffer_pitch * i + 3 * i;
									*pixel = (color & 0xffffff) | (*pixel & 0xff000000);
								} break;

								case 32: {
									multiboot_uint32_t *pixel = fb + tagfb->common.framebuffer_pitch * i + 4 * i;
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
	sprintf(buffer, "Total mbi size %u\n\n", (unsigned)tag - ((uint32_t)mb2_info));
}

elf32_section_header_t *get_elf_sec_hdr(const char *sec_name) {
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
			if (_strcmp(sec_name, name) == 0) {
				return section_headers + i;
			}
		}
	}

	return NULL;
}