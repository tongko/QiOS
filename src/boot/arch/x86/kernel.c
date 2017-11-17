/* Check if the compiler thinks we are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */

#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

#include "boot.h"
#include "multiboot2.h"

    static const unsigned char VERSION[] = "SHOS Version 0.0.1";
static const unsigned char COPYRIGHT[] = "Sin Hing 2018 all rights reserved";

static void _clear_screen() {
	_set_color(2, 0);  //	Green foreground, black background
	_clear();
}

/*  Check if MAGIC is valid and print the Multiboot information structure pointed by ADDR. */
void _kernel_main(uint32_t magic, uint32_t mbi_addr) {
	struct multiboot_tag *tag;
	uint32_t size;

	// Initialize screen
	_enable_cursor(0, 15);  // Block shape cursor
	_clear_screen();

	//	Am I booted by a Multiboot-compliant boot loader?
	if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
		_printf("Invalid magic number: 0x%x\n", magic);
		return;
	}

	if (mbi_addr & 7) {
		_printf("Unaligned MBI: 0x%x\n", mbi_addr);
		return;
	}

	_printf("%s\n%s\n", VERSION, COPYRIGHT);

	size = *(uint32_t *)mbi_addr;
	_printf("Announced mbi size 0x%x\n", size);
	for (tag = (struct multiboot_tag *)(mbi_addr + 8);
	     tag->type != MULTIBOOT_TAG_TYPE_END;
	     tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7))) {
		_printf("Tag 0x%x, Size 0x%x\n", tag->type, tag->size);
		switch (tag->type) {
			case MULTIBOOT_TAG_TYPE_CMDLINE:
				_printf("Command line = %s\n",
				        ((struct multiboot_tag_string *)tag)->string);
				break;
			case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
				_printf("Boot loader name = %s\n",
				        ((struct multiboot_tag_string *)tag)->string);
				break;
			case MULTIBOOT_TAG_TYPE_MODULE:
				_printf("Module at 0x%x-0x%x. Command line %s\n",
				        ((struct multiboot_tag_module *)tag)->mod_start,
				        ((struct multiboot_tag_module *)tag)->mod_end,
				        ((struct multiboot_tag_module *)tag)->cmdline);
				break;
			case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
				_printf("mem_lower = %uKB, mem_upper = %uKB\n",
				        ((struct multiboot_tag_basic_meminfo *)tag)->mem_lower,
				        ((struct multiboot_tag_basic_meminfo *)tag)->mem_upper);
				break;
			case MULTIBOOT_TAG_TYPE_BOOTDEV:
				_printf("Boot device 0x%x,%u,%u\n",
				        ((struct multiboot_tag_bootdev *)tag)->biosdev,
				        ((struct multiboot_tag_bootdev *)tag)->slice,
				        ((struct multiboot_tag_bootdev *)tag)->part);
				break;
			case MULTIBOOT_TAG_TYPE_MMAP: {
				multiboot_memory_map_t *mmap;

				_printf("mmap\n");

				for (mmap = ((struct multiboot_tag_mmap *)tag)->entries;
				     (multiboot_uint8_t *)mmap < (multiboot_uint8_t *)tag + tag->size;
				     mmap = (multiboot_memory_map_t *)((unsigned long)mmap + ((struct multiboot_tag_mmap *)tag)->entry_size))
					_printf(
					    " base_addr = 0x%x%x,"
					    " length = 0x%x%x, type = 0x%x\n",
					    (uint32_t)(mmap->addr >> 32),
					    (uint32_t)(mmap->addr & 0xffffffff),
					    (uint32_t)(mmap->len >> 32),
					    (uint32_t)(mmap->len & 0xffffffff),
					    (uint32_t)mmap->type);
				break;
			}
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
	_printf("Total mbi size 0x%x\n", (unsigned)tag - mbi_addr);
}