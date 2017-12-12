/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
#ifndef __PIC_H_
#define __PIC_H_

#include <stdint.h>

#define MPIC 0x20 /* IO base address for master PIC */
#define SPIC 0xA0 /* IO base address for slave PIC */
#define MPIC_CMD MPIC
#define MPIC_DTA (MPIC + 1)
#define SPIC_CMD SPIC
#define SPIC_DTA (SPIC + 1)
#define PIC_EOI 0x20 /* End-of-interrupt command code */

#define ICW1_ICW4 0x01      /* ICW4 (not) needed */
#define ICW1_SINGLE 0x02    /* Single (cascade) mode */
#define ICW1_INTERVAL4 0x04 /* Call address interval 4 (8) */
#define ICW1_LEVEL 0x08     /* Level triggered (edge) mode */
#define ICW1_INIT 0x10      /* Initialization - required! */

#define ICW4_8086 0x01       /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO 0x02       /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE 0x08  /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C /* Buffered mode/master */
#define ICW4_SFNM 0x10       /* Special fully nested (not) */

void init_pic(void);
/* Used to disable irq */
void irq_set_line(uint8_t irq_line);
/* Used to enabled irq */
void irq_clear_mask(uint8_t irq_line);
void pic_remap(uint32_t offset1, uint32_t offset2);
void pic_acknowledge(uint8_t irq);

#endif  //	__PIC_H_