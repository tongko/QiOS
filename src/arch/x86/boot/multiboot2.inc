;-- QiOS------------------------------------------------------------------------
;
;    Copyright (C) 2017, 2018 Liew Tze Wei
;
; 	This file is part of the QiOS kernel, and is made available under the      
;   terms of The Unlicense (That means just do whatever you want with the code  
;   base).                                                                      
;-------------------------------------------------------------------------------
;
;   multiboot2.asm
;   Include files for dealing with multiboot2, the content are converted from 
;	the multiboot2.h downloaded from
;	https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html#multiboot2_002eh
;	with just the #define portion.
;
;-------------------------------------------------------------------------------
%ifndef		__MULTIBOOT_ASM_
%define		__MULTIBOOT_ASM_

;  How many bytes from the start of the file we search for the header. 
MULTIBOOT_SEARCH						equ 32768
MULTIBOOT_HEADER_ALIGN					equ 8

;  The magic field should contain this. 
MULTIBOOT2_HEADER_MAGIC					equ 0xe85250d6

;  This should be in %eax. 
MULTIBOOT2_BOOTLOADER_MAGIC				equ 0x36d76289

;  Grub2 Architecture: 0 - 32-bit pmode i386,   4 - 32-bit MIPS
GRUB_MULTIBOOT_ARCHITECTURE_I386		equ	0

;  Alignment of multiboot modules. 
MULTIBOOT_MOD_ALIGN						equ 0x00001000

;  Alignment of the multiboot info structure. 
MULTIBOOT_INFO_ALIGN					equ 0x00000008

;  Flags set in the 'flags' member of the multiboot header. 

MULTIBOOT_TAG_ALIGN							equ 8
MULTIBOOT_TAG_TYPE_END						equ 0
MULTIBOOT_TAG_TYPE_CMDLINE					equ 1
MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME			equ 2
MULTIBOOT_TAG_TYPE_MODULE					equ 3
MULTIBOOT_TAG_TYPE_BASIC_MEMINFO			equ 4
MULTIBOOT_TAG_TYPE_BOOTDEV					equ 5
MULTIBOOT_TAG_TYPE_MMAP						equ 6
MULTIBOOT_TAG_TYPE_VBE						equ 7
MULTIBOOT_TAG_TYPE_FRAMEBUFFER				equ 8
MULTIBOOT_TAG_TYPE_ELF_SECTIONS				equ 9
MULTIBOOT_TAG_TYPE_APM						equ 10
MULTIBOOT_TAG_TYPE_EFI32					equ 11
MULTIBOOT_TAG_TYPE_EFI64					equ 12
MULTIBOOT_TAG_TYPE_SMBIOS					equ 13
MULTIBOOT_TAG_TYPE_ACPI_OLD					equ 14
MULTIBOOT_TAG_TYPE_ACPI_NEW					equ 15
MULTIBOOT_TAG_TYPE_NETWORK					equ 16
MULTIBOOT_TAG_TYPE_EFI_MMAP					equ 17
MULTIBOOT_TAG_TYPE_EFI_BS					equ 18
MULTIBOOT_TAG_TYPE_EFI32_IH					equ 19
MULTIBOOT_TAG_TYPE_EFI64_IH					equ 20
MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR			equ 21

MULTIBOOT_HEADER_TAG_END					equ 0
MULTIBOOT_HEADER_TAG_INFORMATION_REQUEST	equ 1
MULTIBOOT_HEADER_TAG_ADDRESS				equ 2
MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS			equ 3
MULTIBOOT_HEADER_TAG_CONSOLE_FLAGS			equ 4
MULTIBOOT_HEADER_TAG_FRAMEBUFFER			equ 5
MULTIBOOT_HEADER_TAG_MODULE_ALIGN			equ 6
MULTIBOOT_HEADER_TAG_EFI_BS					equ 7
MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_EFI32	equ 8
MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_EFI64	equ 9
MULTIBOOT_HEADER_TAG_RELOCATABLE			equ 10

MULTIBOOT_ARCHITECTURE_I386					equ 0
MULTIBOOT_ARCHITECTURE_MIPS32				equ 4
MULTIBOOT_HEADER_TAG_OPTIONAL				equ 1

MULTIBOOT_LOAD_PREFERENCE_NONE				equ 0
MULTIBOOT_LOAD_PREFERENCE_LOW				equ 1
MULTIBOOT_LOAD_PREFERENCE_HIGH				equ 2

MULTIBOOT_CONSOLE_FLAGS_CONSOLE_REQUIRED	equ 1
MULTIBOOT_CONSOLE_FLAGS_EGA_TEXT_SUPPORTED	equ 2

MULTIBOOT_MODULE_ALIGN_REQUIRED				equ 1

%endif		;__MULTIBOOT_ASM_