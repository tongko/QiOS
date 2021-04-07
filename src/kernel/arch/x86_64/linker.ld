OUTPUT_FORMAT(elf64-x86-64)
ENTRY(bootstrap)

KERNEL_LMA = 0x00100000;
KERNEL_OFFSET = 0xFFFFFF0000000000;
KERNEL_VMA = KERNEL_LMA + KERNEL_OFFSET;

SECTIONS
{
	. = KERNEL_LMA;

	.loader : 
	{
		*loader.o(.mb2)
		*loader.o(.boot)
		*loader.o(.rodata)
		*loader.o(.data)
		*loader.o(.bss)
	}

	. += KERNEL_OFFSET;
	
	.text ALIGN(0x1000) : AT(ADDR(.text) - KERNEL_OFFSET)
	{
		KERNEL_TEXT = . - KERNEL_OFFSET;
		*loader.o(.text64)
		*(.text)
		*(.gnu.linkonce.t*)
	   	*(.init)
		*(.fini)
	}

	.rodata ALIGN(0x1000) : AT(ADDR(.rodata) - KERNEL_OFFSET)
	{
		KERNEL_RODATA = . - KERNEL_OFFSET;
		start_ctors = .;
			*(SORT(.ctors*))
		end_ctors = .;

		start_dtors = .;
			*(SORT(.dtors*))
		end_dtors = .;

		*(.rodata*)
		*(.gnu.linkonce.r*)
	}

	.data ALIGN(0x1000) : AT(ADDR(.data) - KERNEL_OFFSET)
	{
		KERNEL_DATA = . - KERNEL_OFFSET;
		*(.data)
        *(.gnu.linkonce.d*)
	}

	.bss ALIGN(0x1000) : AT(ADDR(.bss) - KERNEL_OFFSET)
	{
		KERNEL_BSS = . - KERNEL_OFFSET;
 		*(COMMON)
		*(.bss*)
		*(.gnu.linkonce.b*)
	}

	.eh_frame ALIGN(0x1000) : AT(ADDR(.eh_frame) - KERNEL_OFFSET)
	{
		KERNEL_EHFRAME = . - KERNEL_OFFSET;
		*(.eh_frame)
	}
	
	. = ALIGN(0x1000);
	KERNEL_END = . - KERNEL_OFFSET;
	KERNEL_VMA_END = .;
	KERNEL_SIZE = KERNEL_END - KERNEL_LMA;

	/DISCARD/ :
	{
		*(.comment)
	}
}
