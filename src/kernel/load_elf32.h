/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 * ****************************************************************************/
#ifndef __LOAD_ELF32_H_
#define __LOAD_ELF32_H_

#include <elf32.h>
#include <multiboot/multiboot2.h>

/* Verify ELF32 header */
void load_elf32_verify_header(elf32_ehdr_t *hdr);
/* Load ELF 32 bits binaries from multiboot2 info */

#endif  //	__LOAD_ELF32_H_