/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
#ifndef	__EXCEPTION_H_
#define __EXCEPTION_H_

#include <stdint.h>

//! divide by 0
extern void divide_by_zero_fault (uint32_t cs, uint32_t eip, uint32_t eflags);

//! single step
extern void single_step_trap (uint32_t cs, uint32_t eip, uint32_t eflags);

//! non maskable interrupt trap
extern void nmi_trap (uint32_t cs, uint32_t eip, uint32_t eflags);

//! breakpoint hit
extern void breakpoint_trap (uint32_t cs, uint32_t eip, uint32_t eflags);

//! overflow
extern void overflow_trap (uint32_t cs, uint32_t eip, uint32_t eflags);

//! bounds check
extern void bounds_check_fault (uint32_t cs, uint32_t eip, uint32_t eflags);

//! invalid opcode / instruction
extern void invalid_opcode_fault (uint32_t cs, uint32_t eip, uint32_t eflags);

//! device not available
extern void no_device_fault (uint32_t cs, uint32_t eip, uint32_t eflags);

//! double fault
extern void double_fault_abort (uint32_t cs, uint32_t err, uint32_t eip, uint32_t eflags);

//! invalid Task State Segment (TSS)
extern void invalid_tss_fault (uint32_t cs, uint32_t err,uint32_t eip, uint32_t eflags);

//! segment not present
extern void no_segment_fault (uint32_t cs, uint32_t err,uint32_t eip, uint32_t eflags);

//! stack fault
extern void stack_fault (uint32_t cs, uint32_t err, uint32_t eip, uint32_t eflags);

//! general protection fault
extern void general_protection_fault (uint32_t cs, uint32_t err, uint32_t eip, uint32_t eflags);

//! page fault
extern void page_fault (uint32_t cs, uint32_t err, uint32_t eip, uint32_t eflags);

//! Floating Point Unit (FPU) error
extern void fpu_fault (uint32_t cs, uint32_t eip, uint32_t eflags);

//! alignment check
extern void alignment_check_fault (uint32_t cs, uint32_t err,uint32_t eip, uint32_t eflags);

//! machine check
extern void machine_check_abort (uint32_t cs, uint32_t eip, uint32_t eflags);

//! Floating Point Unit (FPU) Single Instruction Multiple Data (SIMD) error
extern void simd_fpu_fault (uint32_t cs, uint32_t eip, uint32_t eflags);

#endif	//	__EXCEPTION_H_