;-- QiOS------------------------------------------------------------------------
;
;    Copyright (C) 2017, 2018 Liew Tze Wei
;
; 	This file is part of the QiOS kernel, and is made available under the      
;   terms of The Unlicense (That means just do whatever you want with the code  
;   base).                                                                      
;-------------------------------------------------------------------------------
;
;   load.asm
;	Entry point of QiOS kernel. GRUB will load this to designated location
;   which is 0x00100000 (1 MiB) base on the linker script link.ld.
;	Loading kernel to 1 MiB is considered safe since EBDA will never exceed this
;	barrier
;
;-------------------------------------------------------------------------------
[bits 32]

SECTION .boot

GLOBAL _gdt_flush
		align	8
_gdt_flush:
		enter	0, 0

		mov		ecx, [ebp+8]
		lgdt	[ecx]
		mov		ecx, [ebp+12]    ; 0x10 is the offset in the GDT to our data segment, 3rd entry (0x00, 0x08, 0x10, 0x18, ... + 8 byte) 
		mov		ds, cx
		mov		es, cx
		mov		fs, cx
		mov		gs, cx
		mov		ss, cx
		jmp		0x08:.flush2    ; 0x08 is the offset to our code segment: Far jump! 

		align	8
.flush2: 
	    leave 
    	ret            ; Function end
.end:

GLOBAL _tss_flush
		align	8
_tss_flush:
		mov		ax, 0x2B		; Load the index of our TSS structure - The index is
								; 0x28, as it is the 5th selector and each is 8 bytes
								; long, but we set the bottom two bits (making 0x2B)
								; so that it has an RPL of 3, not zero.
		ltr		ax				; Load 0x2B into the task state register.
		ret
.end:	; _tss_flush End of function
