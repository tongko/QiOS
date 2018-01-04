/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
#include "pit.h"
#include <asm.h>
#include <attribs.h>
#include <hal.h>

/*	Controller Registers	*/
#define PIT_REG_COUNTER0 0x40
#define PIT_REG_COUNTER1 0x41
#define PIT_REG_COUNTER2 0x42
#define PIT_REG_COMMAND 0x43

#define pic_port(x) ((x == PIT_ICW_COUNTER_0) ? PIT_REG_COUNTER0 : (x == PIT_ICW_COUNTER_1) ? PIT_REG_COUNTER1 : PIT_REG_COUNTER2)

//	Global Tick count!
static volatile uint32_t _pit_ticks = 0;

//	true if PIT is init.
static bool _pit_is_init = false;

//	PIT Timer Interrupt Handler
static void pit_irq() {
	_asm(
	    "add esp, 12\n"
	    "pushad\n" ::);

	_pit_ticks++;

	intr_done(0);

	_asm(
	    "popad\n"
	    "iretd" ::);
}

//	Sets new PIT tick count and returns previous value.
uint32_t pit_set_tick_count(uint32_t i) {
	uint32_t ret = _pit_ticks;
	_pit_ticks = i;
	return ret;
}

//	Returns current tick count
uint32_t pit_get_tick_count() {
	return _pit_ticks;
}

//	Sends command to a counter
void pit_send_command(uint8_t cmd) {
	outportb(PIT_REG_COMMAND, cmd);
}

//	Sends data to a counter
void pit_send_data(uint16_t data, uint8_t counter) {
	uint8_t port = pic_port(counter);

	outportb(port, (uint8_t)data);
}

//	Read data from counter
uint8_t pit_read_data(uint16_t counter) {
	uint8_t port = pic_port(counter);

	return inportb(port);
}

//	Starts a counter
void pit_start_counter(uint32_t freq, uint8_t counter, uint8_t mode) {
	if (freq == 0) {
		return;
	}

	uint16_t divisor = (uint16_t)(0x1234DD / (uint16_t)freq);

	//	Send operational command
	uint8_t icw = 0;
	icw = (icw & ~PIT_ICW_MASK_MODE) | mode;
	icw = (icw & ~PIT_ICW_MASK_RL) | PIT_ICW_RL_DATA;
	icw = (icw & ~PIT_ICW_MASK_COUNTER) | counter;
	pit_send_command(icw);

	//	Set frequency rate
	pit_send_data(divisor & 0xFF, 0);
	pit_send_data((divisor >> 8) & 0xFF, 0);

	//	Reset tick count
	_pit_ticks = 0;
}

//	Initialize PIT
void pit_init() {
	// Install interrupt handler(irq 0 uses interrupt 32)
	intr_set_vect(32, pit_irq);

	_pit_is_init = true;
}

//	Test if PIT interface is init
bool pit_is_init() {
	return _pit_is_init;
}