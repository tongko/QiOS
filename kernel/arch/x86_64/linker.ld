OUTPUT_FORMAT(elf64-x86-64)
ENTRY(bootstrap)

KERNEL_LMA = 0x00100000;
OFFSET = 0xFFFFFFFF80000000;
KERNEL_VMA = KERNEL_LMA + OFFSET;

SECTIONS
{
	. = KERNEL_LMA;

	.load : 
	{
		*load.o(.multiboot2)
		*load.o(.text)
		*load.o(.rodata)
		*load.o(.data)
		*load.o(.bss)
	}

	. += OFFSET;
	
	.text : AT(ADDR(.text) - OFFSET)
	{
		_code = .;
		*(.text)
		*(.text*)
		*(.init)
		*(.fini)
		*(.ctors)
		*(.dtors)
		*(.rodata)
		*(.rodata*)
		. = ALIGN(0x1000);
	}

	.data : AT(ADDR(.data) - OFFSET)
	{
		_data = .;
		*(.data)
		. = ALIGN(0x1000);
	}

	.eh_frame : AT(ADDR(.eh_frame) - OFFSET)
	{
		_ehframe = .;
		*(.eh_frame)
		. = ALIGN(0x1000);
	}

	.bss : AT(ADDR(.bss) - OFFSET)
	{
		_bss = .;
		*(.bss*)
		*(COMMON)
		. = ALIGN(0x1000);
	}

	_end = . - OFFSET;

	/DISCARD/ :
	{
		*(.comment)
	}
}