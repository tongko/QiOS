#ifndef __MULTIBOOT_H_
#define __MULTIBOOT_H_

#include <elf32.h>
#include <multiboot/multiboot2.h>
#include <stdint.h>

multiboot_tag_mmap_t *get_mmap_table(void);
void load_multiboot2_info(uint32_t mb2_addr);
void print_multiboot_info(void);
elf32_section_header_t *get_elf_section_header(const char *section_name);

#endif  //	__MULTIBOOT_H_