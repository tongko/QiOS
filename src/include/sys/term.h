/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
#ifndef __TERM_H_
#define __TERM_H_

#include <stdint.h>
#include <string.h>

typedef struct term_color {
	uint32_t foreground;
	uint32_t background;
} term_color_t;

typedef struct cursor_point {
	uint32_t x;
	uint32_t y;
} cursor_point_t;

enum enum_cursor_shape {
	hidden = 0,
	underscore = 1,
	half_block = 2,
	block = 3
};

typedef struct cursor_info {
	enum_cursor_shape shape;
	cursor_point_t point;
} cursor_info_t;

typedef struct term_api {
	const term_color_t (*get_color)(void);
	void (*set_color)(const term_color_t *color);
	const cursor_info_t (*get_cursor_info)(void);
	void (*set_cursor_info)(const cursor_info_t *info);
	const cursor_point_t (*set_cursor_point)(const cursor_point_t *point);
	void (*putc)(char c);
	void (*putc_at)(char c, const cursor_point_t *point);
	void (*puts)(const string_t *s);
} term_api_t;

term_api_t term;

void init_term(term_api_t *api);

#endif  //	__TERM_H_