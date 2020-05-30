/*== QiOS =====================================================================+
|                                                                              |
|    Author: Liew Tze Wei                                                      |
|                                                                              |
|    This file is part of the QiOS kernel, and is made available under the     |
|    terms of The Unlicense (That means just do whatever you want with the     |
|    code base).                                                               |
|                                                                              |
+=============================================================================*/
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//   vsprintf.c                                                               //
//   -  QiOS LibK vsprintf function implementation.                           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/* -----------------------------------------------------------------------------
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
	p			Not supported in kernel mode.
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
				an explicit value for precision, 0 is assumed.
	.*			The precision is not specified in the format string, but as an
				additional integer value argument preceding the argument that
				has to be formatted.
--------------------------------------------------------------------------------
	Length:

	h			Not supported.
	l			Not supported.
	L			Not supported.
----------------------------------------------------------------------------- */

#if !(__x86_64__)
	#error \
		"[QiOS (64 bits) - vsprintf.cpp] requires x86_64 architecture. Please use the right compiler options."
#endif

#if !(__is_kernel__)
	#error "[QiOS (64 bits) - vsprintf.cpp] requires __IS_KERNEL, define 1."
#endif
////////////////////////////////////////////////////////////////////////////////
//    IMPLEMENTATION HEADERS                                                  //
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
namespace std {
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
//    PRIVATE DEFINES AND TYPES                                               //
////////////////////////////////////////////////////////////////////////////////

typedef struct format_specifier {
	char		 s;		// Specifier
	unsigned int lj;	// left justified
	unsigned int fz;	// zero pad
	unsigned int hx;	// 0x leading
	int			 fw;
	int			 pw;
} format_specifier_t;

#define DIGIT_STR(f) \
	(char *) (f == 'X' ? "0123456789ABCDEF" : "0123456789abcdef")

#define COPY_VA_INT(x)                               \
	do {                                             \
		const int v = abs(va_arg(arg, int)); \
		*(int *) x	= v;                         \
		ptr++; /* Go past the asterisk.  */          \
	} while (0)

#define PRINT_CHAR(C)    \
	do {                 \
		*value++ = C;    \
		*value	 = '\0'; \
		i++;             \
	} while (0)

#define BASE(x)                     \
	do {                            \
		switch ((char) x) {         \
		case 'x':                   \
		case 'X': base = 16; break; \
		case 'o': base = 8; break;  \
		case 't': base = 2; break;  \
		default: base = 10; break;  \
		}                           \
	} while (0)

#if defined(MAX)
	#undef MAX
#endif
#define MAX(a, b)               \
	({                          \
		__typeof__(a) _a = (a); \
		__typeof__(b) _b = (b); \
		_a > _b ? _a : _b;      \
	})
#if defined(MIN)
	#undef MIN
#endif
#define MIN(a, b)               \
	({                          \
		__typeof__(a) _a = (a); \
		__typeof__(b) _b = (b); \
		_a < _b ? _a : _b;      \
	})

////////////////////////////////////////////////////////////////////////////////
//    PRIVATE MEMBER                                                          //
////////////////////////////////////////////////////////////////////////////////

static int
print_number(char *dest, int value, format_specifier_t *format) {
	char * p   = dest;
	size_t len = 0;

	char	 buf1[32] = {0};
	int base;
	BASE(format->s);
	len += itoa(value, buf1, base, DIGIT_STR(format->s));
	int max_width = MAX(MAX(format->fw, format->pw), (int) len);
	int paddspace = MAX(format->fw - (int) len, 0);
	int paddzero  = MAX(format->pw - (int) len, 0);
	int i		  = 0;
	int leading	  = 0;
	if (format->pw >= format->fw) {
		//	precision greater than fixed width
		if (format->hx && (format->s == 'X' || format->s == 'x')) {
			*p++	= '0';
			*p++	= format->s;
			*p		= '\0';
			leading = 2;
		}
		for (i = 0; i < max_width; i++) {
			*p++ = i < paddzero ? '0' : buf1[i - paddzero];
			*p	 = '\0';
		}

		return max_width + leading;
	}

	int c_to_print = MAX(format->pw, (int) len);
	if (format->lj) {
		if (format->hx && (format->s == 'X' || format->s == 'x')) {
			*p++	= '0';
			*p++	= format->s;
			*p		= '\0';
			leading = 2;
		}
		for (i = 0; i < c_to_print; i++) {
			*p++ = i < paddzero ? '0' : buf1[i - paddzero];
			*p	 = '\0';
		}
		for (i = 0; i < paddspace - paddzero; i++) {
			*p++ = ' ';
			*p	 = '\0';
		}
	}
	else {
		for (i = 0; i < paddspace - paddzero; i++) {
			*p++ = ' ';
			*p	 = '\0';
		}
		if (format->hx && (format->s == 'X' || format->s == 'x')) {
			*p++	= '0';
			*p++	= format->s;
			*p		= '\0';
			leading = 2;
		}
		for (i = 0; i < c_to_print; i++) {
			*p++ = i < paddzero ? '0' : buf1[i - paddzero];
			*p	 = '\0';
		}
	}

	return max_width + leading;
}

static int
print_string(char *dest, const char *str, format_specifier_t *format) {
	size_t len = strlen(str);
	size_t c_to_print
		= MIN(len, (int) format->pw);	  // number of characters to print
	int i;

	if ((int) c_to_print >= format->fw) {
		//	c to print is larger than width, justification not consider
		if (c_to_print == 0) {
			return 0;
		}

		for (i = 0; i < (int) c_to_print; i++) {
			*dest++ = *str++;
			*dest	= '\0';
		}

		return c_to_print;
	}

	//	c to print is shorter than width, process justification
	if (format->lj) {
		//	left justified. print c_to_print, then spaces
		for (i = 0; i < format->fw; i++) {
			*dest++ = i < (int) c_to_print ? *str++ : ' ';
			*dest	= '\0';
		}
	}
	else {
		//	default - right justified.
		for (i = 0; i < format->fw; i++) {
			*dest++ = i < format->fw - (int) c_to_print ? ' ' : *str++;
			*dest	= '\0';
		}
	}

	return format->fw;
}

////////////////////////////////////////////////////////////////////////////////
//    INTERFACE IMPLEMENTATIONS                                               //
////////////////////////////////////////////////////////////////////////////////

int vsprintf(char *str, const char *format, va_list arg) {
	if (!str || !format) {
		return 0;
	}

	format_specifier_t fmt_spcf = {'\0', 0, 0, 0, -1, -1};
	const char *	   ptr		= format;
	char *			   value	= str;
	int				   i		= 0;

	while (*ptr != '\0') {
		if (*ptr != '%') { /* While we have regular characters, print them.  */
			do {
				PRINT_CHAR(*ptr++);
			} while (*ptr != '%' && *ptr != '\0');
		}
		else {
			/* We got a   format specifier! */
			ptr++; /* move pass '%'. */
			fmt_spcf.s	= '\0';
			fmt_spcf.lj = fmt_spcf.fz = fmt_spcf.hx = 0;
			fmt_spcf.fw = fmt_spcf.pw = -1;

			while (strchr("-#0", *ptr)) { /* Move past flags.  */
				if (*ptr == '-') {
					fmt_spcf.lj = 1;
				}
				if (*ptr == '0') {
					fmt_spcf.fz = 1;
				}
				if (*ptr == '#') {
					fmt_spcf.hx = 1;
				}
				ptr++;
			}

			char fsbuf[32] = {0};
			int	 idx	   = 0;
			if (*ptr == '*')
				COPY_VA_INT(&(fmt_spcf.fw));
			else {
				idx = 0;
				while (ISDIGIT(*ptr)) /* Handle explicit numeric value.  */
				{
					fsbuf[idx++] = *ptr;
					fsbuf[idx]	 = '\0';
					ptr++;
				}

				if (idx > 0) {
					fmt_spcf.fw = atoi(fsbuf);
				}
			}

			if (*ptr == '.') {
				ptr++; /* Copy and go past the period.  */
				if (*ptr == '*')
					COPY_VA_INT(&(fmt_spcf.pw));
				else {
					idx = 0;
					//	reset fsbuf
					while (idx < 32) {
						fsbuf[idx++] = '\0';
					}
					idx = 0;
					while (ISDIGIT(*ptr)) /* Handle explicit numeric value.  */
					{
						fsbuf[idx++] = *ptr;
						fsbuf[idx]	 = '\0';
						ptr++;
					}

					fmt_spcf.pw = atoi(fsbuf);
				}
			}

			fmt_spcf.s = *ptr;
			switch (*ptr++) {
			case 'd':
			case 'i':
			case 'u':
			case 'x':
			case 'X': {
				const int v			   = va_arg(arg, int);
				char	  num_buff[32] = {0};
				int		  x			   = print_number(num_buff, v, &fmt_spcf);
				strcat(value, num_buff);
				value += x;
				i += x;
			} break;
			case 'c': {
				char c = va_arg(arg, char);
				PRINT_CHAR(c);
			} break;
			case 's': {
				const char *s = va_arg(arg, char *);
				int			x = print_string(value, s, &fmt_spcf);
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

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//    End vsprintf.c                                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}	 // namespace std
}
#endif