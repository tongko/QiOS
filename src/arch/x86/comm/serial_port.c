/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
#include "serial_port.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include "../asm.h"

static serial_port_api_t _api;

/** serial_configure_baud_rate:
*   Sets the speed of the data being sent. The default speed of a serial
*   port is 115200 bits/s. The argument is a divisor of that number, hence
*   the resulting speed becomes (115200 / divisor) bits/s.
*
*   @param com      The COM port to configure
*   @param divisor  The divisor
*/
static void init_baud_rate(uint16_t com, uint16_t divisor) {
	_outb(SERIAL_LINE_COMMAND_PORT(com),
	      SERIAL_LINE_ENABLE_DLAB);
	_outb(SERIAL_DATA_PORT(com),
	      (divisor >> 8) & 0x00FF);
	_outb(SERIAL_DATA_PORT(com),
	      divisor & 0x00FF);
}

static void set_register(uint16_t port, uint16_t value) {
	_outb(port, value);
}

/** serial_is_transmit_fifo_empty:
*   Checks whether the transmit FIFO queue is empty or not for the given COM
*   port.
*
*   @param  com The COM port
*   @return 0 if the transmit FIFO queue is not empty
*           1 if the transmit FIFO queue is empty
*/
static int serial_is_transmit_fifo_empty(uint16_t com) {
	/* 0x20 = 0010 0000 */
	return _inb(SERIAL_LINE_STATUS_PORT(com)) & 0x20;
}

static void putc(char c) {
	// Block until buffer is not full
	while (!serial_is_transmit_fifo_empty(SERIAL_COM1)) {
	}

	_outb(SERIAL_DATA_PORT(SERIAL_COM1), c);
}

static void puts(const char *str, int32_t n) {
	for (int32_t i = 0; i < n; i++) {
		_api.putc(str[i]);
	}
}

static void print(const char *format, ...) {
	char buffer[MAXLEN] = {0};
	va_list arg;
	int32_t i = 0;

	va_start(arg, format);
	vsprintf(buffer, format, arg);
	va_end(arg);

	while (buffer[i] != '\0') {
		_api.putc(buffer[i++]);
	}
}

void serial_default_config(void) {
	_api.init_baud_rate(SERIAL_COM1, 2);
	/** serial_configure_line:
*   Configures the line of the given serial port. The port is set to have a
*   data length of 8 bits, no parity bits, one stop bit and break control
*   disabled.
*
*   @param com  The serial port to configure
*/
	/* Bit:     | 7 | 6 | 5 4 3 | 2 | 1 0 |
	*  Content: | d | b | prty  | s | dl  |
	*  Value:   | 0 | 0 | 0 0 0 | 0 | 1 1 | = 0x03
	*/
	_api.set_register(SERIAL_LINE_COMMAND_PORT(SERIAL_COM1), 0x03);
	/* Bit:     | 7 6 | 5  | 4 | 3   | 2   | 1   | 0 |
    *  Content: | lvl | bs | r | dma | clt | clr | e |
    *  Value:   | 11  | 0  | 0 | 0   | 1   | 1   | 1 |
    */
	_api.set_register(SERIAL_FIFO_COMMAND_PORT(SERIAL_COM1), 0xC7);
	/* Bit:     | 7 | 6 | 5  | 4  | 3   | 2   | 1   | 0   |
    *  Content: | r | r | af | lb | ao2 | ao1 | rts | dtr |
    *  Value:   | 0 | 0 | 0  | 0  | 0   | 0   | 1   | 1   |
    */
	_api.set_register(SERIAL_MODEM_COMMAND_PORT(SERIAL_COM1), 0x03);
}

serial_port_api_t *serial_port_api() {
	return &_api;
}

void init_serial_port(serial_port_api_t *api) {
	if (api != NULL) {
		_api.init_baud_rate = api->init_baud_rate;
		_api.set_register = api->set_register;
		_api.print = api->print;
		_api.putc = api->putc;
		_api.puts = api->puts;
		return;
	}

	_api.init_baud_rate = init_baud_rate;
	_api.set_register = set_register;
	_api.print = print;
	_api.putc = putc;
	_api.puts = puts;
}
