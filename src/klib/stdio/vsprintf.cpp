/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
/*
	vsprintf - sends formatted output to a string using an argument list passed
	 to it.
	Parameters:
	str		− This is the array of char elements where the resulting string is
			  to be stored.
	format	− This is the C string that contains the text to be written to the
			  str. It can optionally contain embedded format tags that are
			  replaced by the values specified in subsequent additional
			  arguments and are formatted as requested. Format tags prototype −
			   %[flags][width][.precision][length]specifier
--------------------------------------------------------------------------------
	Specifier:

	c			Character
	d or i		Signed decimal integer
	e			Not supported in kernel mode.
	E			Not supported in kernel mode.
	f			Not supported in kernel mode.
	g			Not supported in kernel mode.
	G			Not supported in kernel mode.
	o			Not supported in kernel mode.
	s			String of characters
	u			Unsigned decimal integer
	x			Unsigned hexadecimal integer
	X			Unsigned hexadecimal integer (capital letters)
	p			Pointer address.
	n			Not supported in kernel mode.
	%			% Character
--------------------------------------------------------------------------------
	Flags:

	-			Left-justify within the given field width; Right justification
				is the default
				(see width sub-specifier).
	+			Not supported.
	(space)		Not supported.
	#			Used with x or X specifiers the value is preceded with 0x or 0X
				respectively for values different than zero. Not supported for e
				, E and f.
	0			Left-pads the number with zeroes (0) instead of spaces, where
				padding is specified (see width sub-specifier).
--------------------------------------------------------------------------------
	Width:

	(number)	Minimum number of characters to be printed. If the value to be
				printed is shorter than this number, the result is padded with
				blank spaces. The value is not truncated even if the result is
				larger.
	*			The width is not specified in the format string, but as an
				additional integer value argument preceding the argument that
				has to be formatted.
--------------------------------------------------------------------------------
	.precision:

	.number		For integer specifiers (d, i, x, X) − precision specifies the
				minimum number of digits to be written. If the value to be
				written is shorter than this number, the result is padded with
				leading zeros. The value is not truncated even if the result is
				longer. A precision of 0 means that no character is written for
				the value 0. Not supported e, E and f specifiers. For s − this
				is the maximum number of characters to be printed. By default
				all characters are printed until the ending null character is
				encountered. For c type − it has no effect. When no precision is
				specified, the default is 1. If the period is specified without
				an explicit value for precision, 0 is assumed. If precision has
				been specified, `Width` will be ignored.
	.*			The precision is not specified in the format string, but as an
				additional integer value argument preceding the argument that
				has to be formatted.
--------------------------------------------------------------------------------
	Length:

	h			The argument is interpreted as a short int or unsigned short int
				(only applies to integer specifiers − i, d, o, u, x and X).
	l			The argument is interpreted as a long int or unsigned long int
				for integer specifiers (i, d, o, u, x and X), and as a wide
				character or wide character string for specifiers c and s.
	L			he argument is interpreted as a long double (only applies to
				floating point specifiers − e, E, f, g and G).
*/

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.hpp>
#include <stdlib.hpp>
#include <string.hpp>

using namespace qklib;

#define DIGIT_STR(f) \
	(char *) (f == 'X' ? "0123456789ABCDEF" : "0123456789abcdef")

#define COPY_VA_INT(x)                       \
	do {                                     \
		const int v = abs(va_arg(arg, int)); \
		*(int *) x = v;                      \
		ptr++; /* Go past the asterisk.  */  \
	} while (0)

#define PRINT_CHAR(C)  \
	do {               \
		*value++ = C;  \
		*value = '\0'; \
		i++;           \
	} while (0)

#define BASE(x)                     \
	do {                            \
		switch ((char) x) {         \
		case 'p':                   \
		case 'x':                   \
		case 'X': base = 16; break; \
		case 'o': base = 8; break;  \
		case 't': base = 2; break;  \
		default: base = 10; break;  \
		}                           \
	} while (0)

namespace qklib {

typedef struct format_specifier {
	char specifier;	   // Specifier
#define FMTS_FLAGS_LEFTJUSTIFIED 0x01
#define FMTS_FLAGS_FORCESIGN	 0x02
#define FMTS_FLAGS_FILLSPACE	 0x04
#define FMTS_FLAGS_LEADSIGN		 0x08
#define FMTS_FLAGS_PADZEROES	 0x10
	int flags;	  //	-, +, ' ', #, or 0
	// int	 leftJustified;	   // left justified
	// int	 zeroPad;		   //	zero pad
	// int	 leadingSign;	   //	0x leading
	// int	 forceSign;		   //	Force + sign
	int	 fillWidth;	   //	Width
	int	 precision;	   //	Precision
	char length;	   // h, l, or L
} format_specifier_t;

static const char *FLAGS_STRING = "-+ #0";
static const char *LENGTH_STRING = "hlL";

static int print_number_ul(char *dest, long value, format_specifier_t *format) {

	if (!format->precision && !value)
		return 0;

	char *		p = dest;
	const char *strDigit = DIGIT_STR(format->specifier);
	int			len = 0;
	char		buf1[32] {0};
	int			base;
	bool		bHasSign = value < 0 || format->flags & FMTS_FLAGS_FORCESIGN;

	BASE(format->specifier);
	switch (format->specifier) {
	case 'd':
	case 'i': {
		/* Specifier says this is signed integer */
		/* check the length to find out actual value type */
		/*
		 * we don't want to print any sign to buf1 at this moment
		 * because padding and justification would alter the actual
		 * position of the sign
		 */
		auto v = absl(value);
		if (format->length == 'h') { /* short */
			len += itoa(static_cast<int16_t>(v), buf1, base, strDigit);
		}
		else if (format->length == 'L') { /* long long */
			len += ltoa(v, buf1, base, strDigit);
		}
		else {
			len += itoa(static_cast<int32_t>(v), buf1, base, strDigit);
		}
	} break;
	case 'x':
	case 'X':
	case 'u':
	case 'p': {
		/* Specifier says this is unsigned integer.
		   Force sign is not supported.
		*/
		bHasSign = false;
		/* Check the length to find out actual value type */
		if (format->length == 'h') { /* short */
			len += utoa(static_cast<uint16_t>(value), buf1, base, strDigit);
		}
		else if (format->length == 'L') { /* long long */
			len += ultoa(static_cast<uint64_t>(value), buf1, base, strDigit);
		}
		else {
			len += utoa(static_cast<uint32_t>(value), buf1, base, strDigit);
		}
	} break;
	default: buf1[0] = '\0';
	}

	/*
		If '0' flags specified, Left-pads the number with zeroes instead of
	   spaces, if fillwidth specified.
	*/
	if (format->flags & FMTS_FLAGS_PADZEROES) {
		format->precision = MAX(format->precision, format->fillWidth);
		format->fillWidth = 0;
	}

	/* max_width is taken from precision, width, and len, whichever larger */
	auto max_width = MAX(format->precision, format->fillWidth);
	max_width = MAX(max_width, len);
	auto paddzero = MAX(format->precision - len, 0);
	paddzero = MAX(paddzero, 0);
	auto toBeMinus = MAX(len, format->precision);
	auto paddspace = MAX(format->fillWidth - toBeMinus, 0);
	paddspace = MAX(paddspace, 0);
	auto retVal = max_width;
	auto printed = 0;

	if (len > format->precision && len > format->fillWidth) {
		if (bHasSign) {
			*p++ = value < 0 ? '-' : '+';
			printed++;
			retVal++;
		}
		else if (format->flags & FMTS_FLAGS_LEADSIGN) {
			*p++ = '0';
			*p++ = 'x';
			printed += 2;
			retVal += 2;
		}

		for (auto i = 0; i < max_width; i++) {
			*p++ = buf1[i];
		}
	}
	else if (format->flags & FMTS_FLAGS_LEFTJUSTIFIED) { /* Left justified */

		if (bHasSign) {
			*p++ = value < 0 ? '-' : '+';
			printed++;
			retVal++;
		}
		else if (format->flags & FMTS_FLAGS_LEADSIGN) {
			*p++ = '0';
			*p++ = 'x';
			printed += 2;
			retVal += 2;
		}

		for (auto i = 0; i < paddzero; i++) {
			*p++ = '0';
		}
		for (auto i = 0; i < len; i++) {
			*p++ = buf1[i - paddzero];
		}
		for (auto i = 0; i < (paddspace - printed); i++) {
			*p++ = ' ';
		}
	}
	else { /* right justified */
		if (bHasSign) {
			printed++;
		}
		else if (format->flags & FMTS_FLAGS_LEADSIGN) {
			printed += 2;
		}

		for (auto i = 0; i < (paddspace - printed); i++) {
			*p++ = ' ';
		}
		if (bHasSign) {
			*p++ = value < 0 ? '-' : '+';
			retVal++;
		}
		else if (format->flags & FMTS_FLAGS_LEADSIGN) {
			*p++ = '0';
			*p++ = 'x';
			retVal += 2;
		}

		for (auto i = 0; i < paddzero; i++) {
			*p++ = '0';
		}
		for (auto i = 0; i < len; i++) {
			*p++ = buf1[i];
		}
	}

	*p = '\0';

	return retVal;
}

// static unsigned int print_number_ui(char *				dest,
// 									unsigned int		value,
// 									format_specifier_t *format) {
// 	char * p = dest;
// 	size_t len = 0;
//
// 	char		 buf1[32] = {0};
// 	unsigned int base;
// 	BASE(format->specifier);
// 	len += itoa(value, buf1, base, DIGIT_STR(format->specifier));
// 	int max_width = MAX(MAX(format->fillWidth, format->precision), (int) len);
// 	int paddspace = MAX(format->fillWidth - (int) len, 0);
// 	int paddzero = MAX(format->precision - (int) len, 0);
// 	int i = 0;
// 	int leading = 0;
// 	if (format->precision >= format->fillWidth) {
// 		//	precision greater than fixed width
// 		if ((format->flags & FMTS_FLAGS_LEADSIGN)
// 			&& (format->specifier == 'X' || format->specifier == 'x')) {
// 			*p++ = '0';
// 			*p++ = format->specifier;
// 			*p = '\0';
// 			leading = 2;
// 		}
// 		for (i = 0; i < max_width; i++) {
// 			*p++ = i < paddzero ? '0' : buf1[i - paddzero];
// 			*p = '\0';
// 		}
//
// 		return max_width + leading;
// 	}
//
// 	int c_to_print = MAX(format->precision, (int) len);
// 	if ((format->flags & FMTS_FLAGS_LEFTJUSTIFIED)) {
// 		if ((format->flags & FMTS_FLAGS_LEADSIGN)
// 			&& (format->specifier == 'X' || format->specifier == 'x')) {
// 			*p++ = '0';
// 			*p++ = format->specifier;
// 			*p = '\0';
// 			leading = 2;
// 		}
// 		for (i = 0; i < c_to_print; i++) {
// 			*p++ = i < paddzero ? '0' : buf1[i - paddzero];
// 			*p = '\0';
// 		}
// 		for (i = 0; i < paddspace - paddzero; i++) {
// 			*p++ = ' ';
// 			*p = '\0';
// 		}
// 	}
// 	else {
// 		for (i = 0; i < paddspace - paddzero; i++) {
// 			*p++ = (format->flags & FMTS_FLAGS_PADZEROES) ? '0' : ' ';
// 			*p = '\0';
// 		}
// 		if ((format->flags & FMTS_FLAGS_LEADSIGN)
// 			&& (format->specifier == 'X' || format->specifier == 'x')) {
// 			*p++ = '0';
// 			*p++ = 'x';
// 			*p = '\0';
// 			leading = 2;
// 		}
// 		for (i = 0; i < c_to_print; i++) {
// 			*p++ = i < paddzero ? '0' : buf1[i - paddzero];
// 			*p = '\0';
// 		}
// 	}
//
// 	return max_width + leading;
// }

static unsigned int print_string(char *				 dest,
								 const char *		 str,
								 format_specifier_t *format) {
	size_t len = strlen(str);
	size_t c_to_print = MIN(
		len,
		(unsigned int) format->precision);	  // number of characters to print
	int i;

	if ((int) c_to_print >= format->fillWidth) {
		//	c to print is larger than width, justification not consider
		if (c_to_print == 0) {
			return 0;
		}

		for (i = 0; i < (int) c_to_print; i++) {
			*dest++ = *str++;
			*dest = '\0';
		}

		return c_to_print;
	}

	//	c to print is shorter than width, process justification
	if (format->flags & FMTS_FLAGS_LEFTJUSTIFIED) {
		//	left justified. print c_to_print, then spaces
		for (i = 0; i < format->fillWidth; i++) {
			*dest++ = i < (int) c_to_print ? *str++ : ' ';
			*dest = '\0';
		}
	}
	else {
		//	default - right justified.
		for (i = 0; i < format->fillWidth; i++) {
			*dest++ = i < format->fillWidth - (int) c_to_print ? ' ' : *str++;
			*dest = '\0';
		}
	}

	return format->fillWidth;
}

int vsprintf(char *__restrict str, const char *__restrict format, va_list arg) {
	if (!str || !format) {
		return 0;
	}

	format_specifier_t fmt_spcf = {'\0', 0, -1, -1, '\0'};
	const char *	   ptr = format;
	char *			   value = str;
	unsigned int	   i = 0;
	char			   fsbuf[32] = {0};
	unsigned int	   idx = 0;

	while (*ptr != '\0') {
		if (*ptr != '%') { /* While we have regular characters, print them.  */
			do {
				PRINT_CHAR(*ptr++);
			} while (*ptr != '%' && *ptr != '\0');
		}
		else {
			/* We got a format specifier! */
			ptr++; /* move pass '%'. */
			fmt_spcf.specifier = fmt_spcf.length = '\0';
			fmt_spcf.flags = 0;
			fmt_spcf.fillWidth = fmt_spcf.precision = -1;

			/* Now we check if any flags specified. */
			auto flgIdx = strchr(FLAGS_STRING, *ptr++);
			if (flgIdx) {
				fmt_spcf.flags = 1 << (int) (flgIdx - FLAGS_STRING);
				ptr++;
			}

			/* probe for width */
			if (*ptr == '*') {
				/* Found field specified width, get the width from argument. */
				fmt_spcf.fillWidth = abs(va_arg(arg, int));
				ptr++;
				// COPY_VA_INT(&(fmt_spcf.fillWidth));
			}
			else {
				/* Maybe the width was specified in number form? let try it. */
				idx = 0;
				while (ISDIGIT(*ptr)) /* Handle explicit numeric value.  */
				{
					fsbuf[idx++] = *ptr;
					fsbuf[idx] = '\0';
					ptr++;
				}

				if (idx > 0) {
					/* Yup, is in number form, keep it. */
					fmt_spcf.fillWidth = atoi(fsbuf);
				}
			}

			/* probe for precisions */
			if (*ptr == '.') {
				ptr++; /* go past the period.  */
				if (*ptr == '*') {
					/* Found argument specified precision, get the precision
					 * from argument. */
					fmt_spcf.precision = abs(va_arg(arg, int));
					ptr++;
					// COPY_VA_INT(&(fmt_spcf.precision));
				}
				else {
					/* Maybe precision being specified in number form? */
					// idx = 0;
					////	reset fsbuf
					// while (idx < 32) {
					// 	fsbuf[idx++] = '\0';
					// }
					idx = 0;
					while (ISDIGIT(*ptr)) /* Handle explicit numeric value.  */
					{
						fsbuf[idx++] = *ptr;
						fsbuf[idx] = '\0';
						ptr++;
					}

					if (idx > 0) {
						fmt_spcf.precision = atoi(fsbuf);
					}
				}
			}

			/* probe for length specifier */
			auto lenIdx = strchr(LENGTH_STRING, *ptr++);
			if (lenIdx) {
				fmt_spcf.length = *lenIdx;
				ptr++;
			}

			fmt_spcf.specifier = *ptr;
			switch (*ptr++) {
			case 'd':
			case 'i':
			case 'u':
			case 'x':
			case 'X': {
				// default to int length.
				if (fmt_spcf.length == '\0')
					fmt_spcf.length = 'l';
				const unsigned int v = va_arg(arg, unsigned int);
				char			   num_buff[32] = {0};
				unsigned int	   x = print_number_ul(num_buff, v, &fmt_spcf);
				strcat(value, num_buff);
				value += x;
				i += x;
			} break;
			case 'p': {
				void *	  ptr = va_arg(arg, void *);
				char	  num_buff[32] = {0};
				uintptr_t v = (uintptr_t) ptr;
				fmt_spcf.flags |= FMTS_FLAGS_LEADSIGN | FMTS_FLAGS_PADZEROES;
				fmt_spcf.specifier = 'X';
#if defined(__x86_64__)
				fmt_spcf.precision = 16;
				fmt_spcf.length = 'L';
#else
				fmt_spcf.precision = 8;
				fmt_spcf.length = 'l';
#endif
				unsigned int x = print_number_ul(num_buff, v, &fmt_spcf);
				strcat(value, num_buff);
				value += x;
				i += x;
			} break;
			case 'c': {
				int c = va_arg(arg, int);
				PRINT_CHAR(c);
			} break;
			case 's': {
				const char * s = va_arg(arg, char *);
				unsigned int x = print_string(value, s, &fmt_spcf);
				value += x;
				i += x;
			} break;
			case '%': PRINT_CHAR('%'); break;
			default: return -2;
			} /* End of switch (*ptr) */
		}	  /* End of else statement */
	}

	return i;
}
}	 // namespace qklib