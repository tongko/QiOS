%define	PAGE_SIZE			0x1000
%define	VGABUF				0xB8000

%define	PML_SELF_REF		0xFFFFF50080000000
%define KERNEL_TEXT_BASE	0xFFFFFFFF80000000
%define KERNEL_PHYS_MAP_END 0x01000000

%define P4_INDEX(vadr)		(((vadr) >> 39) & 0x1FF)
%define P3_INDEX(vadr)		(((vadr) >> 30) & 0x1FF)

; bit 0 is PRESENT
%define PG_PRESENT			(1 << 0)
; bit 1 is WRITABLE
%define PG_WRITABLE			(1 << 1)
; bit 7 is 2 MiB Page
%define PG_SIZE				(1 << 7)

; outb(port, val)
%macro outb 2
		mov		al, %2
		mov		dx, %1
		out		dx, al
%endmacro

%macro zero_init 2
		mov		ecx, %2
		mov		eax, %1
		rep mov	dword [eax], 0
%endmacro

%macro map_page 4
		mov		eax, %3
		or		eax, %4		; Set Flags
		mov		ecx, %1
		mov		[ecx + %2 * 8], eax
		and		dword [ecx + 4 + %2 * 8], 0x7FFFFFFF ; Clear NX bit
%endmacro
