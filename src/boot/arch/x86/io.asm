[bits 32]

global _inb:function (_inb.end - _inb)
global _outb:function (_outb.end - _outb)

_inb:
		enter	0, 0
		push	edx

		xor		eax, eax
		mov		dx, word [ebp+8]					; port
		in		al, dx

		pop		edx
		leave
		ret
.end:

_outb:
		enter	0, 0
		push	edx

		mov		dx, word [ebp+8]					; Value
		mov		al, byte [ebp+12]					; port
		out		dx, al

		pop		edx
		leave
		ret
.end: