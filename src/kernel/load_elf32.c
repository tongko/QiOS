/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 * ****************************************************************************/
#include "load_elf32.h"
#include <elf32.h>
#include <mem/memory.h>
#include <sys/panic.h>
#include <sys/sysarch.h>

/* Verify the given header structure against ELF32 standard */
void load_elf32_verify_header(elf32_ehdr_t *hdr) {
	if (hdr) {
		kernel_panic("load_elf32: Invalid header structure specified.\n");
	}

	if (!(hdr->e_ident[EI_MAG0] == ELFMAG0 &&
	      hdr->e_ident[EI_MAG1] == ELFMAG1 &&
	      hdr->e_ident[EI_MAG2] == ELFMAG2 &&
	      hdr->e_ident[EI_MAG3] == ELFMAG3)) {
		kernel_panic("load_elf32: binary is not in sound ELF structure.");
	}

	if (hdr->e_type != ET_EXEC) {
		kernel_panic("load_elf32: binary is not executable.");
	}

	if (hdr->e_machine != _ELF_TARGET_ARCH) {
		kernel_panic("load-elf32: binary target architecture is not i386.");
	}

	if (hdr->e_ident[EI_CLASS] != ELFCLASS32) {
		kernel_panic("load-elf32: binary's word size is not 32 bits.");
	}

	if (hdr->e_ident[EI_DATA] != ELFDATA2LSB) {
		kernel_panic("load-elf32: binary does not have little endian encoding.");
	}

	if (hdr->e_version != EV_CURRENT) {
		kernel_panic("load-elf32: binary is not standard ELF.");
	}
}