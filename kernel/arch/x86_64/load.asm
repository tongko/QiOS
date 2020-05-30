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

%include "multiboot2.inc"
%include "include.inc"

;==============================================================================
;	MULTIBOOT HEADER REGION
;==============================================================================
; This section is for Multiboot2 header.
; The Multiboot2 header must be contained completely within the first 32768
; bytes of the OS image, and must be 64-bit aligned.

BITS	32

SECTION		.multiboot2	PROGBITS ALLOC NOEXEC NOWRITE ALIGN=8
ALIGN	8
header_start:
				dd		MULTIBOOT2_HEADER_MAGIC
				dd		GRUB_MULTIBOOT_ARCHITECTURE_I386
				dd		header_end - header_start
				dd		0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

				dw		MULTIBOOT_HEADER_TAG_CONSOLE_FLAGS
				dw		0
				dd		12
				dd		MULTIBOOT_CONSOLE_FLAGS_CONSOLE_REQUIRED

				dw		MULTIBOOT_HEADER_TAG_END
				dw		0
				dd		8
header_end:


SECTION		.bootrodata	PROGBITS ALLOC NOEXEC NOWRITE ALIGN=4
ALIGN	4
E_MB:			db		"ERROR: Not loaded by multiboot2 boot loader, halt.", 0
E_CPUID:		db		"ERROR: Processor doesn't suppot CPUID instruction, halt.", 0
E_LONG:			db		"ERROR: Require 64-bits processor, halt.", 0
E_LARGE:		db		"ERROR: Kernel image is too large, halt.", 0


SECTION		.bootbss	NOBITS ALLOC NOEXEC WRITE ALIGN=0x1000
ALIGN 0x1000
GLOBAL	P4_TABLE
P4_TABLE:		resb	PAGE_SIZE
P3_TABLE:		resb	PAGE_SIZE
P2_TABLE:		resb	PAGE_SIZE

; kernel stack
STACK_TOP:
				resb	PAGE_SIZE * 4 	; 16 KiB stack
STACK_BOTTOM:

; Stacks for the interrupt stack table
				resb	PAGE_SIZE
IST_STACK_1:
				resb	PAGE_SIZE
IST_STACK_2:



SECTION		.bootdata	PROGBITS ALLOC NOEXEC WRITE ALIGN=4
ALIGN	4
xpos:			dd		0
ypos:			dd		0

ALIGN	16
GLOBAL	TSS64
GLOBAL	INT_STACK_TOP
TSS64:
				dd		0
		times 3 dq		0	; RSPn
				dq		0	; Reserved
	.INT_STACK_TOP:
				dq		IST_STACK_1	; IST1, NMI
				dq		IST_STACK_2	; IST2, Double fault
				dq		0 ; IST3
				dq		0 ; IST4
				dq		0 ; IST5
				dq		0 ; IST6
				dq		0 ; IST7
				dq		0 ; Reserved
				dw		0 ; Reserved
				dw		0 ; I/O Map Base Address
TSS_SIZE	equ	$ - TSS64 - 1

GLOBAL	GDT64
GDT64:
		; null descriptor, if we dont declare this, some VM may make
		; noice
	.null	equ $ - GDT64
		dq		0		; null descriptor
		; kernel code descrptor
	.code	equ $ - GDT64
		; Set 64-bit flag.
		; read and execute.
		; DPL is 0.
		dd		0			; Limit (low) + Base (low)
		dd		0x00209A00	; Access (exec/read) + Granularity
		; kernel data descriptor
	.data	equ $ - GDT64
		; Set 64-bit flag.
		; read and write.
		; DPL is 0.
		dd		0			; Limit (low) + Base (low)
		dd		0x00009200	; Access (read/write)
		; user land code descriptor
	.user_code32	equ $ - GDT64
		; DPL is 3. 
		dd		0			; Limit (low) + Base (low)
		dd		0
	.user_data			equ $ - GDT64
		; DPL is 3.
		dd		0			; Limit (low) + Base (low)
		dd		0x0000F200	; Access (read/write)
	.user_code64	equ $ - GDT64
		dd		0			; Limit (low) + Base (low)
		dd		0x0020FA00	; Access (exec/read) Granularity
		; TSS descriptor
	.tss	equ $ - GDT64
		dw		0		; Base (bytes 0-2)
		db		0		; Base (byte 3)
		db		0x89	; Type, present
		db		0		; Misc
		db		0		; Base (byte 4)
		dd		0		; Base (bytes 5-8)
		dd		0		; Zero / reserved

GLOBAL	GDT_SIZE
GDT_SIZE	equ	$ - GDT64 - 1



SECTION		.boottext	PROGBITS ALLOC EXEC NOWRITE ALIGN=16
GLOBAL	_qstart
_qstart:
		;	Disable interrupts
		cli

		;	Clear the direction flag
		cld

		;	Set stack pointer
		mov		eax, STACK_BOTTOM
		mov		esp, eax

		;	Reset EFLAGS by poping the 0 to EFLAGS.
		push	0
		popfd

		;	Initialize cursor
		call	.init_csr

		;	Verify that we have the correct multiboot2 magic number
		test	eax, MULTIBOOT2_HEADER_MAGIC
		jne		.no_multiboot

		;	Check if CPUID is supported
		call	.probe_cpuid

		;	Preserve multiboot information
		push	ebx

		;	Check if long mode is supported
		call	.probe_long_mode

		;	Pop multiboot information
		pop		ebx

		;	Probe for SSE
		mov		eax, 0x1
		cpuid
		test	edx, 1 << 25
		jz		.init_paging
		;SSE is available

		;	Enable SSE (TODO: Do this in cpu/simd.asm)
		mov		eax, cr0
		and		eax, 0xFFFB
		or		ax, 0x2
		mov		cr0, eax
		mov		eax, cr4
		or		ax, 3 << 9
		mov		cr4, eax

	.init_paging:
		; Setup paging
		; First thing first, disable paging, if anyone enable it
		; Set the A-register to control register 0.
		mov		eax, cr0
		; Clear the PG-bit, which is bit 31.
		and		eax, 0x7FFFFFFF
		; Set control register 0 to the A-register.
		mov		cr0, eax

		map_page	P4_TABLE, P4_INDEX(PML_SELF_REF), P4_TABLE, (PG_PRESENT | PG_WRITABLE)		;	Self-reference
		
		; Identity map first 2 MiB
		map_page	P4_TABLE, 0, P3_TABLE, (PG_PRESENT | PG_WRITABLE)
		map_page	P3_TABLE, 0, P2_TABLE, (PG_PRESENT | PG_WRITABLE)

		; P2_TABLE[0] will map to 0x00 (Physical Addr)
		mov		eax, P2_TABLE
		mov		dword [eax], 0
		mov		eax, (PG_PRESENT | PG_WRITABLE | PG_SIZE)	; Present, Writable, PageSize (2MiB)
		mov		[P2_TABLE], eax
		mov		eax, P2_TABLE
		and		dword [eax + 4], (~(1<<31) & 0xFFFFFFFF) ; Clear NX bit

		;	Map addresses from FFFFF58080000000
		map_page	P4_TABLE, P4_INDEX(KERNEL_TEXT_BASE), P3_TABLE, (PG_PRESENT | PG_WRITABLE)
		map_page	P3_TABLE, P3_INDEX(KERNEL_TEXT_BASE), P2_TABLE, (PG_PRESENT | PG_WRITABLE)

		; Make sure we allocated enough memory in the page tables for the kernel
		extern	PHYSICAL_END
		mov		eax, PHYSICAL_END
		cmp		eax, KERNEL_PHYS_MAP_END
		jg		.kernel_too_big

		;	Enable paging
		; Enable PAE-paging by setting the PAE-bit in the fourth control register
		; Entering long mode without setting this will cause #GP
		; Set the A-register to control register 4.
		mov		eax, cr4
		; Set the PAE-bit, which is the 6th bit (bit 5) and Global flag (bit 7)
		or		eax, (1 << 5) | (1 << 7)
		; Set control register 4 to the A-register.
		mov		cr4, eax
		;	Set P4 address in CR3
		mov		eax, P4_TABLE
		mov		cr3, eax

		; Set the C-register to 0xC0000080, which is the EFER MSR.
		mov		ecx, 0xC0000080
		; Read from the model-specific register.
		rdmsr
		; Set the LME-bit which is the 9th bit (bit 8).
		or		eax, 1 << 8
		; Write to the model-specific register.
		wrmsr

		; Enabling paging:
		; Set the A-register to control register 0.
		mov		eax, cr0
		; Set the PG-bit, which is the 32nd bit (bit 31).
		or		eax, (1 << 31) | (1 << 16)
		; Set control register 0 to the A-register.
		mov		cr0, eax

		; sub		esp, 10
		; mov		word [esp], GDT_SIZE
		; mov		dword [esp + 2], GDT64 - KERNEL_TEXT_BASE
		; mov		dword [esp + 6], 0x00000000

		mov		eax, GDT64
		lgdt	[eax]
		jmp		GDT64.code:.no_return

	.no_return:
		; Set the segment registers.
		mov		ax,	GDT64.data
		mov		ds, ax
		mov		es, ax
		mov		fs, ax
		mov		gs, ax
		mov		ss, ax
		
;		jmp		long_mode

		; Not suppose to reach here.
		cli
		hlt

	;	Initialize text mode cursor
	.init_csr:
		;	Set the cursor to blink
		outb	0x03D4, 0x09
		outb	0x03D5, 0x0F
		outb	0x03D4, 0x0B
		outb	0x03D5, 0x0F
		outb	0x03D4, 0x0A
		outb	0x03D5, 0x0E	

		; ;	Set the cursor position to 0, 1
		; outb	0x03D4, 0x0F
		; ;	Set cursor position to (row * 80) + col = (1 * 80) + 0 = 80
		; outb	0x03D5, 0x50
		; outb	0x03D4, 0x0E
		; outb	0x03D5, 0x00

		ret

	;	Output string to console
	.dochar:
		call	.cprint
	.sprint:
		lodsb		;	string char to AL
		cmp		al, 0
		jne		.dochar   ; else, we're done
		add		dword [ypos], 1   ;down one row
		mov		dword [xpos], 0   ;back to left
		ret
	 
	.cprint:
		mov		ah, 0x47		; attrib = light gray on blue
		mov		cx, ax			; save char/attribute
		movzx	eax, word [ypos]
		mov		dx, 160			; 2 bytes (char/attrib)
		mul		dx				; for 80 columns
		movzx	ebx, word [xpos]
		shl		bx, 1			; times 2 to skip attrib

		mov		di, 0			; start of video memory
		add		di, ax			; add y offset
		add		di, bx			; add x offset

		mov		ax, cx			; restore char/attribute
		stosd					; write char/attribute
		add		byte [xpos], 1	; advance to right
		ret

; Verify that CPUID instruction is supported.
; The ID flag (bit 21) in the EFLAGS register indicates support for the CPUID
; instruction. If this flag can be modified, it refers this CPU support CPUID.
	.probe_cpuid:
		; Store the original EFLAGS value to stack
		pushfd

		; Set the invert value of the ID flag (bit 21)
		; NOTE: we do this directly on the stack
		xor		dword [esp], 0x00200000
		; Load stored EFLAGS (with ID bit inverted)
		popfd
		; Store EFLAGS again (ID bit may or may not be inverted)
		pushfd
		; eax = modified EFLAGS (ID bit may or may not be inverted)
		pop		eax
		; eax = whichever bits were changed, compare to the original EFLAGS VALUE
		xor		eax,[esp]
		; We need the CPUID instruction... if not we 'hang' ourself :)
		jz		.no_cpuid
		; Restore original EFLAGS
		popfd

		ret

;-----------------------------------------------------------------------
; Check whether long mode can be used or not. Long mode can only be 
; detected using the extended functions of CPUID (> 0x80000000), so we 
; have to check if the function that determines whether long mode is 
; available or not is actually available:
;-----------------------------------------------------------------------
	.probe_long_mode:
		; Retrieve extended function CPUID info.
		mov		eax, 0x80000000
		cpuid

		; Now, eax has Maximum Input Value for Extended Function CPUID
		; Information. If the maximum value is 0x80000000, it means this
		; CPU don't have more extended functions.
  		; Compare the EAX with 0x80000000.
		xor		eax, 0x80000000
		jz		.no_cpuid

		; Bit 29 is 1 if Intel 64 Architecture is available.
		mov		eax, 0x80000001
		cpuid

		; Test if the LM-bit, which is bit 29, is set in the D-register.
		test	edx, (1 << 29)
		jz		.no_long_mode

		ret

	;	Multiboot magic value was incorrect
	.no_multiboot:
		mov		esi, E_MB
		jmp		.error

	;	Processor doesn't support CPUID instruction
	.no_cpuid:
		mov		esi, E_CPUID
		jmp		.error

	;	Processor doesn't support long mode
	.no_long_mode:
		mov		esi, E_LONG
		jmp		.error

	;	Kernel executable file is greater than the memory we allocated for page tables
	.kernel_too_big:
		mov		esi, E_LARGE
		jmp		.error

	;	.error: Writes "ERROR: $al" in red text and halts the CPU
	;	al: Contains the ASCII character to be printed to the screen
	.error:
		mov		eax, VGABUF
		mov		es, eax
		call	.sprint

		hlt

BITS	64
SECTION		.text
long_mode:
	;	At this point, Paging kicks in, so we need to add KERNEL_TEXT_BASE to our
	;	addresses
	;	Load TSS descriptor into GDT
		mov		rdi, GDT64
		add		rdi, GDT64.tss
		mov		rax, TSS64
		mov		word [rdi + 2], ax
		shr		rax, 16
		mov		byte [rdi + 4], al
		shr		rax, 8
		mov		byte [rdi + 7], al
		shr		rax, 8
		mov		dword [rdi + 8], eax

		;	Load GDT
		mov		rax, KERNEL_TEXT_BASE	; Reset stack pointer
		add		rsp, rax
		mov		qword [rsp + 2], GDT64
		lgdt	[rsp]

		; mov		rax, higher_half
		; jmp		rax

;higher_half:
	;	unmap the lowre half page tables
		mov		rax, P4_TABLE
		mov		rbx, KERNEL_TEXT_BASE
		add		rax, rbx
		mov		qword [rax], 0		; Invalidate first page entry

		;	Flush whole TLB
		mov		rax, cr3
		mov		cr3, rax

		; extern	long_mode_entry
		; mov		rax, long_mode_entry
		; jmp		rax
