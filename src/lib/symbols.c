/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
#include <multiboot/mb_info.h>
#include <sys/symbols.h>

symbol_api_t _api;
symbol_table_descriptor_t _symb_tab_desc;

static bool load_symbol_table(elf32_section_header_t *symbol_table,
                              elf32_section_header_t *string_table) {
	if (symbol_table == 0) {
		_symb_tab_desc.present = false;
		return false;
	} else {
		_symb_tab_desc.present = true;
		_symb_tab_desc.count = symbol_table->sh_size / sizeof(elf32_symbol_t);
		_symb_tab_desc.symbols = (elf32_symbol_t *)(symbol_table->sh_addr);
		_symb_tab_desc.string_table_addr = (char *)(string_table->sh_addr);
		return true;
	}
}

static char *address_to_symbol_name(uint32_t addr) {
	// Algorithm : Find symbol with greatest value <= address

	elf32_symbol_t *symbol = 0;
	uint32_t symbol_value = 0;

	if (_symb_tab_desc.present) {
		for (uint32_t i = 0; i < _symb_tab_desc.count; i++) {
			elf32_symbol_t *candidate = _symb_tab_desc.symbols + i;
			if (candidate->st_value > symbol_value && candidate->st_value <= addr) {
				symbol = candidate;
				symbol_value = candidate->st_value;
			}
		}

		uint32_t string_index = symbol->st_name;
		return _symb_tab_desc.string_table_addr + string_index;
	}

	return 0;
}

bool symb_default_config(void) {
	return _api.load_symbol_table(mbi_info_api()->get_elf_sec_hdr(".symtab"),
	                              mbi_info_api()->get_elf_sec_hdr(".strtab"));
}

void init_symb_tab(symbol_api_t *api) {
	if (api) {
		_api.load_symbol_table = api->load_symbol_table;
		_api.addr_to_symb_name = api->addr_to_symb_name;
		return;
	}

	_api.load_symbol_table = load_symbol_table;
	_api.addr_to_symb_name = address_to_symbol_name;
}