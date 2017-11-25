#ifndef		__ARCH_X86_MULTIBOOT_H_
#define		__ARCH_X86_MULTIBOOT_H_

#include <stdint.h>

#define STACK_SIZE				0x100000						// 1MB

uint32_t KERNEL_VIRTUAL_BASE;

void *kernel_stack_bottom;
void *PageDirectoryVirtualAddress;
void *PageDirectoryPhysicalAddress;

#endif	//	__ARCH_X86_MULTIBOOT_H_