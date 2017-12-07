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

#define asm(x...) __asm__ __volatile__(x)

static __inline__ void sti(void) {
	asm("sti\n"
	    "nop\n");
}

static __inline__ void cli(void) {
	asm("cli\n"
	    "nop\n");
}

static __inline__ uint8_t _inb(uint16_t port) {
	uint8_t ret_val;
	__asm__ __volatile__("in %0,%1"
	                     : "=a"(ret_val)
	                     : "d"(port));
	return ret_val;
}

static __inline__ void _outb(uint16_t port, uint8_t val) {
	__asm__ __volatile__("out %1,%0" ::"a"(val), "d"(port));
}

static __inline__ uint32_t __early get_cr0(void) {
	uint32_t ret;
	asm("mov %0, cr0"
	    : "=r"(ret));

	return ret;
}

static __inline__ void __early set_cr0(uint32_t val) {
	asm("mov cr0, %0" ::"r"(val));
}

static __inline__ uint32_t __early get_cr3(void) {
	uint32_t ret;
	asm("mov %0, cr3"
	    : "=r"(ret));

	return ret;
}

static __inline__ void __early set_cr3(uint32_t val) {
	asm("mov cr3, %0" ::"r"(val));
}

static __inline__ uint32_t __early get_cr4(void) {
	uint32_t ret;
	asm("mov %0, cr4"
	    : "=r"(ret));

	return ret;
}

static __inline__ void __early set_cr4(uint32_t val) {
	asm("mov cr4, %0" ::"r"(val));
}

#endif  // __ASM_H_