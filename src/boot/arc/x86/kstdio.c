#include "kio.h"
#include "kstring.h"
#include "kstdio.h"

static uint16_t* VIDEO_MEM = (uint16_t*) 0xB8000;
static uint8_t _print_color = 0x2F;

static __inline__ uint8_t vga_color_entry(uint8_t fg, uint8_t bg) {
	return	fg | bg << 4;
}

static __inline__ uint16_t vga_entry(uint8_t c, uint8_t color) {
	return (uint16_t) c | (uint16_t) color << 8;
}

static uint8_t* itoa(uint32_t value, uint8_t* str, uint32_t base) {
	uint8_t* p = str;
	int i = 30;
	
	for(; value && i ; --i, value /= base) {	
		p[i] = "0123456789abcdef"[val % base];
	}
	
	return p;
}

static __inline__ void _set_cursor(uint8_t x, uint8_t y) {
	outb(0x3d4, 0x0f); 
	outb(0x3d5, x); 
	outb(0x3d4, 0x0e); 		
	outb(0x3d5, y);	
}

void _set_color(uint8_t fg, uint8_t bg) {
	_print_color = vga_color_entry(fg, bg);
}

void _clear(void) {
	// Fill the screen with background color 
	for(i = 0; i < 2000; i++) {					
		VIDEO_MEM[i] = vga_entry(' ', _print_color);
	}

	// Set the cursor to the upper-left corner of the screen
	_set_cursor(0, 0);
}

void _putc(uint8 c) {
	//	Video memory offset
	uint32_t offset, i;

	//	Get cursor Y position
	outb(ox3d4, 0x0e);
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

		__asm__ (".intel_syntax;\n"
			"push edi;\n"
			"push esi;\n"
			"push eax;\n"
			"push ecx;\n"
			"mov edi, 0xB8000;\n"
			"mov esi, 0xB80A0;\n"
			"mov ecx, 0x30C;\n"
			"rep movsw;\n"
			"mov edi, 0xB9680"
			"mov eax, 0;\n"
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

uint32_t _vsprintf(uint8_t* str, const uint8_t* format, va_list arg) {
	uint8_t*	p = str;
	uint8_t*	f = format;
	uint8_t*	s;
	uint32_t	d;
	uint8_t		c;
	uint32_t	i = 0;

	*p = '\0';
	//	loop through the string until NULL terminator
	while (*f != '\0') {
		if (*f != '%') {
			//	Normal character
			i = strlen(p);
			p[i] = *f;
			p[i + 1] = '\0';
		}
		else {
			switch (*(f + 1)) {
				case 'c': // Char
					c = (char) va_arg(ap, int);
					i = strlen(p);
					p[i] = c;
					p[i + 1]  = '\0';
					break;
				case 's': // String
					s = va_arg(ap, char *);
					strcat(p, s);
					break;
				case 'd': // Integer
				case 'i':
					d = va_arg(ap, int);
					uint8_t buff[10] = {0};	// decimal max 10 digits.
					strcat(p, itoa(d, s, 10));
					break;
				default:
				// Print the % and the following character
					i = strlen(p);
					p[i]  = *f;
					p[i + 1]  = *(f + 1);
					p[i + 2]  = '\0';
					break;
			}

			f++;
		}
		f++;
	}
}

uint32_t _printf(const uint8_t* format, ...) {
	uint8_t buffer[MAXLEN];
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