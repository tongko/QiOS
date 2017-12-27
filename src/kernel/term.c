/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
#include <asm.h>
#include <attribs.h>
#include <sys/term.h>

#define VIDEO_MEM (uint16_t *)0xB8000

static __earlydata term_api_t _api;
static __earlydata uint32_t _term_width;
//static const uint32_t TERM_HEIGHT = 25;
static __earlydata volatile uint16_t *_video_mem;
static __earlydata term_color_t _color;
static __earlydata cursor_info_t _info;

static __early __inline__ uint8_t vga_color(const term_color_t *color) {
	return color->foreground | color->background << 4;
}

static __early __inline__ uint16_t vga_entry(char c, const term_color_t *color) {
	return (uint16_t)c | ((uint16_t)vga_color(color) << 8);
}

static __early const term_color_t *get_color(void) {
	return &_color;
}

static __early void set_color(const term_color_t *color) {
	if (!color) {
		return;
	}

	_color.background = color->background;
	_color.foreground = color->foreground;
}

static __early const cursor_info_t *get_cursor_info(void) {
	return &_info;
}

static __early void set_cursor_xy(uint32_t x, uint32_t y) {
	uint32_t pos = y * _term_width + x;

	_outb(0x3D4, 0x0F);
	_outb(0x3D5, (uint8_t)(pos & 0xFF));

	_outb(0x3D4, 0x0E);
	_outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));

	_info.point.x = x;
	_info.point.y = y;
}

static __early void cursor_shape(uint8_t scanStart, uint8_t scanEnd) {
	if (scanStart == 0 && scanEnd == 0) {
		// Disable cursor
		_outb(0x3D4, 0x0A);
		_outb(0x3D5, 0x20);
		return;
	}

	_outb(0x3D4, 0x0A);
	_outb(0x3D5, (_inb(0x3D5) & 0xC0) | scanStart);

	_outb(0x3D4, 0x0B);
	_outb(0x3D5, (_inb(0x3E0) & 0xE0) | scanEnd);
}

static __early void set_cursor_info(const cursor_info_t *info) {
	if (!info) {
		return;
	}

	set_cursor_xy(info->point.x, info->point.y);
	uint8_t s, e;
	switch (info->shape) {
		case crshape_hidden:
			s = -1, e = 0;
			break;
		case crshape_underscore:
			s = 15, e = 15;
			break;
		case crshape_half_block:
			s = 8, e = 15;
			break;
		default:
			s = 0, e = 15;
			break;
	}

	cursor_shape(s, e);
	_info.shape = info->shape;
}

static __early void set_cursor_point(const cursor_point_t *point) {
	if (!point) {
		return;
	}

	set_cursor_xy(point->x, point->y);
}

static __early void putc(char c) {
	cursor_point_t point = _info.point;
	uint32_t offset = point.y * _term_width + point.x;

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
			_video_mem[offset] = vga_entry(' ', &_color);
			break;
		default:
			_video_mem[offset++] = vga_entry(c, &_color);
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
	point.x = offset % 80;
	point.y = (offset / 80);
	set_cursor_point(&point);
}

static __early void putc_at(char c, const cursor_point_t *point) {
	set_cursor_point(point);
	putc(c);
}

static __early void puts(const char *s) {
	if (!s) {
		return;
	}

	for (uint32_t i = 0; i < strlen(s); i++) {
		putc(s[i]);
	}
}

static __early void clear() {
	for (int32_t i = 0; i < 2000; i++) {
		putc(' ');
	}
	//	place cursor to left top
	cursor_point_t point = {0, 0};
	_api.set_cursor_point(&point);
}

void term_default_config() {
	_video_mem = VIDEO_MEM;
	_term_width = 80;
	term_color_t color = {0x02, 0x00};  // green, black
	_api.set_color(&color);
	cursor_point_t point = {0, 0};                // 0, 0
	cursor_info_t info = {crshape_block, point};  // enable cursor block
	_api.set_cursor_info(&info);

	_api.clear();
}

term_api_t *term_api() {
	return &_api;
}

void term_init(term_api_t *api) {
	if (api != NULL) {
		_api.get_color = api->get_color;
		_api.set_color = api->set_color;
		_api.get_cursor_info = api->get_cursor_info;
		_api.set_cursor_info = api->set_cursor_info;
		_api.set_cursor_point = api->set_cursor_point;
		_api.putc = api->putc;
		_api.putc_at = api->putc_at;
		_api.puts = api->puts;
		_api.clear = api->clear;

		return;
	}

	_api.get_color = get_color;
	_api.set_color = set_color;
	_api.get_cursor_info = get_cursor_info;
	_api.set_cursor_info = set_cursor_info;
	_api.set_cursor_point = set_cursor_point;
	_api.putc = putc;
	_api.putc_at = putc_at;
	_api.puts = puts;
	_api.clear = clear;
}