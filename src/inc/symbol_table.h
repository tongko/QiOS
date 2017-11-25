#ifndef __SYMBOL_TABLE_H_
#define __SYMBOL_TABLE_H_

#include <stdbool.h>
#include <stdint.h>
#include "elf32.h"

typedef struct symbol_table_descriptor {
	bool present;
	uint32_t count;
	elf32_symbol_t *symbols;
	char *string_table_addr;
} symbol_table_descriptor_t;

bool load_symbol_table(elf32_section_header_t *symbol_table,
                       elf32_section_header_t *string_table);
char *address_to_symbol_name(uint32_t symbol_addr);

#endif  //	__SYMBOL_TABLE_H_