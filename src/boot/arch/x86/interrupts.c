#include "interrupts.h"
#include "multiboot.h"
#include "paging.h"
#include "serial_port.h"
#include "symbol_table.h"

static void log_stack_trace_line(uint32_t eip) {
	char *symbol_name = address_to_symbol_name(eip);
	serial_print("%x : %s\n", eip, symbol_name);
}

static void log_interrupt_details(char *int_name, uint32_t error_code, uint32_t eip, CPU_STATE *cpu) {
	if (cpu->ebp) {
	}
	serial_print("--------------------\n");

	serial_print("Interrupt: %s\n", int_name);
	serial_print("error_code: %x\n", error_code);

	serial_print("\nStack trace:\n");
	eip -= 4;  // eip actually points one past the instruction that triggered interrupt
	log_stack_trace_line(eip);
	uint32_t ebp = cpu->ebp;
	uint32_t kernel_stack_highest_address = ((uint32_t)&kernel_stack_bottom + STACK_SIZE - 4);
	while (ebp <= kernel_stack_highest_address && ebp >= ((uint32_t)&kernel_stack_bottom)) {
		eip = ((uint32_t *)ebp)[1];
		log_stack_trace_line(eip);

		ebp = *((uint32_t *)ebp);
	}

	serial_print("--------------------\n");
}

uint32_t interrupt_handler(CPU_STATE cpu, uint32_t interrupt_number, uint32_t error_code, uint32_t eip) {
	switch (interrupt_number) {
		case (INT_KEYBOARD):
			//keyboard_interrupt_handler();
			serial_print("Keyboard int.\n");
			break;
		case (INT_PAGE_FAULT):
			if ((error_code & 0b1) == 0) {
				// Caused by page-not-present
				page_in((void *)cpu.cr2);
				break;
			}
			log_interrupt_details("INT_PAGE_FAULT", error_code, eip, &cpu);
			serial_print("Interrupt was a page fault. Here's what I know:\n");
			serial_print("- Tried to access virtual address %x\n", cpu.cr2);
			if (error_code & 0b1) {
				serial_print("- Couldn't complete because of page-protection violation\n");
			} else {
				serial_print("- Couldn't complete because page was not present\n");
			}
			if (error_code & 0b10) {
				serial_print("- This was an attempt to WRITE to this address.\n");
			} else {
				serial_print("- This was an attempt to READ from this address.\n");
			}
			if (error_code & 0b100) {
				serial_print("- Memory access came from user.\n");
			} else {
				serial_print("- Memory access came from kernel.\n");
			}
			if (error_code & 0b1000) {
				serial_print("- caused by reading a 1 in a reserved field.\n");
			}
			if (error_code & 0b10000) {
				serial_print("- caused by an instruction fetch.\n");
			}
			while (1) {
			}
			break;

		case (INT_SOFTWARE):
			log_interrupt_details("INT_SOFTWARE", error_code, eip, &cpu);
			break;

		case (INT_SYSCALL):
			serial_print("Handle sys call.");
			return 0;
		case (INT_OUT_OF_MEMORY):
			log_interrupt_details("INT_OUT_OF_MEMORY", error_code, eip, &cpu);
			while (1) {
			}
			break;

		default:
			serial_print("ERROR: Unable to handle interrupt: %x\n", interrupt_number);
			log_interrupt_details("INT_UNKNOWN", error_code, eip, &cpu);
			while (1) {
			}
			break;
	}

	// pic_acknowledge();
	return 0;
}