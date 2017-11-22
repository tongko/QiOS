/* Check if the compiler thinks we are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */

#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

#define __KERNEL_

#include "boot.h"
#include "kconsole.h"
#include "multiboot2.h"

    static const unsigned char VERSION[] = "SHOS Version 0.0.1";
static const unsigned char COPYRIGHT[] = "Sin Hing 2018 all rights reserved";

static void initilize_console() {
	cursor_shape(0, 15);
	CONSOLECOLOR cc = {2, 0};
	init_console(&cc);
	set_console_color(&cc);
	clear();
}

/*  Check if MAGIC is valid and print the Multiboot information structure pointed by ADDR. */
void _kernel_main(uint32_t magic, uint32_t mbi_addr) {
	// Initialize screen
	initilize_console();

	//	Am I booted by a Multiboot-compliant boot loader?
	if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
		write_console("Invalid magic number: 0x%x.8\n", magic);
		return;
	}

	if (mbi_addr & 7) {
		write_console("Unaligned MBI: 0x%x.8\n", mbi_addr);
		return;
	}

	write_console("%s\n%s\n\n", VERSION, COPYRIGHT);

	print_multiboot_info(mbi_addr);
}