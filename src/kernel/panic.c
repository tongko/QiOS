/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
#include <sys/panic.h>
#include <asm.h>
#include <karg.h>
#include <kio.h>
#include <sys/term.h>

extern void system_halt(void);
void kernel_panic(const char *format, ...) {
	//	First, stop all hardware interrupt.
	cli();

	va_list args;
	static char buffer[4096];
	va_start(args, format);

	vsprintf(buffer, format, args);

	va_end(args);

	char *disclaimer =
	    "System encountered unrecoverable problem and is shutting down\n\
to prevent damage to your computer. Any unsaved work might be lost.\n\
We are sorry for the inconvenience caused.\n\n\
Please report the following information and restart your computer.\n\
System halted.\n\n";

	term_color_t color = {1, 15};
	term_api()->set_color(&color);
	term_api()->clear();
	cursor_point_t cxy = {0, 0};
	term_api()->set_cursor_point(&cxy);
	term_api()->puts(disclaimer);
	term_api()->puts("[STOP]: ");
	term_api()->puts(buffer);

	system_halt();
}