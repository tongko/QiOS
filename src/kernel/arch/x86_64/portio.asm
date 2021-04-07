[BITS 64]

SECTION		.text

[GLOBAL inportb]
inportb:
	and		rax, 0
	mov		rdx, rdi

	in		al, dx

	ret

[GLOBAL inportw]
inportw:
	and		rax, 0
	mov		rdx, rdi

	in		ax, dx

	ret

[GLOBAL outportb]
outportb:
	mov		rax, rsi
	mov		rdx, rdi

	out		dx, al

	ret

[GLOBAL outportw]
outportw:
	mov		rax, rsi
	mov		rdx, rdi

	out		dx, ax

	ret
	