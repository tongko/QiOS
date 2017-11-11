#include "boot.h"

static uint16_t* VIDEO_MEM = (uint16_t*) 0xB8000;
static unsigned char _print_color = 0x2F;

static __inline__ unsigned char vga_color_entry(unsigned char fg, unsigned char bg) {
	return	fg | bg << 4;
}

static __inline__ uint16_t vga_entry(char c, unsigned char color) {
	return (uint16_t) c | (uint16_t) color << 8;
}

static char *itoa(uint32_t value, char *str, uint32_t base) {
	char *p = str;
	int i = 30;
	
	for(; value && i ; --i, value /= base) {	
		p[i] = "0123456789abcdef"[value % base];
	}
	
	return p;
}

static __inline__ void _set_cursor(unsigned char x, unsigned char y) {
	outb(0x3d4, 0x0f); 
	outb(0x3d5, x); 
	outb(0x3d4, 0x0e); 		
	outb(0x3d5, y);	
}

void _set_color(unsigned char fg, unsigned char bg) {
	_print_color = vga_color_entry(fg, bg);
}

void _clear(void) {
	// Fill the screen with background color 
	for(uint32_t i = 0; i < 2000; i++) {					
		VIDEO_MEM[i] = vga_entry(' ', _print_color);
	}

	// Set the cursor to the upper-left corner of the screen
	_set_cursor(0, 0);
}

void _putc(char c) {
	//	Video memory offset
	uint32_t offset;

	//	Get cursor Y position
	outb(0x3d4, 0x0e);
	offset = inb(0x3d5);
	offset <<= 8;		// shift left 8 bit to make room for x

	//	Add cursor x position
	outb(0x3d4, 0x0f);
	offset += inb(0x3d5);
	offset <<= 1;		//	shift left 1 bit, 1 byte for color, 1 byte for char

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
			VIDEO_MEM[offset] = vga_entry(' ', _print_color);
			break;
		default:
			VIDEO_MEM[offset++] = vga_entry(c, _print_color);
			break;
	}

	//	end of screen?
	if (offset >= 2000) {

		__asm__ __volatile__(//".intel_syntax noprefix;\n"
			"push edi;\n"
			"push esi;\n"
			"push eax;\n"
			"push ecx;\n"
			"mov edi, 0xB8000;\n"
			"mov esi, 0xB80A0;\n"
			"mov ecx, 0x30C;\n"
			"rep movsw;\n"
			"mov edi, 0xB9680;\n"
			"xor eax, eax;\n"
			"mov ecx, 40;\n"
			"rep stosw;\n"
			"pop ecx;\n"
			"pop eax;\n"
			"pop esi;\n"
			"pop edi;\n"
		);
/*		//	Scroll the screen up 1 line
		for (i = 0; i < 1920; i++) {
			VIDEO_MEM[i] = VIDEO_MEM[i + 80];
		}

		// clear the last row
		for (i = 1920; i < 2000; i++) {
			VIDEO_MEM[i] = vga_entry(' ', _print_color);
		}
*/
		VIDEO_MEM -= 80;
	}

	//	set cursor new position
	offset >>= 1;
	_set_cursor(offset & 0x00ff, offset >> 8);
}

uint32_t _vsprintf(char *str, const char *format, va_list arg) {
	char*	p = str;
	char*	s;
	uint32_t	d;
	unsigned char		c;
	uint32_t	i = 0;

	*p = '\0';
	//	loop through the string until NULL terminator
	while (*format != '\0') {
		if (*format != '%') {
			//	Normal character
			i = strlen(p);
			p[i] = *format;
			p[i + 1] = '\0';
		}
		else {
			switch (*(format + 1)) {
				case 'c': // Char
					c = (char) va_arg(arg, int);
					i = strlen(p);
					p[i] = c;
					p[i + 1]  = '\0';
					break;
				case 's': // String
					s = va_arg(arg, char *);
					strcat(p, s);
					break;
				case 'd': // Integer
				case 'i':
					d = va_arg(arg, int);
					char buff[10] = {0};	// decimal max 10 digits.
					strcat(p, itoa(d, buff, 10));
					break;
				default:
				// Print the % and the following character
					i = strlen(p);
					p[i]  = *format;
					p[i + 1]  = *(format + 1);
					p[i + 2]  = '\0';
					break;
			}

			format++;
		}

		format++;
	}

	return strlen(str);
}

uint32_t _printf(const char *format, ...) {
	char buffer[MAXLEN];
	va_list arg;
	uint32_t i = 0;

	va_start(arg, format);
	_vsprintf(buffer, format, arg);
	va_end(arg);

	while(buffer[i] != '\0') {
		_putc(buffer[i++]);
	}

	return i;
}