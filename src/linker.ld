/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(_start)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	/* Begin putting sections at 1 MiB, a conventional place for kernels to be
	   loaded at by the bootloader. */
	/* . = 1M; */
	/*. = 0xC0100000*/          /* the code should be relocated to 3GB + 1MB */
	/* these labels get exported to the code files */
    /*kernel_virtual_start = .;
    kernel_physical_start = . - 0xC0000000;*/

	.boot : ALIGN(0x1000)
	{
		*(.multiboot)
	}

	/* First put the multiboot header, as it is required to be put very early
	   early in the image or the bootloader won't recognize the file format.
	   Next we'll put the .text section. */
	.text : /*AT(ADDR(.text)-0xC0000000)*/ ALIGN(0x1000)
	{
		*(.text)
	}

	/* Read-only data. */
	.rodata : /*AT(ADDR(.text)-0xC0000000)*/ ALIGN(4K)
	{
		*(.rodata)
	}

	/* Read-write data (initialized) */
	.data : /*AT(ADDR(.text)-0xC0000000)*/ ALIGN(4K)
	{
		*(.data)
	}

	/* Read-write data (uninitialized) and stack */
	.bss : /*AT(ADDR(.text)-0xC0000000)*/ ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}

	/* The compiler may produce other sections, by default it will put them in
	   a segment with the same name. Simply add stuff here as needed. */
	
	/*kernel_virtual_end = .;
    kernel_physical_end = . - 0xC0000000;*/
}
