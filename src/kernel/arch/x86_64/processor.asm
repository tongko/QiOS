[BITS 64]

[GLOBAL _probeCPUID]
[GLOBAL _enableSSE]

; in include/kernel/system/processor/Processor.hpp
; bool _probeCPUID()
;
; Check if the CPUID is supported. This is implemented in pure assembly,
; because C/C++ could cause rase condition due to the ability of the compiler
; to modify the EFLAGS when it needs to.
;
_probeCPUID:
		; Store the original RFLAGS value to stack
		pushfq
		pop		rax
		mov		rcx, rax
		; Set the invert value of the ID flag (bit 21)
		xor		rax, 0x00200000
		push	rax
		; Load stored RFLAGS (with ID bit inverted)
		popfq
		; Store RFLAGS again (ID bit may or may not be inverted)
		pushfq
		; eax = modified RFLAGS (ID bit may or may not be inverted)
		pop		rax
		; eax = whichever bits were changed, compare to the original EFLAGS VALUE
		xor		rax, rcx
		shr		rax, 21
		and		rax, 1
		push	rcx
		; Restore original EFLAGS
		popfq
		ret

;
;	void _enableSSE()
;
;	Prepares the necessary CPU flags to enable SSE instructions
;
_enableSSE:
		mov		rax, cr0
		and		ax, 0xFFFB
		or		ax, 0x2
		mov		cr0, rax
		mov		rax, cr4
		or		ax, 3 << 9
		mov		cr4, rax
		ret
