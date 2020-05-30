OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_qstart)

PHYS = 0x00100000;
OFFSET = 0xFFFFFFFF80000000;
VIRT = PHYS + OFFSET;

SECTIONS
{
	. = PHYS;

	.bootrodata ALIGN(0x1000) :
	{
		*load.o(.multiboot2)
		*load.o(.bootrodata)
	}

	.bootbss ALIGN(0x08) :
	{
		*load.o(.bootbss)
	}

	.bootdata ALIGN(0x08) :
	{
		*load.o(.bootdata)
	}

	.boottext ALIGN(0x10) :
	{
		*load.o(.boottext)
	}

	. += OFFSET;

	.rodata ALIGN(0x08) : AT(ADDR(.rodata) - OFFSET)
	{
		*(.rodata)
		*(.rodata*)
		*(.eh_frame)
	}

	.text ALIGN(0x10) : AT(ADDR(.text) - OFFSET)
	{
		*(.text)
		*(.text*)
		*(.init)
		*(.fini)
		*(.ctors)
		*(.dtors)
	}

	.data ALIGN(0x1000) : AT(ADDR(.data) - OFFSET)
	{
		*(.data)
	}

	.bss ALIGN(0x1000) : AT(ADDR(.bss) - OFFSET) /* align at 4 KB, the size of a page */
	{
		*(.bss*)
	}

	/DISCARD/ :
	{
		*(.comment)
	}

	PHYSICAL_END = . + 0x1000 - OFFSET;
}