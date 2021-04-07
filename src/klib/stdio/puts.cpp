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
	Function	- puts
	Description	- writes a string to stdout up to but not including the null
				  character. A newline character is appended to the output.
	Declaration	- int puts(const char *str)
	Parameters	-
		str		: This is the C string to be written.

	Returns		- If successful, non-negative value is returned. On error, the
				  function returns EOF.
*******************************************************************************/
#include <kernel/KTextMode.hpp>
#include <stdio.hpp>
#include <string.hpp>

namespace qklib {

int puts(const char *str) {

	qkrnl::TextModePutString(str);
	return strlen(str);
}
}	 // namespace qklib