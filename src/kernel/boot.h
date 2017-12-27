/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/

#ifndef __BOOT_H_
#define __BOOT_H_

#include <hal.h>
#include <multiboot/mb_info.h>
#include <multiboot/multiboot2.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/symbols.h>
#include <sys/term.h>
#include "../arch/x86/asm.h"
#include "../arch/x86/boot/paging.h"
#include "../arch/x86/comm/serial_port.h"
#include "../arch/x86/exception.h"
#include "../arch/x86/func.h"

#endif  //	__BOOT_H_