/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 * ****************************************************************************/
#ifndef __PAGING_H_
#define __PAGING_H_

#include <mem/pmm.h>
#include <stdint.h>

typedef void *vaddr_t;

//	laod a new paging structure
paddr_t pg_load(paddr_t new_paging);
//	Enable/Disable paging
void pg_enable(bool enable);
//	Flush Translation Lookaside Buffer (TLB) entry
void pg_flush_tlb_entry(vaddr_t vaddr);
//	Map virtual address to a physical address
void pg_map_addr(vaddr_t vaddr, paddr_t paddr, uint32_t length);
//	Initialize Paging
void pg_init(void);

#endif  //	__PAGING_H_