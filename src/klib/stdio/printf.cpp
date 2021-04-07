/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2020 Liew Tze Wei                                            *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
/*******************************************************************************
	stdio.h
	function:		printf
	Description:	Sends formatted output to stdout.
	Declaration:	int printf(const char *format, ...)
	Parameters:
		− format − This is the string that contains the text to be written to
		  stdout. It can optionally contain embedded format tags that are
		  replaced by the values specified in subsequent additional arguments
		  and formatted as requested. Format tags prototype is

			%[flags][width][.precision][length]specifier,

		  which is explained below −
+-------+-----------------------------------------------------------+
| Sr.No |            Specifier & Output                             |
+-------+-----------------------------------------------------------+
| 1.    | c                                                         |
|       | Character                                                 |
+-------+-----------------------------------------------------------+
| 2.    | d or i                                                    |
|       | Signed decimal integer                                    |
+-------+-----------------------------------------------------------+
| 3.    | e                                                         |
|       | Scientific notation (mantissa/exponent) using e character |
+-------+-----------------------------------------------------------+
| 4.    | E                                                         |
|       | Scientific notation (mantissa/exponent) using E character |
+-------+-----------------------------------------------------------+
| 5.    | f                                                         |
|       | Decimal floating point                                    |
+-------+-----------------------------------------------------------+
| 6.    | g                                                         |
|       | Uses the shorter of %e or %f                              |
+-------+-----------------------------------------------------------+
| 7.    | G                                                         |
|       | Uses the shorter of %E or %f                              |
+-------+-----------------------------------------------------------+
| 8.    | o                                                         |
|       | Signed octal                                              |
+-------+-----------------------------------------------------------+
| 9.    | s                                                         |
|       | String of characters                                      |
+-------+-----------------------------------------------------------+
| 10.   | u                                                         |
|       | Unsigned decimal interger                                 |
+-------+-----------------------------------------------------------+
| 11.   | x                                                         |
|       | Unsigned hexadecimal integer                              |
+-------+-----------------------------------------------------------+
| 12.   | X                                                         |
|       | Unsigned hexadecimal integer (Capital letters)            |
+-------+-----------------------------------------------------------+
| 13.   | p                                                         |
|       | Pointer address                                           |
+-------+-----------------------------------------------------------+
| 14.   | n                                                         |
|       | nothing printed                                           |
+-------+-----------------------------------------------------------+
| 15.   | %                                                         |
|       | Character                                                 |
+-------+-----------------------------------------------------------+


+-------+-----------------------------------------------------------+
| Sr.No |            Flags & Description                            |
+-------+-----------------------------------------------------------+
| 1.    | -                                                         |
|       | Left-justify within the given field width; Right          |
|       | justification is the default (see width sub-specifier).   |
+-------+-----------------------------------------------------------+
| 2.    | +                                                         |
|       | Forces to precede the result with a plus or minus sign (+ |
|       | or -) even for positive numbers. By default, only negative|
|       | numbers are preceded with a -ve sign.                     |
+-------+-----------------------------------------------------------+
| 3.    | (space)                                                   |
|       | If no sign is going to be written, a blank space is       |
|       | inserted before the value.                                |
+-------+-----------------------------------------------------------+
| 4.    | #                                                         |
|       | Used with o, x or X specifiers the value is preceded with |
|       | 0, 0x or 0X respectively for values different than zero.  |
|       | Used with e, E and f, it forces the written output to     |
|       | contain a decimal point even if no digits would follow. By|
|       | default, if no digits follow, no decimal point is written.|
|       | Used with g or G the result is the same as with e or E but|
|       | trailing zeros are not removed.                           |
+-------+-----------------------------------------------------------+
| 5.    | 0                                                         |
|       | Left-pads the number with zeroes (0) instead of spaces,   |
|       | where padding is specified (see width sub-specifier).     |
+-------+-----------------------------------------------------------+


+-------+-----------------------------------------------------------+
| Sr.No |            Width & Description                            |
+-------+-----------------------------------------------------------+
| 1.    | (number)                                                  |
|       | Minimum number of characters to be printed. If the value  |
|       | to be printed is shorter than this number, the result is  |
|       | padded with blank spaces. The value is not truncated even |
|       | if the result is larger.                                  |
+-------+-----------------------------------------------------------+
| 2.    | *                                                         |
|       | The width is not specified in the format string, but as an|
|       | additional integer value argument preceding the argument  |
|       | that has to be formatted.                                 |
+-------+-----------------------------------------------------------+


+-------+-----------------------------------------------------------+
| Sr.No |            .precision & Description                       |
+-------+-----------------------------------------------------------+
| 1.    | .number                                                   |
|       | For integer specifiers (d, i, o, u, x, X) − precision     |
|       | specifies the minimum number of digits to be written. If  |
|       | the value to be written is shorter than this number, the  |
|       | result is padded with leading zeros. The value is not     |
|       | truncated even if the result is longer. A precision of 0  |
|       | means that no character is written for the value 0. For e,|
|       | E and f specifiers − this is the number of digits to be   |
|       | printed after the decimal point. For g and G specifiers − |
|       | This is the maximum number of significant digits to be    |
|       | printed. For s − this is the maximum number of characters |
|       | to be printed. By default all characters are printed until|
|       | the ending null character is encountered. For c type − it |
|       | has no effect. When no precision is specified, the default|
|       | is 1. If the period is specified without an explicit value|
|       | for precision, 0 is assumed.                              |
+-------+-----------------------------------------------------------+
| 2.    | .*                                                        |
|       | The precision is not specified in the format string, but  |
|       | as an additional integer value argument preceding the     |
|       | argument that has to be formatted.                        |
+-------+-----------------------------------------------------------+


+-------+-----------------------------------------------------------+
| Sr.No |            Length & Description                           |
+-------+-----------------------------------------------------------+
| 1.    | h                                                         |
|       | The argument is interpreted as a short int or unsigned    |
|       | short int (only applies to integer specifiers: i, d, o, u,|
|       | x and X).                                                 |
+-------+-----------------------------------------------------------+
| 2.    | l                                                         |
|       | The argument is interpreted as a long int or unsigned long|
|       | int for integer specifiers (i, d, o, u, x and X), and as a|
|       | wide character or wide character string for specifiers c  |
|       | and s.                                                    |
+-------+-----------------------------------------------------------+
| 3.    | L                                                         |
|       | The argument is interpreted as a long double (only applies|
|       | to floating point specifiers: e, E, f, g and G).          |
+-------+-----------------------------------------------------------+

	additional arguments − Depending on the format string, the function may
	expect a sequence of additional arguments, each containing one value to be
	inserted instead of each %-tag specified in the format parameter (if any).
	There should be the same number of these arguments as the number of %-tags
	that expect a value.

	Return value
	If successful, the total number of characters written is returned. On
	failure, a negative number is returned.

*******************************************************************************/
#include <kernel/KTextMode.hpp>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.hpp>

#ifdef MAXLEN
	#undef MAXLEN
#endif
#define MAXLEN 4096

namespace qklib {

int printf(const char *__restrict format, ...) {
	char buffer[MAXLEN] = {0};

	va_list arg;
	va_start(arg, format);
	uint32_t ret = vsprintf(buffer, format, arg);
	va_end(arg);

	qkrnl::TextModePutString(buffer);
	// uint32_t i = 0;
	// while (buffer[i] != '\0') {
	// 	putchar(buffer[i]);
	// }

	return ret;
}
}	 // namespace qklib