/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 * ****************************************************************************/
#ifndef __SYMBOL_TABLE_H_
#define __SYMBOL_TABLE_H_

#include <stdint.h>

#include <stdbool.h>

#include <elf32.h>

typedef struct {
	bool present;
	uint32_t count;
	elf32_symbol_t *symbols;
	char *string_table_addr;
} symbol_table_descriptor_t;

typedef struct {
	bool (*load_symbol_table)(elf32_section_header_t *symb_tab,
	                          elf32_section_header_t *string_table);
	char *(*addr_to_symb_name)(uint32_t symb_addr);
} symbol_api_t;

symbol_api_t *symbol_table_api(void);
bool symb_default_config(void);
void init_symb_tab(symbol_api_t *api);

#endif  //	__SYMBOL_TABLE_H_