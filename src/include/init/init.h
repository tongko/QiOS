/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 * ****************************************************************************/
#ifndef __INIT_H_
#define __INIT_H_

#include <attribs.h>
#include <mem/memory.h>
#include <multiboot/mb_info.h>
#include <stdint.h>
#include <sys/types.h>

/*	Interfaces	*/

void early_init(kernel_meminfo_t k_meminfo, mbi_t mbi);

#endif  //	__INIT_H_