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

static term_color_t color;
static cursor_info_t info;

static const term_color_t get_color(void) {
}
static void set_color(const term_color_t *color) {
}
static const cursor_info_t get_cursor_info(void) {
}
static void set_cursor_info(const cursor_info_t *info) {
	if (!info) {
		return;
	}
}
static const cursor_point_t set_cursor_point(const cursor_point_t *point) {
}
static void putc(char c) {
}
static void putc_at(char c, const cursor_point_t *point) {
}
static void puts(const string_t *s) {
}

void init_term(term_api_t *api) {
	if (api) {
		term.get_color = api->get_color;
		term.set_color = api->set_color;
		term.get_cursor_info = api->get_cursor_info;
		term.set_cursor_info = api->set_cursor_info;
		term.set_cursor_point = api->set_cursor_point;
		term.putc = api->putc;
		term.putc_at = api->putc_at;
		term.puts = api->puts;

		return;
	}

	term.get_color = get_color;
	term.set_color = set_color;
	term.get_cursor_info = get_cursor_info;
	term.set_cursor_info = set_cursor_info;
	term.set_cursor_point = set_cursor_point;
	term.putc = putc;
	term.putc_at = putc_at;
	term.puts = puts;
}