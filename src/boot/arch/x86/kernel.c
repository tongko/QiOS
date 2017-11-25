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

#include "gdt.h"
#include "idt.h"
#include "kconsole.h"
#include "multiboot_info.h"
#include "paging.h"
#include "serial_port.h"
#include "symbol_table.h"

    static const unsigned char VERSION[] = "SHOS Version 0.0.1";
static const unsigned char COPYRIGHT[] = "Sin Hing 2018 all rights reserved";

static void test_interrupt() {
	__asm__(
	    "int 49;\n");
}

static void initilize_console() {
	cursor_shape(0, 15);
	CONSOLECOLOR cc = {2, 0};
	init_console(&cc);
	set_console_color(&cc);
	clear();
}

/*  Check if MAGIC is valid and print the Multiboot information structure pointed by ADDR. */
void _kernel_main(struct kmemory_descriptor kernel_memory,
                  uint32_t magic,
                  uint32_t mbi_addr) {
	// Load multiboot2 info table
	load_multiboot2_info(mbi_addr);
	// Initialize screen
	initilize_console();
	//	Initialize serial port for debugging
	init_serial_port();

	serial_print("Kernel loaded at: 0x%x.16", kernel_memory);
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

	serial_print("Loading kernel symbol table... ");
	if (load_symbol_table(get_elf_section_header(".symtab"), get_elf_section_header(".strtab"))) {
		serial_print("[OK]\n");
	} else {
		serial_print("[Error] loading symbol table.");
		return;
	}

	//print_multiboot_info(mbi_addr);
	serial_print("Initializing GDT... ");
	init_gdt();
	serial_print("[OK]\n");

	serial_print("Initializing IDT... ");
	init_idt();
	serial_print("[OK]\n");

	serial_print("Issuing test interrupt... ");
	test_interrupt();
	serial_print("[OK]\n");

	serial_print("Initializing PIC controller... ");
	init_pic();
	serial_print("[OK]\n");

	serial_print("Initializing page allocator... ");
	uint32_t free_count = init_page_allocator(kernel_memory);
	serial_print("[OK]\n");
	serial_print("%d free pages (%d MiB)\n", free_count, free_count / 256);
	write_console("Available free memory: %d MiB (%d Pages)", free_count / 256, free_count);
}