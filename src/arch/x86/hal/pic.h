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

//	The following devices use PIC 1 to generate interrupts
#define I386_PIC_IRQ_TIMER 0
#define I386_PIC_IRQ_KEYBOARD 1
#define I386_PIC_IRQ_SERIAL2 3
#define I386_PIC_IRQ_SERIAL1 4
#define I386_PIC_IRQ_PARALLEL2 5
#define I386_PIC_IRQ_DISKETTE 6
#define I386_PIC_IRQ_PARALLEL1 7

//	The following devices use PIC 2 to generate interrupts
#define I386_PIC_IRQ_CMOSTIMER 0
#define I386_PIC_IRQ_CGARETRACE 1
#define I386_PIC_IRQ_AUXILIARY 4
#define I386_PIC_IRQ_FPU 5
#define I386_PIC_IRQ_HDC 6

#define I386_PIC_ICW1_ICW4 0x01      /* ICW4 (not) needed */
#define I386_PIC_ICW1_SINGLE 0x02    /* Single (cascade) mode */
#define I386_PIC_ICW1_INTERVAL4 0x04 /* Call address interval 4 (8) */
#define I386_PIC_ICW1_LEVEL 0x08     /* Level triggered (edge) mode */
#define I386_PIC_ICW1_INIT 0x10      /* Initialization - required! */

//	Command Word 2 bit masks. Use when sending commands
#define i386_PIC_ICW2_MASK_L1 0x01      //00000001
#define i386_PIC_ICW2_MASK_L2 0x02      //00000010
#define i386_PIC_ICW2_MASK_L3 0x04      //00000100
#define i386_PIC_ICW2_MASK_EOI 0x20     //00100000
#define i386_PIC_ICW2_MASK_SL 0x40      //01000000
#define i386_PIC_ICW2_MASK_ROTATE 0x80  //10000000

//! Command Word 3 bit masks. Use when sending commands
#define I386_PIC_ICW3_MASK_RIS 0x01   //00000001
#define I386_PIC_ICW3_MASK_RIR 0x02   //00000010
#define I386_PIC_ICW3_MASK_MODE 0x04  //00000100
#define I386_PIC_ICW3_MASK_SMM 0x20   //00100000
#define I386_PIC_ICW3_MASK_ESMM 0x40  //01000000
#define I386_PIC_ICW3_MASK_D7 0x80    //10000000

#define I386_PIC_ICW4_8086 0x01       /* 8086/88 (MCS-80/85) mode */
#define I386_PIC_ICW4_AUTO 0x02       /* Auto (normal) EOI */
#define I386_PIC_ICW4_BUF_SLAVE 0x08  /* Buffered mode/slave */
#define I386_PIC_ICW4_BUF_MASTER 0x0C /* Buffered mode/master */
#define I386_PIC_ICW4_SFNM 0x10       /* Special fully nested (not) */

//	Initialize PIC
extern void pic_init(uint8_t base0, uint8_t base1);

//	Read data byte from PIC
extern uint8_t pic_read_byte(uint8_t pic_num);

//	Send data byte to PIC
extern void pic_send_byte(uint8_t data, uint8_t pic_num);

//	Send operational command to PIC
extern void pic_send_command(uint8_t cmd, uint8_t pic_num);

//	Enables and disables interrupts
extern void pic_mask_irq(uint8_t irq_mask, uint8_t pic_num);

// /* Used to disable irq */
// void irq_set_line(uint8_t irq_line);
// /* Used to enabled irq */
// void irq_clear_mask(uint8_t irq_line);
// void pic_remap(uint32_t offset1, uint32_t offset2);
// void pic_acknowledge(uint8_t irq);

#endif  //	__PIC_H_