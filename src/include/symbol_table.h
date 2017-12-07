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