/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 * ****************************************************************************/
#include <asm.h>
#include <init/init.h>
#include <kio.h>
#include <sys/panic.h>
#include <sys/serial_port.h>
#include <sys/term.h>

void early_init(kernel_meminfo_t k_meminfo, mbi_t mbi) {
	//	Ensure all hardware interrupt are stop
	cli();

	//	Initialize terminal with default api implementation.
	term_init(NULL);
	term_default_config();
	//	Initialize serial port for debugging.
	serial_port_init(NULL);
	serial_port_default_config();
	serial_port_api_t *spapi = serial_port_api();

	printf("Performing early initialization...\n");
	spapi->print("Performing early initialization...\n");

	printf("Initializing multiboot info... ");
	spapi->print("Initializing multiboot info... ");
	mbi_init(NULL);
	printf("[OK]\n");
	spapi->print("[OK]\n");

	//	Multiboot Info
	mbiapi_t *mbiapi = mbi_api();
	if (!mbiapi->verify_magic(mbi.magic)) {
		kernel_panic("Invalid multiboot2 magic number '%#x'.", mbi.magic);
	}
	if (!mbi.addr) {
		kernel_panic("Invalid multiboot2 information pointer '%#x'.", mbi.addr);
	}
	mbiapi->load_mb2i(mbi.addr);

	//	Memory management
	printf("Initializing memory management... ");
	spapi->print("Initializing memory management... ");
	mm_init(k_meminfo);
	printf("[OK]\n");
	spapi->print("[OK]\n");
}