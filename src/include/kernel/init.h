/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 * ****************************************************************************/
#ifndef __KERNEL_INIT_H_
#define __KERNEL_INIT_H_

#include <stdint.h>
#include <sys/types.h>

typedef struct setup_info {
	paddr_t kernel_phys_start;
	paddr_t kernel_phys_end;
	vaddr_t kernel_virt_start;
	vaddr_t kernel_virt_end;
	paddr_t mbi_addr;
	uint32_t mbi_magic;
} setup_info_t;

#endif  //	__KERNEL_INIT_H_