/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
#include "exception.h"
#include <hal.h>

extern void kernel_panic(const char *format, ...);

//! divide by 0 fault
void divide_by_zero_fault(uint32_t cs, uint32_t eip, uint32_t eflags) {
	//	intstart ();
	kernel_panic("Divide by 0\nCS: %12.8x\nEIP: %11.8x\nEFLAGS: %.8x", cs, eip, eflags);
	while (1) {
	}
}

//! single step
void single_step_trap(uint32_t cs, uint32_t eip, uint32_t eflags) {
	//	intstart ();
	kernel_panic("Single step\nCS: %12.8x\nEIP: %11.8x\nEFLAGS: %.8x", cs, eip, eflags);
	while (1) {
	}
}

//! non maskable  trap
void nmi_trap(uint32_t cs, uint32_t eip, uint32_t eflags) {
	//	intstart ();
	kernel_panic("NMI trap\nCS: %12.8x\nEIP: %11.8x\nEFLAGS: %.8x", cs, eip, eflags);
	while (1) {
	}
}

//! breakpoint hit
void breakpoint_trap(uint32_t cs, uint32_t eip, uint32_t eflags) {
	//	intstart ();
	kernel_panic("Breakpoint trap\nCS: %12.8x\nEIP: %11.8x\nEFLAGS: %.8x", cs, eip, eflags);
	while (1) {
	}
}

//! overflow
void overflow_trap(uint32_t cs, uint32_t eip, uint32_t eflags) {
	//	intstart ();
	kernel_panic("Overflow trap\nCS: %12.8x\nEIP: %11.8x\nEFLAGS: %.8x", cs, eip, eflags);
	while (1) {
	}
}

//! bounds check
void bounds_check_fault(uint32_t cs, uint32_t eip, uint32_t eflags) {
	//	intstart ();
	kernel_panic("Bounds check fault\nCS: %12.8x\nEIP: %11.8x\nEFLAGS: %.8x", cs, eip, eflags);
	while (1) {
	}
}

//! invalid opcode / instruction
void invalid_opcode_fault(uint32_t cs, uint32_t eip, uint32_t eflags) {
	//	intstart ();
	kernel_panic("Invalid opcode\nCS: %12.8x\nEIP: %11.8x\nEFLAGS: %.8x", cs, eip, eflags);
	while (1) {
	}
}

//! device not available
void no_device_fault(uint32_t cs, uint32_t eip, uint32_t eflags) {
	//	intstart ();
	kernel_panic("Device not found\nCS: %12.8x\nEIP: %11.8x\nEFLAGS: %.8x", cs, eip, eflags);
	while (1) {
	}
}

//! double fault
void double_fault_abort(uint32_t cs, uint32_t err, uint32_t eip, uint32_t eflags) {
	//	intstart ();
	kernel_panic("Double fault\nCS: %12.8x\nError: %9.8x, EIP: %11.8x\nEFLAGS: %.8x", cs, err, eip, eflags);
	while (1) {
	}
}

//! invalid Task State Segment (TSS)
void invalid_tss_fault(uint32_t cs, uint32_t err, uint32_t eip, uint32_t eflags) {
	//	intstart ();
	kernel_panic("Invalid TSS\nCS: %12.8x\nError: %9.8x, EIP: %11.8x\nEFLAGS: %.8x", cs, err, eip, eflags);
	while (1) {
	}
}

//! segment not present
void no_segment_fault(uint32_t cs, uint32_t err, uint32_t eip, uint32_t eflags) {
	//	intstart ();
	kernel_panic("Invalid segment\nCS: %12.8x\nError: %9.8x, EIP: %11.8x\nEFLAGS: %.8x", cs, err, eip, eflags);
	while (1) {
	}
}

//! stack fault
void stack_fault(uint32_t cs, uint32_t err, uint32_t eip, uint32_t eflags) {
	//	intstart ();
	kernel_panic("Stack fault\nCS: %12.8x\nError: %9.8x, EIP: %11.8x\nEFLAGS: %.8x", cs, err, eip, eflags);
	while (1) {
	}
}

//! general protection fault
void general_protection_fault(uint32_t cs, uint32_t err, uint32_t eip, uint32_t eflags) {
	//	intstart ();
	kernel_panic("General Protection Fault\nCS: %12.8x\nError: %9.8x, EIP: %11.8x\nEFLAGS: %.8x", cs, err, eip, eflags);
	while (1) {
	}
}

//! page fault
void page_fault(uint32_t cs, uint32_t err, uint32_t eip, uint32_t eflags) {
	//	intstart ();
	kernel_panic("Page Fault\nCS: %12.8x\nError: %9.8x, EIP: %11.8x\nEFLAGS: %.8x", cs, err, eip, eflags);
	while (1) {
	}
}

//! Floating Point Unit (FPU) error
void fpu_fault(uint32_t cs, uint32_t eip, uint32_t eflags) {
	//	intstart ();
	kernel_panic("FPU Fault\nCS: %12.8x\nEIP: %11.8x\nEFLAGS: %.8x", cs, eip, eflags);
	while (1) {
	}
}

//! alignment check
void alignment_check_fault(uint32_t cs, uint32_t err, uint32_t eip, uint32_t eflags) {
	//	intstart ();
	kernel_panic("Alignment Check\nCS: %12.8x\nError: %9.8x, EIP: %11.8x\nEFLAGS: %.8x", cs, err, eip, eflags);
	while (1) {
	}
}

//! machine check
void machine_check_abort(uint32_t cs, uint32_t eip, uint32_t eflags) {
	//	intstart ();
	kernel_panic("Machine Check\nCS: %12.8x\nEIP: %11.8x\nEFLAGS: %.8x", cs, eip, eflags);
	while (1) {
	}
}

//! Floating Point Unit (FPU) Single Instruction Multiple Data (SIMD) error
void simd_fpu_fault(uint32_t cs, uint32_t eip, uint32_t eflags) {
	//	intstart ();
	kernel_panic("FPU SIMD fault\nCS: %12.8x\nEIP: %11.8x\nEFLAGS: %.8x", cs, eip, eflags);
	while (1) {
	}
}
