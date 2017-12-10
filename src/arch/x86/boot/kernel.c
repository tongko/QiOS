/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************
 * 																			   *
 *  kernel.c
 **
 *  Everything about kernel.
 **
 * 																			   *
 * ---------------------------------------------------------------------------*/
/* Check if the compiler thinks we are targeting the wrong operating system. */
#ifdef __linux__
#error \
    "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */

#ifndef __i386__
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

#include <sys/version.h>
#include "boot.h"

#define serial serial_port_api
#define mbiapi mbi_info_api

void _kmain(uint32_t magic, uint32_t mbi_addr) {
	init_term(NULL);          // use default term implementation
	term_default_config();    // default configuration
	init_serial_port(NULL);   // use default serial_port implementation
	serial_default_config();  // default configuration

	if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
		serial().print("Invalid magic number: %#.8x\n", magic);
		printf("Invalid magic number: %#.8x\n", magic);
		return;
	}

	printf("%s\n%s\n\n", WELCOME_STR, COPYRIGHT);

	serial().print("Initialize GDT... ");
	init_gdt();
	serial().print("[OK]\n");

	char buffer[4096];
	init_mb_info(NULL);
	mbiapi().load_mb2i(mbi_addr);
	mbiapi().print_mb2i(buffer);

	serial().puts(buffer, (int32_t)strlen(buffer));
	printf("\n%s", buffer);

	while (1) {
	}
}