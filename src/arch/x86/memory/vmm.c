/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 * ****************************************************************************/
#include <attribs.h>
#include <mem/paging.h>
#include <mem/pmm.h>
#include <mem/vmm.h>
#include <stdbool.h>

void vmm_init() {
	pg_init();
	pg_enable(true);
	pg_flush_tlb();
}
