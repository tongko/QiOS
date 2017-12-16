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
#define symbol symbol_table_api

char *logo =
    "\
  ____    _ ____  ____\n\
 / __ \\  (_) __ \\/ __/\n\
/ /_/ / / / /_/ /\\ \\    Very Simple Operating System\n\
\\___\\_\\/_/\\____/___/\n\n";

void _kmain(uint32_t magic, uint32_t mbi_addr) {
	init_mb_info(NULL);
	mbiapi()->load_mb2i(mbi_addr);
	init_term(NULL);          // use default term implementation
	term_default_config();    // default configuration
	init_serial_port(NULL);   // use default serial_port implementation
	serial_default_config();  // default configuration

	if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
		serial()->print("Invalid magic number: %#.8x\n", magic);
		printf("Invalid magic number: %#.8x\n", magic);
		return;
	}

	printf("%s\n%s\n\n", WELCOME_STR, COPYRIGHT);

	serial()->print("Loading kernel symbol table... ");
	init_symb_tab(NULL);
	if (symb_default_config()) {
		serial()->print("[OK]\n");
	} else {
		serial()->print("\n[ERROR] Error loading kernel's symbols table...\n");
		printf("Error loading kernel's symbols table...");
		return;
	}

	hal_init();

	//! install our exception handlers
	intr_set_vect(0, divide_by_zero_fault);
	intr_set_vect(1, single_step_trap);
	intr_set_vect(2, nmi_trap);
	intr_set_vect(3, breakpoint_trap);
	intr_set_vect(4, overflow_trap);
	intr_set_vect(5, bounds_check_fault);
	intr_set_vect(6, invalid_opcode_fault);
	intr_set_vect(7, no_device_fault);
	intr_set_vect(8, double_fault_abort);
	intr_set_vect(10, invalid_tss_fault);
	intr_set_vect(11, no_segment_fault);
	intr_set_vect(12, stack_fault);
	intr_set_vect(13, general_protection_fault);
	intr_set_vect(14, page_fault);
	intr_set_vect(16, fpu_fault);
	intr_set_vect(17, alignment_check_fault);
	intr_set_vect(18, machine_check_abort);
	intr_set_vect(19, simd_fpu_fault);

	sti();

	term_api()->clear();
	cursor_point_t cxy = {0, 0};
	term_api()->set_cursor_point(&cxy);
	term_color_t color = {2, 0};
	term_api()->set_color(&color);

	printf("%s", logo);
	printf("The PIC, PIT, and exception handlers are installed!\n\n");
	printf("Hitting any key will fire the default HAL handlers as we do\n");
	printf("not have a keyboard driver yet to install one.\n\n");
	printf("CPU: %s\n\n", get_cpu_vender());

	while (1) {
		cxy.x = 0;
		cxy.y = 14;
		term_api()->set_cursor_point(&cxy);
		printf("Current tick count: %i", get_tick_count());
	}

	// serial()->print("Initializing GDT... ");
	// init_gdt();
	// serial()->print("[OK]\n");

	// serial()->print("Initializing IDT... ");
	// init_idt();
	// serial()->print("[OK]\n");

	// char buffer[4096];
	// mbiapi()->print_mb2i(buffer);

	// serial()->puts(buffer, (int32_t)strlen(buffer));
	// printf("\n%s", buffer);

	while (1) {
	}
}