[BITS	64]

[EXTERN KERNEL_VMA]
[EXTERN KERNEL_VMA_END]
[EXTERN KERNEL_OFFSET]
[EXTERN PAGE_DIR]
SECTION		.text

[GLOBAL long_mode_entry]
long_mode_entry:
	; Align the stack to 16 bytes
		and		rsp, ~0xF

	; Multiboot structure (linear address)
		push	rbx
		;sub		rsp, 8				; Make some gap

	; ; Initialise VGA textmode driver
	 	[extern	TextModeInit]
	 	call	TextModeInit
		mov		dword [0xb8000], 0x2f4b2f4f	; Debug code

	; ; Set up BSP per-CPU data area
	; 	extern	percpu_init_bsp
	; 	call	percpu_init_bsp

	; ; Initialise scheduler (set dummy task)
	; 	extern	sched_init
	; 	call	sched_init

	;; Load TSS
		; mov		ax, GDT_TSS
		; ltr		ax

	;; Enable SSE, AVX, AVX-512
		; extern	simd_init
		; call	simd_init

	;; Enable syscall/sysret instruction
		; extern	syscall_enable
		; call	syscall_enable

	; Call global constructors
		[EXTERN		_init]
		call	_init

	; Pass multiboot information to kmain
		pop		rax					; Restore MB2 Info Structure
		mov		[bootParams], rax
		mov		rax, KERNEL_VMA		; From linker.ld
		mov		[bootParams + 8], rax
		mov		rax, KERNEL_VMA_END	; From linker.ld
		mov		[bootParams + 16], rax
		mov		rax, PAGE_DIR		; From loader.asm
		mov		[bootParams + 24], rax
		mov		rax, KERNEL_OFFSET	; From linker.ld
		mov		[bootParams + 32], rax
		mov		rdi, bootParams		; Pointer to bootParams
		
		[EXTERN		_kmain]
		call	_kmain

	; Don't call global destructors - we should never get here
		sti
.end:
		hlt
		jmp .end


SECTION		.data
; struct BootParams
bootParams:
		dq		0	; Pointer to MB2 Info Struct
		dq		0	; Pointer to Kernel starts (VMA)
		dq		0	; Pointer to Kernel end (VMA)
		dq		0	; PAGE_DIR place holder
		dq		0	; Kernel virtual offset
