#ifndef			__TYPEDEF_H_
#define			__TYPEDEF_H_

/* Surely you will remove the processor conditionals and this comment
appropriately depending on whether or not you use C++. */
#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
#include <stddef.h>
#include <stdint.h>

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

typedef struct IDT_Entry_s
{
	uint16_t	HandlerPtr_Low;
	uint16_t	Selector;
	uint8_t		Reserved;
	uint8_t		GateType		: 4;
	uint8_t		StorageSegment	: 1;
	uint8_t		DPL				: 2;
	uint8_t		Present			: 1;
	uint16_t	HandlerPtr_High;
} IDT_Entry_t;

typedef struct IDT_Pointer_s
{
	uint16_t		TableSize;
	IDT_Entry_t*	IDT_Ptr;
} IDT_Pointer_t;

#endif	/* __TYPEDEF_H_ */