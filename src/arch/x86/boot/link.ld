/* OUTPUT_FORMAT(elf32-i386) */
/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(_start)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	. = 0x00100000;
    kernel_physical_start = 0x00100000;

	/* First put the multiboot header, as it is required to be put very early
	   early in the image or the bootloader won't recognize the file format.
	   Next we'll put the .text section. */
	.boot : ALIGN (0x1000) /* align at 4 KiB, the size of a page */
	{
		*(.multiboot)
		*(.boot)
	}

	.bootdata : ALIGN (0x1000)
	{
		*(.bootdata)
	}

	/* WARNING: All this need to be done after kernel loaded at physical address 1M */
	/* the code should be linked as though it were loaded at 3GB + 1MB. In fact,
	   it will be loaded at physical address 1MB, then we will map virtual addresses
	   above 3GB to physical address starting at 0 */

	. = . + 0xC0000000;          /* the code should be relocated to 3GB + 1MB + .boot */

	/* these labels get exported to the code files */
	kernel_virtual_start = .;

	.text ALIGN (0x1000) :	AT(ADDR(.text)-0xC0000000) /* align at 4 KB, the size of a page */
	{
		*(.text)
	}

	/* Read-only data. */
	.rodata ALIGN (0x1000) :	AT(ADDR(.rodata)-0xC0000000) /* align at 4 KB, the size of a page */
	{
		*(.rodata)
	}

	/* Read-write data (initialized) */
	.data ALIGN (0x1000) :	AT(ADDR(.data)-0xC0000000) /* align at 4 KB, the size of a page */
	{
		*(.data)
	}

	/* Read-write data (uninitialized) and stack */
	.bss ALIGN (0x1000) :	AT(ADDR(.bss)-0xC0000000) /* align at 4 KB, the size of a page */
	{
		*(COMMON)
		*(.bss)
	}

	/* The compiler may produce other sections, by default it will put them in
	   a segment with the same name. Simply add stuff here as needed. */

    kernel_virtual_end = .;
    kernel_physical_end = . - 0xC0000000;
}
