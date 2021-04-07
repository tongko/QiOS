[BITS 32]

%define	PML_SELF_REF		0xFFFFF50000000000
%define KERNEL_TEXT_BASE	0xFFFFFFFF80000000
%define KERNEL_PHYS_MAP_END 0x01000000

%define P4_INDEX(vadr)		(((vadr) >> 39) & 0x1FF)
%define P3_INDEX(vadr)		(((vadr) >> 30) & 0x1FF)
%define P2_INDEX(vadr)		(((vadr) >> 21) & 0x1FF)

%define PG_PRESENT			1
%define PG_WRITABLE			(1 << 1)
%define PG_RING0			(1 << 2)
%define PG_PWT				(1 << 3)
%define PG_PCD				(1 << 4)
%define PG_ACCESS			(1 << 5)
%define PG_PAT				(1 << 7)
%define PG_GLOBAL			(1 << 8)
%define PG_NXE				(1 << 31) ; Higher DWORD
%macro map_page 4
		mov		eax, dword %3
		or		eax, dword %4		; Set Flags
		mov		ecx, dword %1
		mov		[ecx + %2 * 8], eax
		and		dword [ecx + 4 + %2 * 8], 0x7FFFFFFF ; Clear NX bit
%endmacro

SECTION		.mb2

header_start:
MULTIBOOT2_HEADER_MAGIC						equ 0xE85250D6
MULTIBOOT2_BOOTLOADER_MAGIC					equ 0x36d76289
GRUB_MULTIBOOT_ARCHITECTURE_I386			equ	0
MULTIBOOT_HEADER_TAG_END					equ 0
MULTIBOOT_HEADER_TAG_MODULE_ALIGN			equ 6
MULTIBOOT_MODULE_ALIGN_REQUIRED				equ 1
PAGE_SIZE									equ 0x1000
KERNEL_OFFSET								equ 0xC0000000
VGABUF										equ 0x000B8000

ALIGN		8
				dd		MULTIBOOT2_HEADER_MAGIC
				dd		GRUB_MULTIBOOT_ARCHITECTURE_I386
				dd		header_end - header_start
				dd		0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

ALIGN		8
module_tag_start:
				dw		MULTIBOOT_HEADER_TAG_MODULE_ALIGN
				dw		MULTIBOOT_MODULE_ALIGN_REQUIRED
				dd		module_tag_end - module_tag_start
module_tag_end:

ALIGN		8
				dw		MULTIBOOT_HEADER_TAG_END
				dw		0
				dd		8
header_end:


SECTION		.boot
bits 32
[GLOBAL		bootstrap]
bootstrap:
		; We want to get into C++ code as soon as we can, so do not
		; add or do anything fency here.
		; We don't want interrupt until the kernel is ready to handle it.
		cli
		; Make sure the code is executing in forward direction
		cld

		; Setup kernel stack
		mov		esp, STACK_BOTTOM

		; Reset EFLAGS
		push	0
		popfd

		; Make sure we're boot by multiboot2 loader
		cmp		eax, MULTIBOOT2_BOOTLOADER_MAGIC
		jne		.no_multiboot

		; Keep the multiboot2 info struct so we could free the ebx register
		mov		dword [mb2addr], ebx
		; Check if CPUID is supported
		call	.probe_cpuid
		; Check if long mode is supported, cause this is 64 bits build
		call	.probe_long_mode
		; Setup temporary Paging
		call	.setup_page_table

		; Make sure we allocated enough memory in the page tables for the kernel
		extern	KERNEL_END
		mov		eax, KERNEL_END
		cmp		eax, 0x200000		; For now we map first 2 MiB
		jg		.kernel_too_big

		; pass page table location to CR3
		mov		eax, PAGE_DIR
		mov		cr3, eax

		; enable PAE
		mov		eax, cr4
		or		eax, 1 << 5
		mov		cr4, eax

		; enable long mode
		; Set ECX to 0xC0000080, which is the EFER MSR.
		mov		ecx, 0xC0000080
		rdmsr
		; Set the EFER.LME (bit 8) and EFER.NXE (bit 11)
		or		eax, (1 << 8)
		wrmsr

		; Enabling paging:
		mov		eax, cr0
		; Set CR0.PG (bit 31) and CR0.WP (bit 16).
		or		eax, (1 << 31)
		mov		cr0, eax

		; mov		ax, word [GDT_ENTRIES.PTR]
		; mov		word [esp], ax
		; mov		eax, dword [GDT_ENTRIES.PTR + 2]
		; mov		dword [esp + 2], eax
		; mov		dword [esp + 6], 0x0

		lgdt	[GDT_ENTRIES.PTR]
		jmp		GDT_ENTRIES.kcode:.no_return

	.no_return:	
		; Set the segment register
		mov		ax, GDT_ENTRIES.kdata
		mov		ds, ax
		mov		es, ax
		mov		fs, ax
		mov		gs, ax
		mov		ss, ax

		;lea		ecx, [long_mode]
		jmp		long_mode

		;	Not suppose to reach here
		cli
		hlt

	; Setup temporarty page table for enabling long mode. This sub routing will
	; setup a page table which identity-map the first 1 GiB
	.setup_page_table:
		mov		eax, P3_TABLE
		or		eax, 0b11 ; present, writable
		mov		[PAGE_DIR], eax
		mov		[PAGE_DIR + 0xFF8], eax		;	Also setup for higher-half

		mov		eax, P2_TABLE
		or		eax, 0b11 ; present, writable
		mov		[P3_TABLE], eax
		mov		[P3_TABLE + 0xFF0], eax			; 	Also setup for higher-half

		mov		ecx, 0	; counter

	.loop:
		mov		eax, 0x200000 ; 2 MiB
		mul		ecx
		or		eax, 0b10000011 ; present, writable, huge page
		mov		[P2_TABLE + ecx * 8], eax

		inc		ecx	; increment counter
		cmp		ecx, 512	; checks if the whole table is mapped
		jne		.loop	; if not, continue

		; Setup recursive map
		mov		eax, PAGE_DIR
		or		eax, 0b11	; present, writable
		mov		[PAGE_DIR + 0xFF0], eax		;	Self-reference recursive map

		ret

	; Verify that CPUID instruction is supported.
	; The ID flag (bit 21) in the EFLAGS register indicates support for the CPUID
	; instruction. If this flag can be modified, it refers this CPU support CPUID.
	.probe_cpuid:
		; Save EFLAGS
		pushfd
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
		xor		eax, [esp]
		; Restore original EFLAGS
		popfd		
		; We need the CPUID instruction... if not we 'hang' ourself :)
		and		eax, 0x00200000
		jz		.no_cpuid

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

	.dochar:
		call	.cprint
	.sprint:
		lodsb	;	string char to ALGDT64.Pointer
	.cprint:
		mov		ah, 0x17		; attrib = light gray on blue
		mov		cx, ax			; save char/attribute
		movzx	eax, word [ypos]
		mov		dx, 160			; 2 bytes (char/attrib)
		mul		dx				; for 80 columns
		mov		edi, VGABUF		; start of video memory
		movzx	ebx, word [xpos]
		shl		bx, 1			; times 2 to skip attrib

		add		di, ax			; add y offset
		add		di, bx			; add x offset

		mov		ax, cx			; restore char/attribute
		stosd					; write char/attribute
		add		byte [xpos], 1	; advance to right
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
		call	.sprint
	.hlt:
		hlt
		jmp		.hlt

	.endbootstrap:



SECTION		.text64
BITS	64
long_mode:
		; Interrupt is still not ready
		cli
		; Long mode doesn't care about most segments.
		; GS and FS base addresses can be set with MSRs.
		mov		ax, GDT_ENTRIES.kdata	; 0x10
		mov		ss, ax
		mov		ds, ax
		mov		es, ax
		mov		fs, ax
		mov		gs, ax

		; Adjust stack pointer
		mov		rax, KERNEL_TEXT_BASE
		add		rsp, rax
	
		; Adjust MB2 info struct
		mov		ebx, dword [mb2addr]
		add		rbx, rax

		sub		rsp, 8					; make some space
		mov		cx, word [GDT_ENTRIES.PTR]
		mov		word [rsp], cx
		mov		rcx, qword [GDT_ENTRIES.PTR + 2]	; 2 byte limit
		add		rcx, rax							; Adjust base addr
		mov		qword [rsp + 2], rcx
		mov		qword [rsp + 10], 0x0

		lgdt	[rsp]
		add		rsp, 8					; remove space
		push	rbx						; store MB2 info struct

		mov		rax, refresh_gdt
		jmp		rax

refresh_gdt:
		pop		rbx					; Restore MB2Addr
		extern	long_mode_entry
		mov		rax, long_mode_entry
		jmp		rax

	.end_long_mode:


	

SECTION		.rodata
E_MB:			db		"ERROR: Not loaded by multiboot2 boot loader, halt.", 0
E_CPUID:		db		"ERROR: Processor doesn't suppot CPUID instruction, halt.", 0
E_LONG:			db		"ERROR: Require 64-bits processor, halt.", 0
E_LARGE:		db		"ERROR: Kernel image is too large, halt.", 0
E_CHECK:		db		"Check Point reach, all OK.", 0
[GLOBAL		GDT_ENTRIES]
GDT_ENTRIES:
	.null:	equ $ - GDT_ENTRIES
				dq		0

	.kcode:	equ $ - GDT_ENTRIES
				dw		0xFFFF			; Limit low
				dw		0x00			; Base low
				db		0x00			; Base middle
				db		0x9A			; Access
				db		0xAF			; Flags and limits
				db		0x00			; Base High
	
	.kdata:	equ $ - GDT_ENTRIES
				dw		0xFFFF			; Limit low
				dw		0x00			; Base low
				db		0x00			; Base middle
				db		0x92			; Access
				db		0xAF			; Flags and limits
				db		0x00			; High
	
	.PTR:
				dw		$ - GDT_ENTRIES - 1	;	Limit
				dq		GDT_ENTRIES			;	Base
				dd		0

SECTION		.data
ALIGN		4
mb2addr:		dd		0
xpos:			dd		0
ypos:			dd		0

SECTION		.bss

[GLOBAL		PAGE_DIR]
ALIGN		0x1000
PAGE_DIR:		resb	PAGE_SIZE
P3_TABLE:		resb	PAGE_SIZE
P2_TABLE:		resb	PAGE_SIZE

; kernel stack space
STACK_TOP:
				resb	PAGE_SIZE * 4	; 16 KiB
STACK_BOTTOM:
				resb	4
