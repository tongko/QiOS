#ifndef __KCONSOLE_H
#define __KCONSOLE_H

#include <stdint.h>

typedef struct tagCONSOLECOLOR {
	uint8_t foreground;
	uint8_t background;
} CONSOLECOLOR, *LPCONSOLECOLOR;

typedef struct tagPOINT {
	uint8_t x;
	uint8_t y;
} POINT, *LPPOINT;

void init_console(LPCONSOLECOLOR conColor);
void clear(void);
void set_console_color(LPCONSOLECOLOR conColor);
void write_console(const char *format, ...);
void putc_console(char c, LPPOINT lpPoint);
void get_cursor(LPPOINT lpPoint);
void set_cursor(LPPOINT point);
void cursor_shape(uint8_t scanStart, uint8_t scanEnd);

#endif  //	__KCONSOLE_H