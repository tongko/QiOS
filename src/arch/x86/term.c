/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
#include <sys/term.h>
#include "asm.h"

static const uint32_t TERM_WIDTH = 80;
static const uint32_t TERM_HEIGHT = 25;
static __volatile__ uint32_t *_video_mem = (uint32_t *)0xB8000;
static term_color_t _color;
static cursor_info_t _info;

static __inline__ uint8_t vga_color(const term_color_t *color) {
	return color->foreground | color->background << 4;
}

static __inline__ uint16_t vga_entry(char c, const term_color_t *color) {
	return (uint16_t)c | ((uint16_t)vga_color(color) << 8);
}

static const term_color_t *get_color(void) {
	return &_color;
}

static void set_color(const term_color_t *color) {
	if (!color) {
		return;
	}

	_color.background = color->background;
	_color.foreground = color->foreground;
}

static const cursor_info_t *get_cursor_info(void) {
	return &_info;
}

static void set_cursor_xy(uint32_t x, uint32_t y) {
	uint32_t pos = y * TERM_WIDTH + x;

	_outb(0x3D4, 0x0F);
	_outb(0x3D5, (uint8_t)(pos & 0xFF));

	_outb(0x3D4, 0x0E);
	_outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));

	_info.point.x = x;
	_info.point.y = y;
}

void cursor_shape(uint8_t scanStart, uint8_t scanEnd) {
	if (scanStart < 0) {
		_outb(0x3D4, 0x0A);
		_outb(0x3D5, 0x20);
		return;
	}

	_outb(0x3D4, 0x0A);
	_outb(0x3D5, (_inb(0x3D5) & 0xC0) | scanStart);

	_outb(0x3D4, 0x0B);
	_outb(0x3D5, (_inb(0x3E0) & 0xE0) | scanEnd);
}

static void set_cursor_info(const cursor_info_t *info) {
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

static const cursor_point_t set_cursor_point(const cursor_point_t *point) {
	if (!point) {
		return;
	}

	set_cursor_xy(point->x, point->y);
}

static void putc(char c) {
	cursor_point_t point = _info.point;
	uint32_t offset = point.y * TERM_WIDTH + point.x;

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
		    "mov edi, 0xC00B8000;\n"  // dest idx to start of video
		    "mov esi, 0xC00B80A0;\n"  // src idx to 2nd row
		    "mov ecx, 0x780;\n"       // prepare to loop for 1920 times.
		    "rep movsw;\n"            // loop
		    "mov edi, 0xC00B8F00;\n"  // 25th row to dest idx
		    "mov eax, 32;\n"          // space
		    "mov ecx, 80;\n"          // prepare to loop 80 times
		    "rep stosw;\n"            // loop
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

static void putc_at(char c, const cursor_point_t *point) {
	set_cursor_point(point);
	putc(c);
}

static void puts(const char *s) {
	if (!s) {
		return;
	}

	for (uint32_t i = 0; i < strlen(s); i++) {
		putc(s[i]);
	}
}

static void clear() {
	for (int32_t i = 0; i < 2000; i++) {
		putc(' ');
	}
}

void term_default_config() {
	term_color_t color = {0x03, 0x00};  // green, black
	term_api.set_color(&color);
	cursor_point_t point = {0, 0};                // 0, 0
	cursor_info_t info = {crshape_block, point};  // enable cursor block
	term_api.set_cursor_info(&info);

	term_api.clear();
}

void init_term(term_api_t *api) {
	if (api) {
		term_api.get_color = api->get_color;
		term_api.set_color = api->set_color;
		term_api.get_cursor_info = api->get_cursor_info;
		term_api.set_cursor_info = api->set_cursor_info;
		term_api.set_cursor_point = api->set_cursor_point;
		term_api.putc = api->putc;
		term_api.putc_at = api->putc_at;
		term_api.puts = api->puts;
		term_api.clear = api->clear;

		return;
	}

	term_api.get_color = get_color;
	term_api.set_color = set_color;
	term_api.get_cursor_info = get_cursor_info;
	term_api.set_cursor_info = set_cursor_info;
	term_api.set_cursor_point = set_cursor_point;
	term_api.putc = putc;
	term_api.putc_at = putc_at;
	term_api.puts = puts;
	term_api.clear = clear;
}