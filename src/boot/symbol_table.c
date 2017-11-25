#include "symbol_table.h"
#include "paging.h"

symbol_table_descriptor_t symbol_table_descriptor;

bool load_symbol_table(elf32_section_header_t *symbol_table,
                       elf32_section_header_t *string_table) {
	if (symbol_table == 0) {
		symbol_table_descriptor.present = false;
		return false;
	} else {
		symbol_table_descriptor.present = true;
		symbol_table_descriptor.count = symbol_table->sh_size / sizeof(elf32_symbol_t);
		symbol_table_descriptor.symbols = (elf32_symbol_t *)physical_to_virtual(symbol_table->sh_addr);
		symbol_table_descriptor.string_table_addr = (char *)physical_to_virtual(string_table->sh_addr);
		return true;
	}
}

char *address_to_symbol_name(uint32_t addr) {
	// Algorithm : Find symbol with greatest value <= address

	elf32_symbol_t *symbol = 0;
	uint32_t symbol_value = 0;

	if (symbol_table_descriptor.present) {
		for (uint32_t i = 0; i < symbol_table_descriptor.count; i++) {
			elf32_symbol_t *candidate = symbol_table_descriptor.symbols + i;
			if (candidate->st_value > symbol_value && candidate->st_value <= addr) {
				symbol = candidate;
				symbol_value = candidate->st_value;
			}
		}

		uint32_t string_index = symbol->st_name;
		return symbol_table_descriptor.string_table_addr + string_index;
	}

	return 0;
}