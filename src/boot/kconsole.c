#ifndef __KERNEL_
#define __KERNEL_
#endif

#include "kconsole.h"
#include "kio.h"
#include "kstdarg.h"
#include "kstdlib.h"
#include "kstring.h"

#ifndef MAXLEN
#define MAXLEN 1024
#endif
#define CONSOLE_WIDTH 80
#define CONSOLE_HEIGHT 25

//	static, not define so we can dynamically change in future.
static volatile uint16_t *_video_mem = (uint16_t *)0xB8000;
static POINT _cursor_point;
static CONSOLECOLOR _console_color;

static __inline__ uint8_t vga_color(LPCONSOLECOLOR lpColor) {
	return lpColor->foreground | lpColor->background << 4;
}

static __inline__ uint16_t vga_entry(char c, LPCONSOLECOLOR lpColor) {
	return (uint16_t)c | ((uint16_t)vga_color(lpColor) << 8);
}

static int32_t format_string(char *str, const char *format, va_list arg) {
	char *p = str;
	char *s;
	uint32_t d;
	char c;
	size_t i = 0;

	*p = '\0';
	//	loop through the string until NULL terminator
	while (*format) {
		if (*format != '%') {
			//	Normal character
			p[i++] = *format;
			p[i] = '\0';
		} else {
			switch (*(format + 1)) {
				case 'c':  // Char
					c = (char)va_arg(arg, int);
					p[i++] = c;
					p[i] = '\0';
					break;
				case 's':  // String
					s = va_arg(arg, char *);
					_strcat(p, s);
					i = _strlen(p);
					break;
				case 'u':  // Unsigned integer
				case 'd':  // Integer
				case 'i':
					d = va_arg(arg, int);
					char buff1[10] = {0};  // decimal max 10 digits.
					_strcat(p, _itoa(d, buff1, 10));
					i = _strlen(p);
					break;
				case 'x':  // Hexadecimal number
					d = va_arg(arg, uint32_t);
					char buff2[8] = {0};
					// Check for leading 0
					int32_t j = 3;
					char ibuff[10] = {0};
					int32_t leading = 0;
					if (*(format + 2) == '.') {
						//	Read all digits and make it number of leading zeros
						while (*(format + j) >= '0' && *(format + j) <= '9') {
							ibuff[j - 3] = *(format + j);
							j++;
						}
						//	the . is meant for printing not formating if j == 2
						if (j > 3) {
							format += j - 2;
							leading = _atoi(ibuff);
						}
					}
					_itoa(d, buff2, 16);
					if (leading > 0) {
						char buff3[leading];
						size_t bufflen = _strlen(buff2);
						int32_t excess = leading - bufflen;
						for (int32_t j = 0; j < leading; j++) {
							if (j < excess) {
								buff3[j] = '0';
							} else {
								buff3[j] = buff2[j - excess];
							}
						}
						_strcat(p, buff3);
					} else {
						_strcat(p, buff2);
					}
					i = _strlen(p);
					break;
				default:
					// Print the % and the following character
					p[i++] = *format;
					p[i++] = *(format + 1);
					p[i] = '\0';
					break;
			}

			format++;
		}

		format++;
	}

	return i - 1;
}

void init_console(LPCONSOLECOLOR conColor) {
	_console_color.foreground = conColor->foreground;
	_console_color.background = conColor->background;

	clear();

	_cursor_point.x = 0;
	_cursor_point.y = 0;
}

void clear() {
	for (int32_t i = 0; i < 2000; i++) {
		_video_mem[i] = vga_entry(' ', &_console_color);
	}
}

void set_console_color(LPCONSOLECOLOR conColor) {
	_console_color.foreground = conColor->foreground;
	_console_color.background = conColor->background;
}

void write_console(const char *format, ...) {
	char buffer[MAXLEN] = {0};
	va_list arg;
	int32_t i = 0;

	va_start(arg, format);
	format_string(buffer, format, arg);
	va_end(arg);

	while (buffer[i] != '\0') {
		putc_console(buffer[i++], &_cursor_point);
	}

	set_cursor(&_cursor_point);
}

void putc_console(char c, LPPOINT lpPoint) {
	uint32_t offset = lpPoint->y * CONSOLE_WIDTH + lpPoint->x;

	switch (c) {
		// new line
		case '\n':
			offset = (offset / 80) * 80 + 80;
			break;
		// carriage return
		case '\r':
			offset = (offset / 80) * 80;
			break;
		// tab
		case '\t':
			offset += 4;
			break;
		// delete
		case 8:
			offset--;
			_video_mem[offset] = vga_entry(' ', &_console_color);
			break;
		default:
			_video_mem[offset++] = vga_entry(c, &_console_color);
			break;
	}

	//	end of screen?
	if (offset > 1999) {
		__asm__ __volatile__(  //".intel_syntax noprefix;\n"
		    "push edi;\n"
		    "push esi;\n"
		    "push eax;\n"
		    "push ecx;\n"
		    "mov edi, 0xB8000;\n"  // dest idx to start of video
		    "mov esi, 0xB80A0;\n"  // src idx to 2nd row
		    "mov ecx, 0x780;\n"    // prepare to loop for 1920 times.
		    "rep movsw;\n"         // loop
		    "mov edi, 0xB8F00;\n"  // 25th row to dest idx
		    "mov eax, 32;\n"       // space
		    "mov ecx, 80;\n"       // prepare to loop 80 times
		    "rep stosw;\n"         // loop
		    "pop ecx;\n"
		    "pop eax;\n"
		    "pop esi;\n"
		    "pop edi;\n");

		offset -= 80;
	}

	//	set cursor new position
	lpPoint->x = offset % 80;
	lpPoint->y = (offset / 80);
}

void get_cursor(LPPOINT lpPoint) {
	_outb(0x3D4, 0x0F);
	lpPoint->x = _inb(0x3D5);
	_outb(0x3D4, 0x0E);
	lpPoint->y = _inb(0x3D5);
}

void set_cursor(LPPOINT lpPoint) {
	uint16_t pos = lpPoint->y * CONSOLE_WIDTH + lpPoint->x;

	_outb(0x3D4, 0x0F);
	_outb(0x3D5, (uint8_t)(pos & 0xFF));

	_outb(0x3D4, 0x0E);
	_outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void cursor_shape(uint8_t scanStart, uint8_t scanEnd) {
	_outb(0x3D4, 0x0A);
	_outb(0x3D5, (_inb(0x3D5) & 0xC0) | scanStart);

	_outb(0x3D4, 0x0B);
	_outb(0x3D5, (_inb(0x3E0) & 0xE0) | scanEnd);
}
