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
		; Store the original EFLAGS value to stack
		pushfd
		pop		eax
		mov		ecx, eax
		; Set the invert value of the ID flag (bit 21)
		xor		eax, 0x00200000
		push	eax
		; Load stored EFLAGS (with ID bit inverted)
		popfd
		; Store EFLAGS again (ID bit may or may not be inverted)
		pushfd
		; eax = modified EFLAGS (ID bit may or may not be inverted)
		pop		eax
		; eax = whichever bits were changed, compare to the original EFLAGS VALUE
		xor		eax, ecx
		shr		eax, 21
		and		eax, 1
		push	ecx
		; Restore original EFLAGS
		popfd
		ret

;
;	void _enableSSE()
;
;	Prepares the necessary CPU flags to enable SSE instructions
;
_enableSSE:
		mov		eax, cr0
		and		ax, 0xFFFB
		or		ax, 0x2
		mov		cr0, eax
		mov		eax, cr4
		or		ax, 3 << 9
		mov		cr4, eax
		ret
