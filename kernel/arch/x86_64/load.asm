;== QiOS ======================================================================;
;                                                                              ;
;    Author - 2020 Liew Tze Wei                                                ;
;                                                                              ;
;    This file is part of the QiOS kernel, and is made available under the     ;
;    terms of The Unlicense (That means just do whatever you want with the     ;
;    code base).                                                               ;
;                                                                              ;
;==============================================================================;
;                                                                              ;
;    load.asm                                                                  ;
;    - This is QiOS kernel loader. GRUB will load this to designated location  ;
;      which is 0x00100000 (1 MiB) base on the linker script link.ld.          ;
;      Loading kernel to 1 MiB is considered safe since EBDA will never exceed ;
;      this barrier.                                                           ;
;                                                                              ;
;==============================================================================;
[BITS 32]

%include "multiboot2.inc"

SECTION	.multiboot
		align 8
multiboot_header:
		;header_len	equ	multiboot_header_end - multiboot_header
		;checksum	equ	-(MULTIBOOT2_HEADER_MAGIC + GRUB_MULTIBOOT_ARCHITECTURE_I386 + header_len)
		;  magic 
		dd		MULTIBOOT2_HEADER_MAGIC
		;  ISA: i386 
		dd		GRUB_MULTIBOOT_ARCHITECTURE_I386
		;  Header length. 
		dd		multiboot_header_end - multiboot_header	;header_len
		;  checksum 
		dd		-(MULTIBOOT2_HEADER_MAGIC + GRUB_MULTIBOOT_ARCHITECTURE_I386 + (multiboot_header_end - multiboot_header));header_len)	;checksum

consoleFlags_tag_start:
		dw		MULTIBOOT_HEADER_TAG_CONSOLE_FLAGS
		dw		MULTIBOOT_CONSOLE_FLAGS_CONSOLE_REQUIRED
		dd		consoleFlags_tag_end - consoleFlags_tag_start
		dd		MULTIBOOT_CONSOLE_FLAGS_EGA_TEXT_SUPPORTED
consoleFlags_tag_end:

		align 8
		dw		MULTIBOOT_HEADER_TAG_END
		dw		0
		dd		8
multiboot_header_end:


SECTION .boot
GLOBAL start, _start

start:
		align 8
mbr_magic:
		dd		1
		align 4
mbr_addr:
		dd		1
		align 8
_start:
		;	Clear direction flag to make sure we're moving forward
		cld
		;	Disable all interrupts so we won't get into trouble
		cli

		;	Reset EFLGAS
		push	0
		popfd

		; Keep multiboot magic and address
		mov		[mbr_magic], eax
		mov		[mbr_addr], ebx

.detect_long_mode:
		;-----------------------------------------------------------------------
		; Check whether long mode can be used or not. Long mode can only be 
		; detected using the extended functions of CPUID (> 0x80000000), so we 
		; have to check if the function that determines whether long mode is 
		; available or not is actually available:
		;-----------------------------------------------------------------------
		; Set the A-register to 0x80000000.
		mov		eax, 0x80000000
		; CPU identification.
		cpuid
		; Compare the A-register with 0x80000001.
		cmp		eax, 0x80000001
		; It is less, there is no long mode.
		jb		no_long_mode

		;-----------------------------------------------------------------------
		; Now that we know that extended function is available we can use it to
		; detect long mode:
		;-----------------------------------------------------------------------
		; Set the A-register to 0x80000001.
		mov		eax, 0x80000001
		; CPU identification.
		cpuid
		; Test if the LM-bit, which is bit 29, is set in the D-register.
		test	edx, 1 << 29
		; They aren't, there is no long mode.
		jz		no_long_mode

		;-----------------------------------------------------------------------
		; Setup a simple PML4T paging at 0x1000
		; then identity map the first 2 MiB
		;-----------------------------------------------------------------------
		;	First thing first, disable paging, if anyone enable it
		; Set the A-register to control register 0.
		mov		eax, cr0
		; Clear the PG-bit, which is bit 31.
		and		eax, 01111111111111111111111111111111b
		; Set control register 0 to the A-register.
		mov cr0, eax

		;	Call 32-bit C program to setup PML4 paging
		push	DWORD [mbr_addr]
		extern	kernel_physical_end
		push	DWORD kernel_physical_end
		extern	kernel_physical_start
		push	DWORD kernel_physical_start
		extern	init_paging_32
		call	init_paging_32
		add		esp, 12

		;-----------------------------------------------------------------------
		; In future, PML5 will be the norm, but will deal with that in CPP
		; code, for now, this is sufficient
		;-----------------------------------------------------------------------
		; Enable PAE-paging by setting the PAE-bit in the fourth control register
		; Entering long mode without setting this will cause #GP
		; Set the A-register to control register 4.
		mov		eax, cr4
		; Set the PAE-bit, which is the 6th bit (bit 5).
		or		eax, 1 << 5
		; Set control register 4 to the A-register.
		mov		cr4, eax

		;-----------------------------------------------------------------------
		; We're now ready to enable long mode, let's do it
		; We should set the long mode bit in the EFER MSR and then we should
		; enable paging and then we are in compatibility mode (which is part of
		; long mode).
		;-----------------------------------------------------------------------
		; First set the LM-bit:
		; Set the C-register to 0xC0000080, which is the EFER MSR.
		mov		ecx, 0xC0000080
		; Read from the model-specific register.
		rdmsr
		; Set the LM-bit which is the 9th bit (bit 8).
		or		eax, 1 << 8
		; Write to the model-specific register.
		wrmsr
		; Enabling paging:
		; Set the A-register to control register 0.
		mov		eax, cr0
		; Set the PG-bit, which is the 32nd bit (bit 31).
		or		eax, 1 << 31
		; Set control register 0 to the A-register.
		mov		cr0, eax

		;	Tada! Now we're in compatibility mode.

		;-----------------------------------------------------------------------
		; Load GDT with 64 bits flags set in the code and data selector
		;-----------------------------------------------------------------------
		; Load the 64-bit global descriptor table.
		lgdt [GDT64.Pointer]
		; Set the code segment and enter 64-bit long mode.
		jmp GDT64.Code:higher_half_entry
.end:

no_cpuid:
		cli
		push	no_cpuid_msg
		call	put_string
		add		esp, 4
		jmp		hang

no_long_mode:
		push	no_long_msg
		call	put_string
		add		esp, 4
		jmp		hang
hang:
		cli
		push	halt_message
		call	put_string
		hlt
		jmp		hang

put_string:
		push	ebp
		mov		ebp, esp

		mov		esi, [ebp + 4]		;	string
		mov		edi, 0xB8000		;	video mem
.L1:
		movsb						;	move byte from esi to edi
		mov		BYTE [edi], 0x07	;	Gray on Black
		inc		edi
		cmp		BYTE [esi], 0		;	compare with zero
		jne		.L1					;	next byte if not zero		
.L2:
		; flush keyboard buffer
		in		al, 0x64
		test	al, 0x01
		jz		.L3
		in		al, 0x60
		jmp		.L2
.L3:
		; wait for key pressed
		in		al, 0x64
		test	al, 0x01
		jz		.L3

		; reboot by triggering triple fault
		int		0xFF
.end

SECTION .data
		align 8
halt_message:
		db		"\nHalt.", 0
no_long_msg:
		db		"\nERROR: Invalid processor architecture. CPU doesn't support long mode.", 0

; Global Descriptor Table (64-bit).
GDT64:
; The null descriptor.
.Null:	equ		$ - GDT64
		dw		0xFFFF					; Limit (low).
		dw		0						; Base (low).
		db		0						; Base (middle)
		db		0						; Access.
		db		1						; Granularity.
		db		0						; Base (high).
; The code descriptor.
.Code:	equ		$ - GDT64
		dw		0						; Limit (low).
		dw		0						; Base (low).
		db		0						; Base (middle)
		db		10011010b				; Access (exec/read).
		db		10101111b				; Granularity, 64 bits flag, limit19:16.
		db		0						; Base (high).
; The data descriptor.
.Data:	equ		$ - GDT64
		dw		0						; Limit (low).
		dw		0						; Base (low).
		db		0						; Base (middle)
		db		10010010b				; Access (read/write).
		db		00000000b				; Granularity.
		db		0						; Base (high).
; The GDT-pointer.
.Pointer: dw		$ - GDT64 - 1			; Limit.
		dq		GDT64					; Base.
_edata:

[CPU X64]
[BITS 64]
SECTION .text
		align 8
higher_half_entry:	; <== this will be map to 0xC0000000 (VAS)
		; Now that we are in 64 bits realm, let's do some real business

		; Reset paging with 64-bits C++ program and map kernel to higher half (64 bits)
		push	QWORD [mbr_addr]
		push	QWORD kernel_physical_end
		push	QWORD kernel_physical_start
		extern	pmman_init_paging
		add		esp, 24


.higher_half_end:

SECTION .bss
GLOBAL kstack_bottom
		align 4		
kstack_bottom:
		KSTACK_SIZE		equ	0x100000		; 1 MiB Stack
		resb			KSTACK_SIZE
kstack_top:
_end: