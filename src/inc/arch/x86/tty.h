#ifndef		__ARC_X86_TTY_
#define		__ARC_X86_TTY_

#include	<stdint.h>

typedef enum {
	COLOR_BLACK = 0,
	COLOR_BLUE = 1,
	COLOR_GREEN = 2,
	COLOR_CYAN = 3,
	COLOR_RED = 4,
	COLOR_MAGENTA = 5,
	COLOR_BROWN = 6,
	COLOR_LIGHT_GREY = 7,
	COLOR_DARK_GREY = 8,
	COLOR_LIGHT_BLUE = 9,
	COLOR_LIGHT_GREEN = 10,
	COLOR_LIGHT_CYAN = 11,
	COLOR_LIGHT_RED = 12,
	COLOR_LIGHT_MAGENTA = 13,
	COLOR_LIGHT_BROWN = 14,
	COLOR_WHITE = 15,
} VGA_COLOR;

static inline uint8_t vga_color_entry(VGA_COLOR foreground, VGA_COLOR background) {
	return	foreground | background << 4;
}

static inline uint16 vga_entry(unsigned char c, uint8_t color) {
	return (uint16_t) c | (uint16_t) color << 8;
}

void term_init(void);
void term_setcursor(uint16_t);
void term_clear(uint8_t);
void term_puts(const char*);


#endif	//	__ARC_X86_TTY_