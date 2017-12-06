/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/term.h>

#define PRINT_TYPE(TYPE)                                  \
	do {                                                  \
		int32_t result;                                   \
		TYPE value = va_arg(arg, TYPE);                   \
		*sptr++ = *ptr++; /* Copy the type specifier.  */ \
		*sptr = '\0';     /* NULL terminate sptr.  */     \
		result = fprintf(stream, specifier, value);       \
		if (result == -1)                                 \
			return -1;                                    \
		else {                                            \
			total_printed += result;                      \
			continue;                                     \
		}                                                 \
	} while (0)

int32_t sprintf(unsigned char *str, const char *format, ...) {
	va_list arg;
	va_start(arg, format);
	vsprintf(&str, format, arg);
	va_end(arg);
}

int32_t printf(const string_t *format, ...) {
	unsigned char buffer[1024] = {0};
	string_t str;
	makestr(&str, buffer);

	va_list arg;
	va_start(arg, format);
	vsprintf(&str, format, arg);
	va_end(arg);

	int32_t i = 0;
	while (buffer[i] != '\0') {
		term.putc(buffer[i++]);
	}
}