#ifndef __ELF32_H_
#define __ELF32_H_

#include <stdint.h>

typedef struct elf32_section_header {
	uint32_t sh_name;
	uint32_t sh_type;
	uint32_t sh_flags;
	uint32_t sh_addr;
	uint32_t sh_offset;
	uint32_t sh_size;
	uint32_t sh_link;
	uint32_t sh_info;
	uint32_t sh_addralign;
	uint32_t sh_entsize;
} elf32_section_header_t;

typedef struct elf32_symbol {
	uint32_t st_name;
	uint32_t st_value;
	uint32_t st_size;
	uint8_t st_info;
	uint8_t st_other;
	uint16_t st_shndx;
} elf32_symbol_t;

#endif  //	__ELF32_H_