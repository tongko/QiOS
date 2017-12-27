/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 * ****************************************************************************/
#ifndef __ASM_H_
#define __ASM_H_

#include <attribs.h>
#include <stdint.h>

#define _asm(x...) asm volatile(x)

static __inline__ void sti(void) {
	_asm(
	    "sti\n"
	    "nop\n");
}

static __inline__ void cli(void) {
	_asm(
	    "cli\n"
	    "nop\n");
}

static __inline__ void lidt(void *idt) {
	_asm(
	    "mov eax, %0\n"
	    "lidt [eax]\n" ::"r"(idt));
}

static __inline__ uint8_t _inb(uint16_t port) {
	uint8_t ret_val;
	_asm("in %0,%1"
	     : "=a"(ret_val)
	     : "d"(port));
	return ret_val;
}

static __inline__ void _outb(uint16_t port, uint8_t val) {
	_asm("out %1,%0" ::"a"(val), "d"(port));
}

static __inline__ void io_wait(void) {
	/* Port 0x80 is used for 'checkpoints' during POST. */
	/* The Linux kernel seems to think it is free for use :-/ */
	_asm("out 0x80, %0"
	     :
	     : "a"(0));
	/* %%al instead of %0 makes no difference.  TODO: does the register need to be zeroed? */
}

static __inline__ uint32_t __early get_cr0(void) {
	uint32_t ret;
	_asm("mov %0, cr0"
	     : "=r"(ret));

	return ret;
}

static __inline__ void __early set_cr0(uint32_t val) {
	_asm("mov cr0, %0" ::"r"(val));
}

static __inline__ uint32_t __early get_cr3(void) {
	uint32_t ret;
	_asm("mov %0, cr3"
	     : "=r"(ret));

	return ret;
}

static __inline__ void __early set_cr3(uint32_t val) {
	_asm("mov cr3, %0" ::"r"(val));
}

static __inline__ uint32_t __early get_cr4(void) {
	uint32_t ret;
	_asm("mov %0, cr4"
	     : "=r"(ret));

	return ret;
}

static __inline__ void __early set_cr4(uint32_t val) {
	_asm("mov cr4, %0" ::"r"(val));
}

#endif  // __ASM_H_