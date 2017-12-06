#include <stdio.h>
#include <stdlib.h>

#define COPY_VA_INT(x)                               \
	do {                                             \
		const int32_t v = abs(va_arg(arg, int32_t)); \
		*(int32_t *)x = v;                           \
		unsigned char buf[32];                       \
		ptr++;        /* Go past the asterisk.  */   \
		*sptr = '\0'; /* NULL terminate sptr.  */    \
		itoa(v, buf, 10);                            \
		strcat(sptr, buf);                           \
		while (*sptr)                                \
			sptr++;                                  \
	} while (0)

#define PRINT_CHAR(C)    \
	do {                 \
		value[i++] = C;  \
		value[i] = '\0'; \
		ptr++;           \
	} while (0)

typedef struct format_specifier {
	int32_t lj;  // left justified
	int32_t fz;  //	zero pad
	int32_t hx;  //	0x leading
	int32_t fw;
	int32_t pw;
} format_specifier_t;

static int32_t print_hex(unsigned char *dest, uint32_t value, format_specifier_t *format) {
	unsigned char buf1[32] = {0};
	size_t len = itoa(value, buf1, 16);
	int32_t paddspace = format->fw - len;
	if (paddspace < 0) {
		paddspace = 0;
	}
	int32_t paddzero = format->pw - len;
	if (paddzero < 0) {
		paddzero = 0;
	}
	int32_t fixed_width = paddspace > 0 ? format->fw : len;
	if (paddzero > fixed_width) {
		fixed_width = paddzero;
	}
	int32_t i, j = 0;
	unsigned char buf2[32] = {0};
	for (i = 0; i < paddzero + len; i++) {
		buf2[i] = i < paddzero ? '0' : buf1[j++];
	}

	if (format->lj) {
		if (format->hx) {
			*dest++ = '0';
			*dest++ = 'x';
			fixed_width -= 2;
		}
		for (i = 0; i < fixed_width; i++) {
			*dest++ = i < paddzero + len ? buf2[i] : ' ';
		}
	} else {
		for (i = 0; i < fixed_width; i++) {
		}
	}
}

static int32_t print_int(unsigned char *dest, uint32_t value, format_specifier_t *format) {
	unsigned char buf1[32] = {0};
	size_t len = itoa(value, buf1, 10);
	int32_t paddspace = format->fw - len;
	int32_t paddzero = format->pw - len;

	if (paddspace > 0 || paddzero > 0) {  // shorter than min width
		if (paddzero >= paddspace) {
			for (int32_t i = 0; i < paddzero + len; i++) {
				*dest++ = i >= paddzero ? buf1[i - paddzero] : '0';
			}
		} else {
			unsigned char buf2[32] = {0};
			int32_t i, j = 0;
			for (i = 0; i < paddzero + len; i++) {
				buf2[i] = i < paddzero ? '0' : buf1[j++];
			}

			if (format->lj) {  // left-justified
				for (i = 0; i < format->fw; i++) {
					*dest++ = i < paddzero + len ? buf2[i] : ' ';
				}
			} else {  // right justified (default)
				for (i = 0; i < format->fw; i++) {
					*dest++ = i < format->fw - (paddzero + len) ? ' ' : buf2[i];
				}
			}
		}

		return format->fw > format->pw ? format->fw : format->pw;
	}

	for (int32_t i = 0; i < len; i++) {
		*dest++ = buf1[i];
	}

	return len;
}
/*
	vsprintf - sends formatted output to a string using an argument list passed to it.
	Parameters:
	str		− This is the array of char elements where the resulting string is to be stored.
	format	− This is the C string that contains the text to be written to the str. It can
			  optionally contain embedded format tags that are replaced by the values 
			  specified in subsequent additional arguments and are formatted as requested. 
			  Format tags prototype − %[flags][width][.precision][length]specifier
---------------------------------------------------------------------------------------------
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
---------------------------------------------------------------------------------------------
	Flags:

	-			Left-justify within the given field width; Right justification is the default
				(see width sub-specifier).
	+			Not supported.
	(space)		Not supported.
	#			Used with x or X specifiers the value is preceded with 0x or 0X 
				respectively for values different than zero. Not supported for e, E and f. 
	0			Left-pads the number with zeroes (0) instead of spaces, where padding is 
				specified (see width sub-specifier).
---------------------------------------------------------------------------------------------
	Width:

	(number)	Minimum number of characters to be printed. If the value to be printed is 
				shorter than this number, the result is padded with blank spaces. The value is 
				not truncated even if the result is larger.
	*			The width is not specified in the format string, but as an additional integer 
				value argument preceding the argument that has to be formatted.
---------------------------------------------------------------------------------------------
	.precision:

	.number		For integer specifiers (d, i, x, X) − precision specifies the minimum 
				number of digits to be written. If the value to be written is shorter than 
				this number, the result is padded with leading zeros. The value is not 
				truncated even if the result is longer. A precision of 0 means that no 
				character is written for the value 0. Not supported e, E and f specifiers.
				For s − this is the maximum number of characters to be printed. By default 
				all characters are printed until the ending null character is encountered. For
				c type − it has no effect. When no precision is specified, the default is 1. 
				If the period is specified without an explicit value for precision, 0 is 
				assumed.
	.*			The precision is not specified in the format string, but as an additional 
				integer value argument preceding the argument that has to be formatted.
---------------------------------------------------------------------------------------------
	Length:

	h			Not supported.
	l			Not supported.
	L			Not supported.
*/
int32_t vsprintf(string_t *str, const string_t *format, va_list arg) {
	if (!str || !str->value || !format) {
		return 0;
	}

	format_specifier_t fmt_spcf;
	const char *ptr = format;
	unsigned char *value = str->value;
	unsigned char specifier[128];
	int32_t total_printed = 0;
	int32_t i = 0;

	while (*ptr != '\0') {
		if (*ptr != '%') { /* While we have regular characters, print them.  */
			value[i++] = *ptr++;
			value[i] = '\0';
		} else { /* We got a format specifier! */
			unsigned char *sptr = specifier;
			int32_t wide_width = 0, short_width = 0;

			*sptr++ = *ptr++; /* Copy the % and move forward.  */

			fmt_spcf.lj = 0;
			while (strchr("-#0", *ptr)) { /* Move past flags.  */
				fmt_spcf.lj = *ptr == '-' ? 1 : fmt_spcf.lj;
				fmt_spcf.fz = *ptr == '0' ? 1 : fmt_spcf.fz;
				fmt_spcf.hx = *ptr == '#' ? 1 : fmt_spcf.hx;
				*sptr++ = *ptr++;
			}

			char fsbuf[32];
			int32_t idx = 0;
			if (*ptr == '*')
				COPY_VA_INT(&(fmt_spcf.fw));
			else {
				idx = 0;
				while (ISDIGIT(*ptr)) /* Handle explicit numeric value.  */
				{
					fsbuf[idx++] = *ptr;
					*sptr++ = *ptr++;
				}

				fmt_spcf.fw = atoi(fsbuf);
			}

			if (*ptr == '.') {
				*sptr++ = *ptr++; /* Copy and go past the period.  */
				if (*ptr == '*')
					COPY_VA_INT(&(fmt_spcf.pw));
				else {
					idx = 0;
					while (ISDIGIT(*ptr)) /* Handle explicit numeric value.  */
					{
						fsbuf[idx++] = *ptr;
						*sptr++ = *ptr++;
					}

					fmt_spcf.pw = atoi(fsbuf);
				}
			}

			switch (*ptr) {
				case 'd':
				case 'i': {
					const uint32_t v = abs(va_arg(arg, uint32_t));
					i += print_int(&value[i], v, &fmt_spcf);
				}
				case 'u':
				case 'x':
				case 'X':
				case 'c': {
					/* Short values are promoted to int, so just copy it
                   as an int and trust the C library printf to cast it
                   to the right width.  */
					if (short_width)
						PRINT_TYPE(int);
					else {
						switch (wide_width) {
							case 0:
								PRINT_TYPE(int);
								break;
							case 1:
								PRINT_TYPE(long);
								break;
							case 2:
							default:
#if defined(__GNUC__) || defined(HAVE_LONG_LONG)
								PRINT_TYPE(long long);
#else
								PRINT_TYPE(long); /* Fake it and hope for the best.  */
#endif
								break;
						} /* End of switch (wide_width) */
					}     /* End of else statement */
				}         /* End of integer case */
				break;
				case 'f':
				case 'e':
				case 'E':
				case 'g':
				case 'G': {
					if (wide_width == 0)
						PRINT_TYPE(double);
					else {
#if defined(__GNUC__) || defined(HAVE_LONG_DOUBLE)
						PRINT_TYPE(long double);
#else
						PRINT_TYPE(double); /* Fake it and hope for the best.  */
#endif
					}
				} break;
				case 's':
					PRINT_TYPE(char *);
					break;
				case 'p':
					PRINT_TYPE(void *);
					break;
				case '%':
					PRINT_CHAR('%');
					break;
				default:
					return -2;
			} /* End of switch (*ptr) */
		}     /* End of else statement */
	}

	return total_printed;
}
