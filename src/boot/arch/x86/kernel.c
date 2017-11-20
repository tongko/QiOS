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

#include "boot.h"
#include "multiboot2.h"

    static const unsigned char VERSION[] = "SHOS Version 0.0.1";
static const unsigned char COPYRIGHT[] = "Sin Hing 2018 all rights reserved";

static void _clear_screen() {
	_set_color(2, 0);  //	Green foreground, black background
	_clear();
}

/*  Check if MAGIC is valid and print the Multiboot information structure pointed by ADDR. */
void _kernel_main(uint32_t magic, uint32_t mbi_addr) {
	// Initialize screen
	_enable_cursor(0, 15);  // Block shape cursor
	_clear_screen();

	//	Am I booted by a Multiboot-compliant boot loader?
	if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
		_printf("Invalid magic number: 0x%x\n", magic);
		return;
	}

	if (mbi_addr & 7) {
		_printf("Unaligned MBI: 0x%x\n", mbi_addr);
		return;
	}

	_printf("%s\n%s\n\n", VERSION, COPYRIGHT);

	print_multiboot_info(mbi_addr);
}