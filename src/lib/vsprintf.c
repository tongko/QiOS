#include <stdio.h>
#include <stdlib.h>

#define COPY_VA_INT(x)                               \
	do                                               \
	{                                                \
		const int32_t v = abs(va_arg(arg, int32_t)); \
		*(int32_t *)x = v;                           \
		unsigned char buf[32];                       \
		ptr++;		  /* Go past the asterisk.  */   \
		*sptr = '\0'; /* NULL terminate sptr.  */    \
		strcat(sptr, itoa(v, buf, 10));              \
		while (*sptr)                                \
			sptr++;                                  \
	} while (0)

#define PRINT_CHAR(C)    \
	do                   \
	{                    \
		value[i++] = C;  \
		value[i] = '\0'; \
		ptr++;           \
	} while (0)

typedef struct format_specifier
{
	char spcf;
	char f;
	int32_t fw;
	int32_t pw;
	char l;
} format_specifier_t;

static int32_t print_int(unsigned char *dest, int32_t value, format_specifier_t format)
{
	unsigned char buf[32];
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
				Scientific notation (mantissa/exponent) using e character
	E			Scientific notation (mantissa/exponent) using E character
	f			Decimal floating point
	g			Not supported in kernel mode. Uses the shorter of %e or %f.
	G			Uses the shorter of %E or %f
	o			Signed octal
	s			String of characters
	u			Unsigned decimal integer
	x			Unsigned hexadecimal integer
	X			Unsigned hexadecimal integer (capital letters)
	p			Pointer address
	n			Nothing printed
	%			Character
---------------------------------------------------------------------------------------------
	Flags:

	-			Left-justify within the given field width; Right justification is the default
				(see width sub-specifier).
	+			Forces to precede the result with a plus or minus sign (+ or -) even for 
				positive numbers. By default, only negative numbers are preceded with a -ve 
				sign.
	(space)		If no sign is going to be written, a blank space is inserted before the 
				value.
	#			Used with o, x or X specifiers the value is preceded with 0, 0x or 0X 
				respectively for values different than zero. Used with e, E and f, it forces 
				the written output to contain a decimal point even if no digits would follow. 
				By default, if no digits follow, no decimal point is written. Used with g or G 
				the result is the same as with e or E but trailing zeros are not removed.
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

	.number		For integer specifiers (d, i, o, u, x, X) − precision specifies the minimum 
				number of digits to be written. If the value to be written is shorter than 
				this number, the result is padded with leading zeros. The value is not 
				truncated even if the result is longer. A precision of 0 means that no 
				character is written for the value 0. For e, E and f specifiers − this is the 
				number of digits to be printed after the decimal point. For g and G 
				specifiers − This is the maximum number of significant digits to be printed. 
				For s − this is the maximum number of characters to be printed. By default 
				all characters are printed until the ending null character is encountered. For
				c type − it has no effect. When no precision is specified, the default is 1. 
				If the period is specified without an explicit value for precision, 0 is 
				assumed.
	.*			The precision is not specified in the format string, but as an additional 
				integer value argument preceding the argument that has to be formatted.
---------------------------------------------------------------------------------------------
	Length:

	h			The argument is interpreted as a short int or unsigned short int (only applies
				to integer specifiers − i, d, o, u, x and X).
	l			The argument is interpreted as a long int or unsigned long int for integer 
				specifiers (i, d, o, u, x and X), and as a wide character or wide character 
				string for specifiers c and s.
	L			The argument is interpreted as a long double (only applies to floating point 
				specifiers − e, E, f, g and G).
*/
int32_t vsprintf(string_t *str, const string_t *format, va_list arg)
{
	if (!str || !str->value || !format)
	{
		return 0;
	}

	format_specifier_t fmt_spcf;
	const char *ptr = format;
	unsigned char *value = str->value;
	unsigned char specifier[128];
	int32_t total_printed = 0;
	int32_t i = 0;

	while (*ptr != '\0')
	{
		if (*ptr != '%')
		{ /* While we have regular characters, print them.  */
			value[i++] = *ptr++;
			value[i] = '\0';
		}
		else
		{ /* We got a format specifier! */
			unsigned char *sptr = specifier;
			int32_t wide_width = 0, short_width = 0;

			*sptr++ = *ptr++; /* Copy the % and move forward.  */

			while (strchr("-+ #0", *ptr))
			{ /* Move past flags.  */
				fmt_spcf.f = *ptr;
				*sptr++ = *ptr++;
			}

			char fsbuf[32];
			if (*ptr == '*')
				COPY_VA_INT(&(fmt_spcf.fw));
			else
			{
				while (ISDIGIT(*ptr)) /* Handle explicit numeric value.  */
				{
					fsbuf++ = *ptr;
					*sptr++ = *ptr++;
				}
			}

			if (*ptr == '.')
			{
				*sptr++ = *ptr++; /* Copy and go past the period.  */
				if (*ptr == '*')
					COPY_VA_INT(&(fmt_spcf.pw));
				else
				{
					while (ISDIGIT(*ptr)) /* Handle explicit numeric value.  */
					{
						fsbuf++ = *ptr;
						*sptr++ = *ptr++;
					}
				}
			}

			while (strchr("hlL", *ptr))
			{
				switch (*ptr)
				{
				case 'h':
					short_width = 1;
					break;
				case 'l':
					wide_width++;
					break;
				case 'L':
					wide_width = 2;
					break;
				default:
					return 0;
				}
				*sptr++ = *ptr++;
			}

			switch (*ptr)
			{
			case 'd':
			case 'i':
			case 'o':
			case 'u':
			case 'x':
			case 'X':
			case 'c':
			{
				/* Short values are promoted to int, so just copy it
                   as an int and trust the C library printf to cast it
                   to the right width.  */
				if (short_width)
					PRINT_TYPE(int);
				else
				{
					switch (wide_width)
					{
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
				}	 /* End of else statement */
			}		  /* End of integer case */
			break;
			case 'f':
			case 'e':
			case 'E':
			case 'g':
			case 'G':
			{
				if (wide_width == 0)
					PRINT_TYPE(double);
				else
				{
#if defined(__GNUC__) || defined(HAVE_LONG_DOUBLE)
					PRINT_TYPE(long double);
#else
					PRINT_TYPE(double); /* Fake it and hope for the best.  */
#endif
				}
			}
			break;
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
		}	 /* End of else statement */
	}

	return total_printed;
}
