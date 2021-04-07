/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2020 Liew Tze Wei                                            *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 *                                                                             *
 * ****************************************************************************/
/*******************************************************************************

	Function	- putchar
	Description	- writes a character (an unsigned char) specified by the
				  argument char to stdout.
	Declaration	- int putchar(int char)
	Parameers	-
		char	: This is the character to be written. This is passed as its
				  int promotion.
	Returns		- This function returns the character written as an unsigned
				  char cast to an int or EOF on error.
*******************************************************************************/
//#include <kernel/term.hpp>
#include <kernel/KTextMode.hpp>
#include <stdio.hpp>

namespace qklib {

int putchar(int c) {

	char ch = (char) c;

	qkrnl::TextModePutChar(ch);
	return c;
}
}	 // namespace qklib