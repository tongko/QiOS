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

#include <asm.h>
#include <hal.h>
#include <kernel/init.h>
#include <kio.h>
#include <mem/memory.h>
#include <multiboot/mb_info.h>
#include <multiboot/multiboot2.h>
#include <stdarg.h>
#include <stdint.h>
#include <sys/serial_port.h>
#include <sys/symbols.h>
#include <sys/term.h>
#include "../arch/x86/exception.h"
#include "../arch/x86/func.h"

#endif  //	__BOOT_H_