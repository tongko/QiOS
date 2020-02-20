/*== QiOS =====================================================================+
|                                                                              |
|    Copyright (C) 2017 - 2020 Liew Tze Wei                                    |
|                                                                              |
|    This file is part of the QiOS kernel, and is made available under the     |
|    terms of The Unlicense (That means just do whatever you want with the     |
|    code base).                                                               |
|                                                                              |
+=============================================================================*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//   version.h                                                                //
//   -  Defines version and welcome information.                              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#if !(__x86_64__)
#error "[QiOS (64 bits) - version.h] requires x86_64 architecture. Please use the right compiler options."
#endif

////////////////////////////////////////////////////////////////////////////////
//    INTERFACE REQUIRED HEADERS                                              //
////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
//    STRUCTURE PROTOTYPE                                                     //
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//    INTERFACE DEFINITIONS/ENUMERATIONS/SIMPLE TYPEDEFS                      //
////////////////////////////////////////////////////////////////////////////////
#define QIOS_VER "0.0.3"
#define QIOS_NAME "QiOS"
#define SPACE " "
#define WELCOME_STR QIOS_NAME SPACE QIOS_VER
#define COPYRIGHT "Copyright (C) 2019, 2020 Liew Tze Wei"

#define TXT_LOGO                                                                                        \
	"                                                                               \n"                 \
	"                                                                               \n"                 \
	"                                                                               \n"                 \
	"        MM'\"\"\"\"\"`MMM oo MMP\"\"\"\"\"YMM MP\"\"\"\"\"\"`MM    d8888b.     a8888a          \n" \
	"        M  .mmm,  MM    M' .mmm. `M M  mmmmm..M        `88    d8' ..8b         \n"                 \
	"        M  MMMMM  MM dP M  MMMMM  M M.      `YM     aaad8'    88 .P 88         \n"                 \
	"        M  MM  M  MM 88 M  MMMMM  M MMMMMMM.  M        `88    88 d' 88         \n"                 \
	"        M  `MM    MM 88 M. `MMM' .M M. .MMM'  M        .88 dP Y8'' .8P         \n"                 \
	"        MM.    .. `M dP MMb     dMM Mb.     .dM    d88888P 88  Y8888P          \n"                 \
	"        MMMMMMMMMMMM    MMMMMMMMMMM MMMMMMMMMMM                                \n"                 \
	"                                                                               \n"                 \
	"        A Toy Operating System Created for Liew Qi By Dad                      \n"                 \
	"                                                                               \n"                 \
	"                                                                               \n"                 \
	"                                                                               "

////////////////////////////////////////////////////////////////////////////////
//    INTERFACE STRUCTURES                                                    //
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//    INTERFACE FUNCTION PROTOTYPES                                           //
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//    End version.h                                                      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
