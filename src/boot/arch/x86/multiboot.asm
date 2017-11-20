[bits 32]

%include 'src/inc/arch/x86/multiboot.inc'

section .multiboot
		; Align to 32 bits boundry (i386 need to be align 4, 64 need to be align 8 for Multiboot2)
		align	16
multiboot_header:
		;  magic 
		dd		MULTIBOOT2_HEADER_MAGIC
		;  ISA: i386 
		dd		GRUB_MULTIBOOT_ARCHITECTURE_I386
		;  Header length. 
		dd		multiboot_header_end - multiboot_header
		;  checksum 
		dd		-(MULTIBOOT2_HEADER_MAGIC + GRUB_MULTIBOOT_ARCHITECTURE_I386 + (multiboot_header_end - multiboot_header))

; 		align	8
; address_tag_start:
; 		dw		MULTIBOOT_HEADER_TAG_ADDRESS
; 		dw		MULTIBOOT_HEADER_TAG_OPTIONAL
; 		dd		address_tag_end - address_tag_start
; 		;  header_addr 
; 		dd		multiboot_header
; 		;  load_addr 
; 		dd		_start
; 		;  load_end_addr 
; 		dd		_gdt_flush.end
; 		;  bss_end_addr 
; 		dd		_end
; address_tag_end:

		align	8
entry_address_tag_start:
		dw		MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS
		dw		MULTIBOOT_HEADER_TAG_OPTIONAL
		dd		entry_address_tag_end - entry_address_tag_start
		;  entry_addr 
		dd		_start
entry_address_tag_end:

		align	8
consoleFlags_tag_start:
		dw		MULTIBOOT_HEADER_TAG_CONSOLE_FLAGS
		dw		MULTIBOOT_CONSOLE_FLAGS_CONSOLE_REQUIRED
		dd		consoleFlags_tag_end - consoleFlags_tag_start
		dd		MULTIBOOT_CONSOLE_FLAGS_EGA_TEXT_SUPPORTED
consoleFlags_tag_end:

		align	8
moduleAlign_tag_start:
		dw		MULTIBOOT_HEADER_TAG_MODULE_ALIGN
		dw		MULTIBOOT_MODULE_ALIGN_REQUIRED
		dd		moduleAlign_tag_end - moduleAlign_tag_start
moduleAlign_tag_end:

; 		align	8
; framebuffer_tag_start:
; 		dw		MULTIBOOT_HEADER_TAG_FRAMEBUFFER
; 		dw		MULTIBOOT_HEADER_TAG_OPTIONAL
; 		dd		framebuffer_tag_end - framebuffer_tag_start
; 		dd		1024
; 		dd		768
; 		dd		32
; framebuffer_tag_end:

		align	8
		dw		MULTIBOOT_HEADER_TAG_END
		dw		0
		;dw		0
		dd		8
multiboot_header_end:

section .boot
global _start:function (_start.end - _start)
		align	8
_start:
		cli

		;  Initialize the stack pointer. 
		mov		esp, kernel_stack_top
		
		;  Reset EFLAGS. 
		push	0
		popfd

		;  Push the pointer to the Multiboot information structure. 
		push	ebx
		;  Push the magic value. 
		push	eax

		extern	_kernel_main
 		;lea		eax, [_kernel_main - KERNEL_VIRTUAL_BASE]
		call	_kernel_main
		add		esp, 8

		;	Halted
		extern	_printf
		push	.halt_message
		call	_printf
.hang:	cli
 		hlt
 		jmp		.hang

		align	8
.halt_message:
		db		"Halted."
.end:

section .text
global _gdt_flush:function (_gdt_flush.end - _gdt_flush)
		align	8
_gdt_flush:  ; Function to reload GDT 
		enter  0, 0 

		mov		eax, [ebp+8] 
		lgdt	[eax] 
		mov		eax, 0x10    ; 0x10 is the offset in the GDT to our data segment, 3rd entry (0x00, 0x08, 0x10, 0x18, ... + 8 byte) 
		mov		ds, ax 
		mov		es, ax 
		mov		fs, ax 
		mov		gs, ax 
		mov		ss, ax 
		jmp		0x08:.flush2    ; 0x08 is the offset to our code segment: Far jump! 

		align	8
.flush2: 
    leave 
      ret            ; Function end 
.end:	; _gdt_flush End of function

section .bss
align 4
kernel_stack_bottom:
		resb	STACK_SIZE
kernel_stack_top:

_end:

; ;GLOBAL IDT_Contents:data
; ;GLOBAL IDT_Pointer:data
; GLOBAL PAGE_TABLE:data
; GLOBAL PAGE_DIRECTORY:data
; GLOBAL MBI_INFO_STRUCT:data
; GLOBAL _gdt_flush

; ; Declare constants for the multiboot header.
; MBALIGN				equ	1<<0              ; align loaded modules on page boundaries
; MEMINFO				equ	1<<1              ; provide memory map
; FLAGS				equ	MBALIGN | MEMINFO ; this is the Multiboot 'flag' field
; MAGIC				equ	0x1BADB002        ; 'magic number' lets bootloader find the header
; CHECKSUM			equ	-(MAGIC + FLAGS)   ; checksum of above, to prove we are multiboot
; SINT_MAX			equ	0x7FFFFFFF
; NULL				equ	0
; KERNEL_VIRTUAL_BASE	equ 0xC0000000					; Constant declaring base of Higher-half kernel 
; KERNEL_PAGE_TABLE	equ (KERNEL_VIRTUAL_BASE >> 22)	; Constant declaring Page Table index in virtual memory

; ; Declare a multiboot header that marks the program as a kernel. These are magic
; ; values that are documented in the multiboot standard. The bootloader will
; ; search for this signature in the first 8 KiB of the kernel file, aligned at a
; ; 32-bit boundary. The signature is in its own section so the header can be
; ; forced to be within the first 8 KiB of the kernel file.
; section .multiboot	align=4
; 					dd MAGIC
; 					dd FLAGS
; 					dd CHECKSUM
; MBI_VBEMODE_INFO	dd SINT_MAX						; Multiboot Graphics Runtime Vbe Mode Info Addr
; MBI_VBECTRL_INFO	dd SINT_MAX						; Multiboot Graphics Runtime Vbe Control Info Addr
; MBI_VBEMODE			dd SINT_MAX						; Multiboot Graphics Runtime Vbe Mode
; MBI_INFO_MEM_HIGH	dd NULL							; Multiboot Info Memory High
; MBI_INFO_MEM_LOW	dd NULL							; MultibootInfo_Memory_Low

; section .text
; ; The linker script specifies _start as the entry point to the kernel and the
; ; bootloader will jump to this position once the kernel has been loaded. It
; ; doesn't make sense to return from this function as the bootloader is gone.
; ; Declare _start as a function symbol with the given symbol size.
; global _start:function (_start.end - _start)
; _start:
; 		; The bootloader has loaded us into 32-bit protected mode on a x86
; 		; machine. Interrupts are disabled. Paging is disabled. The processor
; 		; state is as defined in the multiboot standard. The kernel has full
; 		; control of the CPU. The kernel can only make use of hardware features
; 		; and any code it provides as part of itself. There's no printf
; 		; function, unless the kernel provides its own <stdio.h> header and a
; 		; printf implementation. There are no security restrictions, no
; 		; safeguards, no debugging mechanisms, only what the kernel provides
; 		; itself. It has absolute and complete power over the machine.
; 		cli

; 		; BEGIN - Multiboot Info
;  		; MultiBoot compliant loader provides info in registers:
; 		; ebx = MultiBoot Info Structure Pointer
; 		; eax = 0x2BADBOO2
; 		mov		dword ecx, 0x2BADB002
; 		cmp		ecx, eax
; 		jne		.noMBI
	
; 		mov		dword [MBI_INFO_STRUCT], ebx		; copy MBI struct pointer to [MBI_INFO_STRUCT]
; 		add		dword ebx, 0x4						; Increment 4 byte -> MBI Low Memory
; 		mov		dword eax, [ebx]					; copy content in ebx to eax pointer to MBI low mem.
; 		mov		dword [MBI_INFO_MEM_LOW], eax
; 		add		dword ebx, 0x4						; Increment 4 byte -> MBI High Memory
; 		mov		dword eax, [ebx]					; copy content in ebx to eax pointer to MBI High Mem.
; 		mov		dword [MBI_INFO_MEM_HIGH], eax
; 		; END - Multiboot Info

; ; ; debug -- Enable cursor
; ; 		mov		dx, 0x3D4
; ; 		mov		al, 0x0A
; ; 		out		dx, al

; ; 		mov		dx, 0x3D5
; ; 		in		al, dx

; ; 		and		al, 0xC0
; ; 		or		al, 0

; ; 		mov		dx, 0x3D5
; ; 		out		dx, al

; ; 		mov		dx, 0x3D4
; ; 		mov		al, 0x0B
; ; 		out		dx, al

; ; 		mov		dx, 0x3E0
; ; 		in		al, dx

; ; 		and		al, 0xE0
; ; 		or		al, 15

; ; 		mov		dx, 0x3D5
; ; 		out		dx, al

; ; ; move cursor to 10x 10y
; ; 	; cursor low port to VGA index register
; ; 	mov al, 0x0F
; ; 	mov dx, 3D4h
; ; 	out dx, al
 
; ; 	; cursor low position to VGA data register
; ; 	mov ax, 42
; ; 	mov dx, 3D5h
; ; 	out dx, al
 
; ; 	; cursor high port to VGA index register
; ; 	mov al, 0x0E
; ; 	mov dx, 3D4h
; ; 	out dx, al
 
; ; 	; cursor high position to VGA data register
; ; 	mov ax, 3
; ; 	mov dx, 3D5h
; ; 	out dx, al
; ; ; end debug

; 		; To set up a stack, we set the esp register to point to the top of our
; 		; stack (as it grows downwards on x86 systems). This is necessarily done
; 		; in assembly as languages such as C cannot function without a stack.
; 		mov		esp, kernel_stack_top
 
; 		; This is a good place to initialize crucial processor state before the
; 		; high-level kernel is entered. It's best to minimize the early
; 		; environment where crucial features are offline. Note that the
; 		; processor is not fully initialized yet: Features such as floating
; 		; point instructions and instruction set extensions are not initialized
; 		; yet. The GDT should be loaded here. Paging should be enabled here.
; 		; C++ features such as global constructors and exceptions will require
; 		; runtime support to work as well.
; 		extern _gdt_init
; 		call	_gdt_init
 
; ; 		; BEGIN - Tell CPU about GDT
; ; 		mov		dword [GDT_Pointer + 2], GDT_Contents
; ; 		mov		dword eax, GDT_Pointer
; ; 		lgdt	[eax]
; ; 		; Set data segments
; ; 		mov		dword eax, 0x10
; ; 		mov		word ds, eax
; ; 		mov		word es, eax
; ; 		mov		word fs, eax
; ; 		mov		word gs, eax
; ; 		mov		word ss, eax
; ; 		; Force reload of code segment
; ; 		jmp		8:.Boot_FlushCsGDT
; ; .Boot_FlushCsGDT:
; 		; END - Tell CPU about GDT
	
; 		; END - Set Screen Colour
; 		; Enter the high-level kernel. The ABI requires the stack is 16-byte
; 		; aligned at the time of the call instruction (which afterwards pushes
; 		; the return pointer of size 4 bytes). The stack was originally 16-byte
; 		; aligned above and we've since pushed a multiple of 16 bytes to the
; 		; stack since (pushed 0 bytes so far) and the alignment is thus
; 		; preserved and the call is well defined.
;         ; note, that if you are building on Windows, C functions may have "_" prefix in assembly: _kernel_main
; 		extern _kernel_main
; 		lea eax, [_kernel_main - KERNEL_VIRTUAL_BASE]
; 		call _kernel_main

; 		jmp .hang

; ; BEGIN - Set Screen Colour 
; .noMBI:	mov		dword eax, 0x4F		; Colour: 0x4- = Red background, 0x-F = White foreground
; 		mov		dword ebx, 0xB8000 	; Display Memory address
; 		mov		dword ecx, 2000		; 80 x 25 Characters - VGA Text-mode Display size
; .L1:	mov		byte [ebx], 0
; 		mov		byte [ebx+1], al
; 		add		ebx, 2
; 		loop	.L1
; ; END - Set Screen Colour
	
; 		; If the system has nothing more to do, put the computer into an
; 		; infinite loop. To do that:
; 		; 1) Disable interrupts with cli (clear interrupt enable in eflags).
; 		;    They are already disabled by the bootloader, so this is not needed.
; 		;    Mind that you might later enable interrupts and return from
; 		;    kernel_main (which is sort of nonsensical to do).
; 		; 2) Wait for the next interrupt to arrive with hlt (halt instruction).
; 		;    Since they are disabled, this will lock up the computer.
; 		; 3) Jump to the hlt instruction if it ever wakes up due to a
; 		;    non-maskable interrupt occurring or due to system management mode.

; .hang:	cli
; 		hlt
; 		jmp .hang
; .end:

; _gdt_flush:	; Function to reload GDT
; 		enter	0, 0
		
; 		mov		eax, [ebp+8]
; 		lgdt	[eax]
; 		mov		eax, 0x10		; 0x10 is the offset in the GDT to our data segment, 3rd entry (0x00, 0x08, 0x10, 0x18, ... + 8 byte)
; 		mov		ds, ax
; 		mov		es, ax
; 		mov		fs, ax
; 		mov		gs, ax
; 		mov		ss, ax
; 		jmp		0x08:.flush2		; 0x08 is the offset to our code segment: Far jump!
; .flush2:
; 		leave
;     	ret						; Function end
		


; ; ; BEGIN - Interrupt Handler Macro	

; ; extern CommonInterruptHandler
; ; %macro CommonInterruptHandlerMacro 1
; ; global CommonInterruptHandler%1:function
; ; CommonInterruptHandler%1:
; ; 		pushad
	
; ; 		push	dword %1
; ; 		call	CommonInterruptHandler
; ; 		add		esp, 4

; ; 		popad

; ; 		iret
; ; %endmacro
; ; %assign HandlerNum 0
; ; %rep 256
; ;     CommonInterruptHandlerMacro HandlerNum
; ;     %assign HandlerNum HandlerNum+1
; ; %endrep
; ; ; END - Interrupt Handler Macro

; section .data
; ; We keep the multiboot info pass by boot loader here.
; MBI_INFO_STRUCT		dd	NULL

; section .bss
; ; The multiboot standard does not define the value of the stack pointer register
; ; (esp) and it is up to the kernel to provide a stack. This allocates room for a
; ; small stack by creating a symbol at the bottom of it, then allocating 16384
; ; bytes for it, and finally creating a symbol at the top. The stack grows
; ; downwards on x86. The stack is in its own section so it can be marked nobits,
; ; which means the kernel file is smaller because it does not contain an
; ; uninitialized stack. The stack on x86 must be 16-byte aligned according to the
; ; System V ABI standard and de-facto extensions. The compiler will assume the
; ; stack is properly aligned and failure to align the stack will result in
; ; undefined behavior.
; align 16
; kernel_stack_bottom:
; 					resb 65535 
; kernel_stack_top:


; alignb 4096
; PAGE_TABLE:			resb 4194304			; Reserve uninitialised space for Page Table -  # of entries/page table * 4 bytes/entry * total # of page tables 
; 											; actual size = 4194304 bytes = 4MiB, represents 4GiB in physical memory
; 											; ie. each 4 byte entry represent 4 KiB in physical memory
; PAGE_DIRECTORY:		resb 4096				; Reserve uninitialised space for Page Directory - # of pages tables * 4 bytes/entry * # of directory (4096 = 4 KiB)

