/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
#ifndef __SERIAL_PORT_H_
#define __SERIAL_PORT_H_

#include <attribs.h>
#include <stdint.h>

#define SERIAL_COM1 0x3F8 /* COM1 base port */
/* The I/O ports */
/* All the I/O ports are calculated relative to the data port. This is because
*  all serial ports (COM1, COM2, COM3, COM4) have their ports in the same
*  order, but they start at different values.
*/
#define SERIAL_DATA_PORT(base) (base)
#define SERIAL_FIFO_COMMAND_PORT(base) (base + 2)
#define SERIAL_LINE_COMMAND_PORT(base) (base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) (base + 4)
#define SERIAL_LINE_STATUS_PORT(base) (base + 5)

/* Port commands */
/* SERIAL_LINE_ENABLE_DLAB:
*  Tells the serial port to expect first the highest 8 bits on the data port,
*  then the lowest 8 bits will follow
*/
#define SERIAL_LINE_ENABLE_DLAB 0x80

typedef struct {
	void (*init_baud_rate)(uint16_t com, uint16_t divisor);
	void (*set_register)(uint16_t com, uint16_t value);
	void (*putc)(char c);
	void (*puts)(const char *str, int32_t n);
	void (*print)(const char *format, ...);
} serial_port_api_t;

__early serial_port_api_t *serial_port_api(void);
__early void serial_port_default_config(void);
__early void serial_port_init(serial_port_api_t *api);

#endif  //	__SERIAL_PORT_H_