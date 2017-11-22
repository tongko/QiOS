// #ifndef __KERNEL_
// #define __KERNEL_
// #endif
// #include "kstdio.h"

// static uint16_t *VIDEO_MEM = (uint16_t *)0xB8000;

// uint32_t _putc(char c, uint32_t offset) {
// 	switch (c) {
// 		// new line
// 		case '\n':
// 			offset = (offset / 80) * 80 + 80;
// 			break;
// 		// carriage return
// 		case '\r':
// 			offset = (offset / 80) * 80;
// 			break;
// 		// tab
// 		case '\t':
// 			offset += 4;
// 			break;
// 		// delete
// 		case 8:
// 			offset--;
// 			VIDEO_MEM[offset] = vga_entry(' ', _print_color);
// 			break;
// 		default:
// 			VIDEO_MEM[offset++] = vga_entry(c, _print_color);
// 			break;
// 	}

// 	//	end of screen?
// 	if (offset > 1999) {
// 		__asm__ __volatile__(  //".intel_syntax noprefix;\n"
// 		    "push edi;\n"
// 		    "push esi;\n"
// 		    "push eax;\n"
// 		    "push ecx;\n"
// 		    "mov edi, 0xB8000;\n"  // dest idx to start of video
// 		    "mov esi, 0xB80A0;\n"  // src idx to 2nd row
// 		    "mov ecx, 0x780;\n"    // prepare to loop for 1920 times.
// 		    "rep movsw;\n"         // loop
// 		    "mov edi, 0xB8F00;\n"  // 25th row to dest idx
// 		    "mov eax, 32;\n"       // space
// 		    "mov ecx, 80;\n"       // prepare to loop 80 times
// 		    "rep stosw;\n"         // loop
// 		    "pop ecx;\n"
// 		    "pop eax;\n"
// 		    "pop esi;\n"
// 		    "pop edi;\n");

// 		offset -= 80;
// 	}

// 	return offset;
// }

// uint32_t _vsprintf(char *str, const char *format, va_list arg) {
// 	char *p = str;
// 	char *s;
// 	uint32_t d;
// 	char c;
// 	uint32_t i = 0;

// 	*p = '\0';
// 	//	loop through the string until NULL terminator
// 	while (*format != '\0') {
// 		if (*format != '%') {
// 			//	Normal character
// 			//i = strlen(p);
// 			p[i++] = *format;
// 			p[i] = '\0';
// 		} else {
// 			switch (*(format + 1)) {
// 				case 'c':  // Char
// 					c = (char)va_arg(arg, int);
// 					//i = strlen(p);
// 					p[i++] = c;
// 					p[i] = '\0';
// 					break;
// 				case 's':  // String
// 					s = va_arg(arg, char *);
// 					strcat(p, s);
// 					i = strlen(p);
// 					break;
// 				case 'u':  // Unsigned integer
// 				case 'd':  // Integer
// 				case 'i':
// 					d = va_arg(arg, int);
// 					char buff1[10] = {0};  // decimal max 10 digits.
// 					strcat(p, itoa(d, buff1, 10));
// 					i = strlen(p);
// 					break;
// 				case 'x':  // Hexadecimal number
// 					// d = va_arg(arg, uint32_t);
// 					// char buff2[8] = {0};
// 					// strcat(p, itoa(d, buff2, 16));
// 					// i = strlen(p);
// 					// break;
// 					d = va_arg(arg, uint32_t);
// 					char buff2[8] = {0};
// 					// Check for leading 0
// 					int32_t j = 2;
// 					char ibuff[10] = {0};
// 					int32_t leading = 0;
// 					if (*(format + 1) == '.') {
// 						//	Read all digits and make it number of leading zeros
// 						while (*(format + j) >= '0' && *(format + j) <= '9') {
// 							ibuff[j - 2] = *(format + j++);
// 						}
// 						//	the . is meant for printing not formating if j == 2
// 						if (j > 2) {
// 							format += j - 1;
// 							leading = atoi(ibuff);
// 						}
// 					}
// 					_itoa(d, buff2, 16);
// 					if (leading > 0) {
// 						char buff3[leading];
// 						size_t bufflen = _strlen(buff2);
// 						int32_t excess = leading - bufflen;
// 						for (int32_t j = 0; j < leading; j++) {
// 							if (j < excess) {
// 								buff3[j] = '0';
// 							} else {
// 								buff3[j] = buff2[j - excess];
// 							}
// 						}
// 						_strcat(p, buff3);
// 					} else {
// 						strcat(p, buff2);
// 					}
// 					i = strlen(p);
// 					break;
// 				default:
// 					// Print the % and the following character
// 					//i = strlen(p);
// 					p[i++] = *format;
// 					p[i++] = *(format + 1);
// 					p[i] = '\0';
// 					break;
// 			}

// 			format++;
// 		}

// 		format++;
// 	}

// 	return strlen(str);
// }

// uint32_t _printf(const char *format, ...) {
// 	char buffer[MAXLEN] = {0};
// 	va_list arg;
// 	uint32_t i = 0;

// 	va_start(arg, format);
// 	_vsprintf(buffer, format, arg);
// 	va_end(arg);

// 	while (buffer[i] != '\0') {
// 		_putc(buffer[i++]);
// 	}
// 	_set_cursor(_cursor_x, _cursor_y);

// 	return i;
// }