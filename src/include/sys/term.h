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

typedef struct {
	uint32_t foreground;
	uint32_t background;
} term_color_t;

typedef struct {
	uint32_t x;
	uint32_t y;
} cursor_point_t;

typedef enum {
	crshape_hidden = 0,
	crshape_underscore = 1,
	crshape_half_block = 2,
	crshape_block = 3
} enum_crshape_t;

typedef struct {
	enum_crshape_t shape;
	cursor_point_t point;
} cursor_info_t;

typedef struct {
	const term_color_t *(*get_color)(void);
	void (*set_color)(const term_color_t *color);
	const cursor_info_t *(*get_cursor_info)(void);
	void (*set_cursor_info)(const cursor_info_t *info);
	void (*set_cursor_point)(const cursor_point_t *point);
	void (*putc)(char c);
	void (*putc_at)(char c, const cursor_point_t *point);
	void (*puts)(const char *s);
	void (*clear)(void);
} term_api_t;

term_api_t *term_api(void);
void term_default_config(void);
void init_term(term_api_t *api);

#endif  //	__TERM_H_