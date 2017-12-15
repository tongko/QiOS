/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
#ifndef __REGS_H_
#define __REGS_H_

/*		Interface Required Header		*/

#include <stdint.h>

/*		Structures		*/

typedef struct {
	uint32_t eax, ebx, ecx, edx;
	uint32_t esi, edi, ebp, esp, eflags;
	uint8_t cflags;
} _r32_regs_t;

typedef struct {
	uint16_t ax, bx, cx, dx;
	uint16_t si, di, bp, sp, es, cs, ss, ds, flags;
	uint8_t cflags;
} _r16_regs_t;

typedef struct {
	uint16_t ax, axh, bx, bxh, cx, cxh, dx, dxh;
	uint16_t si, di, bp, sp, es, cs, ss, ds, flags;
	uint8_t cflags;
} _r16_regs_32_t;

typedef struct {
	uint8_t al, ah, bl, bh, cl, ch, dl, dh;
} _r8_regs_t;

typedef struct {
	uint8_t al, ah;
	uint16_t axh;
	uint8_t bl, bh;
	uint16_t bxh;
	uint8_t cl, ch;
	uint16_t cxh;
	uint8_t dl, dh;
	uint16_t dxh;
} _r8_regs_32_t;

union intr_16 {
	_r16_regs_t x;
	_r8_regs_t h;
};

union intr_32 {
	_r32_regs_t x;
	_r16_regs_32_t l;
	_r8_regs_32_t h;
};

#endif  //	__REGS_H_