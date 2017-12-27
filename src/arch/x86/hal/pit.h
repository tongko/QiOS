/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
#ifndef __PIT_H_
#define __PIT_H_

#include <stdbool.h>
#include <stdint.h>

/*	Operational Command Bit masks	*/
#define PIT_ICW_MASK_BINCOUNT 0x01  //00000001
#define PIT_ICW_MASK_MODE 0x0E      //00001110
#define PIT_ICW_MASK_RL 0x30        //00110000
#define PIT_ICW_MASK_COUNTER 0xC0   //11000000

/*	Operational Command control bits	*/
//	Use when setting binary count mode
#define PIT_ICW_BINCOUNT_BINARY 0x00  //0
#define PIT_ICW_BINCOUNT_BCD 0x01     //1

//	Use when setting counter mode
#define PIT_ICW_MODE_TERMINALCOUNT 0x0  //0000
#define PIT_ICW_MODE_ONESHOT 0x2        //0010
#define PIT_ICW_MODE_RATEGEN 0x4        //0100
#define PIT_ICW_MODE_SQUAREWAVEGEN 0x6  //0110
#define PIT_ICW_MODE_SOFTWARETRIG 0x8   //1000
#define PIT_ICW_MODE_HARDWARETRIG 0xA   //1010

//	Use when setting data transfer
#define PIT_ICW_RL_LATCH 0x00    //000000
#define PIT_ICW_RL_LSBONLY 0x10  //010000
#define PIT_ICW_RL_MSBONLY 0x20  //100000
#define PIT_ICW_RL_DATA 0x30     //110000

//	Use when setting the counter we are working with
#define PIT_ICW_COUNTER_0 0x00  //00000000
#define PIT_ICW_COUNTER_1 0x40  //01000000
#define PIT_ICW_COUNTER_2 0x80  //10000000

/*	Interface Programmable Interrupt Timer (PIT)	*/
//	Send operational command to pit. Set up command by using the operational
//	command bit masks and setting them with the control bits. Shouldn't need to use
//	this outside the interface
extern void pit_send_command(uint8_t cmd);

//! write data byte to a counter
extern void pit_send_data(uint16_t data, uint8_t counter);

//! reads data from a counter
extern uint8_t pit_read_data(uint16_t counter);

//! Sets new pit tick count and returns prev. value
extern uint32_t pit_set_tick_count(uint32_t i);

//! returns current tick count
extern uint32_t pit_get_tick_count(void);

//! starts a counter. Counter continues until another call to this routine
extern void pit_start_counter(uint32_t freq, uint8_t counter, uint8_t mode);

//! Initialize minidriver
extern void pit_init(void);

//! Test if interface is initialized
extern bool pit_is_init(void);

#endif  //	__PIT_H_