#include "multiboot_info.h"
#include "elf32.h"
#include "kconsole.h"
#include "kstring.h"
#include "multiboot2.h"
#define _NEED_NULL
#include "ktypedef.h"
#include "paging.h"
#include "serial_port.h"

static void *multiboot2_info;

static struct multiboot_tag *get_multiboot_tag(uint32_t type) {
	struct multiboot_tag *tag;
	for (tag = (struct multiboot_tag *)(multiboot2_info + 8);
	     tag->type != MULTIBOOT_TAG_TYPE_END;
	     tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7))) {
		if (tag->type == type) {
			return tag;
		}
	}

	return NULL;
}

void load_multiboot2_info(uint32_t mb2i_addr) {
	if (mb2i_addr) {
		multiboot2_info = (void *)mb2i_addr;
	}
}

elf32_section_header_t *get_elf_section_header(const char *section_name) {
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

multiboot_tag_mmap_t *get_mmap_table(void) {
	return (multiboot_tag_mmap_t *)get_multiboot_tag(MULTIBOOT_TAG_TYPE_MMAP);
}

void print_multiboot_info() {
	uint32_t size = *(uint32_t *)multiboot2_info;
	write_console("Announced mbi size %d B\n", size);

	struct multiboot_tag *tag;
	for (tag = (struct multiboot_tag *)(multiboot2_info + 8);
	     tag->type != MULTIBOOT_TAG_TYPE_END;
	     tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7))) {
		write_console("Tag 0x%x.2, Size 0x%x.16\n", tag->type, tag->size);
		switch (tag->type) {
			case MULTIBOOT_TAG_TYPE_CMDLINE:
				write_console("-> Command line = %s\n",
				              ((struct multiboot_tag_string *)tag)->string);
				break;
			case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
				write_console("-> Boot loader name = %s\n",
				              ((struct multiboot_tag_string *)tag)->string);
				break;
			case MULTIBOOT_TAG_TYPE_MODULE:
				write_console("-> Module at 0x%x.16-0x%x.16. Command line %s\n",
				              ((struct multiboot_tag_module *)tag)->mod_start,
				              ((struct multiboot_tag_module *)tag)->mod_end,
				              ((struct multiboot_tag_module *)tag)->cmdline);
				break;
			case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
				write_console("-> mem_lower = %uKiB, mem_upper = %uKiB\n",
				              ((struct multiboot_tag_basic_meminfo *)tag)->mem_lower,
				              ((struct multiboot_tag_basic_meminfo *)tag)->mem_upper);
				break;
			case MULTIBOOT_TAG_TYPE_BOOTDEV:
				write_console("-> Boot device 0x%x.4, slice: %u, part: %u\n",
				              ((struct multiboot_tag_bootdev *)tag)->biosdev,
				              ((struct multiboot_tag_bootdev *)tag)->slice,
				              ((struct multiboot_tag_bootdev *)tag)->part);
				break;
			case MULTIBOOT_TAG_TYPE_MMAP: {
				multiboot_memory_map_t *mmap;

				write_console("-> Memory Map\n");
				// int32_t count = 0;
				for (mmap = ((struct multiboot_tag_mmap *)tag)->entries;
				     (multiboot_uint8_t *)mmap < (multiboot_uint8_t *)tag + tag->size;
				     mmap = (multiboot_memory_map_t *)((unsigned long)mmap + ((struct multiboot_tag_mmap *)tag)->entry_size)) {
					write_console(
					    //	For 32 bits only 32 bit address exists.
					    //"\tbase_addr = 0x%x%x, length = 0x%x%x, type = 0x%x\n",
					    "\tbase_addr = 0x%x.16, length=0x%x.16, type = 0x%x.2\n",
					    (uint32_t)(mmap->addr & 0xffffffff),
					    (uint32_t)(mmap->len & 0xffffffff),
					    (uint32_t)mmap->type);
					// count++;
					// MEMORY_MAP_ENTRY entry = {0};
					// entry.entry_start = mmap->addr & 0xffffffff;
					// uint32_t len = mmap->len & 0xffffffff;
					// entry.entry_end = entry.entry_start + len;
					// entry.length = len;
					// entry.type = mmap->type;
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
	write_console("Total mbi size 0x%x\n\n", (unsigned)tag - ((uint32_t)multiboot2_info));
}