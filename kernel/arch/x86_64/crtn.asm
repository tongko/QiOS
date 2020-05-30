;-- QiOS------------------------------------------------------------------------
;
;   Author: Liew Tze Wei
;
; 	This file is part of the QiOS kernel, and is made available under the      
;   terms of The Unlicense (That means just do whatever you want with the code  
;   base).                                                                      
;-------------------------------------------------------------------------------
BITS 64

SECTION	.init
		; gcc will nicely put the contents of crtend.o's .init section here. */
		pop		rbp
		ret

SECTION .fini
		; gcc will nicely put the contents of crtend.o's .init section here. */
		pop		rbp
		ret
