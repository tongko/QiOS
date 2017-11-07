#include "io.h"
#include "tty.h"

static uint16_t		_vga_buffer;
static uint16_t		_ctrc_io;
static uint8_t		term_row;
static uint8_t		term_col;
static uint8_t		term_color;

void term_init(void) {
	term_row = 0;
	term_col = 0;
	term_color = vga_color_entry(COLOR_GREEN, COLOR_BLACK);
	
	//	detect video
	if ((inb(0x3CC) & 0x01) != 0) {
		//	Monochrome emulation
		_vga_buffer = 0xB8000L;
		_ctrc_io = 0x3D4;
	}
	else {
		//	Color VGA emulator
		_vga_buffer = 0xB0000L;
		_ctrc_io = 0x3B4;
	}

	term_clear(term_color);
}

void term_setcursor(uint16_t wPosition) {

}

void term_clear(uint8_t bColor) {
	
}

void term_puts(const unsigned char* sText) {

}
