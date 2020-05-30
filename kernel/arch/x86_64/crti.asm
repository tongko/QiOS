;-- QiOS------------------------------------------------------------------------
;
;   Author: Liew Tze Wei
;
; 	This file is part of the QiOS kernel, and is made available under the      
;   terms of The Unlicense (That means just do whatever you want with the code  
;   base).                                                                      
;-------------------------------------------------------------------------------
BITS 64

SECTION .init
GLOBAL	_init
_init:
		push		rbp
		mov			rbp, rsp
		;	gcc will nicely put the contents of crtbegin.o's .init section here

SECTION	.fini
GLOBAL	_fini
_fini:
		push		rbp
		mov			rbp, rsp
		;	gcc will nicely put the contents of crtbegin.o's .init section here
