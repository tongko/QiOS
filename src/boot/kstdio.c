#include "io.h"
#include "kstdio.h"

uint16_t* VIDEO_MEM = (uint16_t*) 0xB8000;
uint8_t _print_color = 0x2F;

static __inline__ uint8_t vga_color_entry(uint8_t fg, uint8_t bg) {
	return	fg | bg << 4;
}

static __inline__ uint16_t vga_entry(uint8_t c, uint8_t color) {
	return (uint16_t) c | (uint16_t) color << 8;
}

void _putc(uint8 c) {
	//	Video memory offset
	uint32 offset, i;

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
		//	Scroll the screen up 1 line
		for (i = 0; i < 1920; i++) {
			VIDEO_MEM[i] = VIDEO_MEM[i + 80];
		}

		// clear the last row
		for (i = 1920; i < 2000; i++) {
			VIDEO_MEM[i] = vga_entry(' ', _print_color);
		}

		VIDEO_MEM -= 80;
	}

	//	set cursor new position
	offset >>= 1;
	outb(0x3d4, 0x0f);
	outb(0x3d5, offset & 0x00ff);
	outb(0x3d4, 0x0e);
	outb(0x3d5, offset >> 8);
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