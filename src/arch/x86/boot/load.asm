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

%include "multiboot2.inc"
; %ifdef	__ELF__
; %define	AOUT_KLUDGE	0
; %else
; %define AOUT_KLUDGE MULTIBOOT_AOUT_KLUDGE
; %endif

SECTION .multiboot
		align	8
multiboot_header:
		header_len	equ	multiboot_header_end - multiboot_header
		checksum	equ	-(MULTIBOOT2_HEADER_MAGIC + GRUB_MULTIBOOT_ARCHITECTURE_I386 + header_len)
		;  magic 
		dd		MULTIBOOT2_HEADER_MAGIC
		;  ISA: i386 
		dd		GRUB_MULTIBOOT_ARCHITECTURE_I386
		;  Header length. 
		dd		header_len
		;  checksum 
		dd		checksum

; %ifndef __ELF__
; 		align	8
; address_tag_start:
;         dw		MULTIBOOT_HEADER_TAG_ADDRESS
;         dw		MULTIBOOT_HEADER_TAG_OPTIONAL
;         dd		address_tag_end - address_tag_start
; 		;  header_addr 
;         dd		multiboot_header
;     	;  load_addr 
;         dd		_start
;         ;  load_end_addr 
;         dd		_edata
;         ;  bss_end_addr 
;         dd		_end
; address_tag_end:

; 		align	8
; entry_address_tag_start:
;         dw		MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS
;         dw		MULTIBOOT_HEADER_TAG_OPTIONAL
;         dd		entry_address_tag_end - entry_address_tag_start
;         ;  entry_addr 
;         dd		_start
; entry_address_tag_end:
; %endif	;  __ELF__

consoleFlags_tag_start:
		dw		MULTIBOOT_HEADER_TAG_CONSOLE_FLAGS
		dw		MULTIBOOT_CONSOLE_FLAGS_CONSOLE_REQUIRED
		dd		consoleFlags_tag_end - consoleFlags_tag_start
		dd		MULTIBOOT_CONSOLE_FLAGS_EGA_TEXT_SUPPORTED
consoleFlags_tag_end:

		align	8
framebuffer_tag_start:
		dw		MULTIBOOT_HEADER_TAG_FRAMEBUFFER
		dw		MULTIBOOT_HEADER_TAG_OPTIONAL
		dd		framebuffer_tag_end - framebuffer_tag_start
		dd		1024
		dd		768
		dd		32
framebuffer_tag_end:

		align	8
moduleAlign_tag_start:
		dw		MULTIBOOT_HEADER_TAG_MODULE_ALIGN
		dw		MULTIBOOT_MODULE_ALIGN_REQUIRED
		dd		moduleAlign_tag_end - moduleAlign_tag_start
moduleAlign_tag_end:

		align	8
		dw		MULTIBOOT_HEADER_TAG_END
		dw		0
		dd		8
multiboot_header_end:


SECTION .boot

GLOBAL start, _start

start:
		align	8
_start:
		cli

		; init gdt
		extern	early_init_gdt
		call	early_init_gdt

		; Deal with paging in C code.
		extern	kernel_virtual_end
		push	kernel_virtual_end
		extern	kernel_virtual_start
		push	kernel_virtual_start
		extern	kernel_physical_end
		push	kernel_physical_end
		extern	kernel_physical_start
		push	kernel_physical_start
		extern	early_init_paging
		call	early_init_paging
		add		esp, 16

		; map higher hafl to page table
		lea		ecx, [higher_half_entry]
		jmp		ecx
.end:

SECTION .text
		align	8
higher_half_entry:
		cli

		mov		esp, kstack_top					; Intialize kernel stack

		push	0								; Reset EFLAGS
		popfd

		extern	_kmain							; Enter C code kernel
		call	_kmain

		; kernel should never return, but just in case...
.hang:
		cli
		hlt
		jmp		.hang

SECTION .data
		align	8
halt_message:
		db		"\nHalt.", 0
_edata:

SECTION .bss
		align	4
kstack_bottom:
		KSTACK_SIZE	equ	0x100000				; 1 MiB Stack
		resb	KSTACK_SIZE
kstack_top:
_end: