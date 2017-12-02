#ifndef __SERIAL_PORT_H_
#define __SERIAL_PORT_H_

#include <stdint.h>

void init_serial_port(void);
void serial_putc(char c);
void serial_write_bytes(char *c, int32_t n);
void serial_print(const char *format, ...);

#endif  //	__SERIAL_PORT_H_