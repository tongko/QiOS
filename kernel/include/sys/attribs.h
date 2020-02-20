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
//   attribs.h                                                                //
//   -  Provides basic macro to simplify repeated code use every where        //
//		in the kernel.                                                        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#if !(__x86_64__)
#error "[QiOS (64 bits) - attribs.h] requires x86_64 architecture. Please use the right compiler options."
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
#ifdef _SECT
#undef _SECT
#endif //	sect
#define _SECT(S) __attribute__((section(#S)))

#ifdef _EARLY
#undef _EARLY
#endif //	EARLY
#define _EARLY _SECT(.early)

#ifdef _EARLYDATA
#undef _EARLYDATA
#endif //	EARLYDATA
#define _EARLYDATA _SECT(.earlydata)

#ifdef _ALIGNED
#undef _ALIGNED
#endif //	ALIGNED
#define _ALIGNED(x) __attribute__((aligned(x)))

#ifdef _PACKED
#undef _PACKED
#endif //	PACKED
#define _PACKED __attribute__((packed))

#ifdef _CDECL
#undef _CDECL
#endif //	CDECL
#define _CDECL __attribute__((cdecl))

#ifdef _ASM
#undef _ASM
#endif //	asm
#define _ASM(x...) __asm__(x)

#ifdef _ASMV
#undef _ASMV
#endif //	ASMV
#define _ASMV(x...) __asm__ __volatile__(x)

#ifdef _IRQH
#undef _IRQH
#endif //	IRQH
#define _IRQH __attribute__((interrupt))

////////////////////////////////////////////////////////////////////////////////
//    INTERFACE STRUCTURES                                                    //
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//    INTERFACE FUNCTION PROTOTYPES                                           //
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//    End attribs.h                                                      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
