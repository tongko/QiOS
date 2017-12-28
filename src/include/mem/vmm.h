/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 * ****************************************************************************/
#ifndef __VMM_H_
#define __VMM_H_

#include <attribs.h>
#include <mem/pmm.h>
#include <stdint.h> 

//	Initialize Virtual Memory Manager
void __early vmm_init();

#endif  //	__VMM_H_