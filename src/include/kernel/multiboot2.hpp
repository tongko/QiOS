#pragma once

#include <cdefs.h>
#include <kernel/elf32.hpp>

namespace qkrnl {

//	 How many bytes from the start of the file we search for the header.
#define MB2_SEARCH		 32768
#define MB2_HEADER_ALIGN 8

//	 The magic field should contain this.
#define MB2_HEADER_MAGIC 0xe85250d6

//	 This should be in %eax.
#define MB2_BOOTLOADER_MAGIC 0x36d76289

//	 Alignment of multiboot modules.
#define MB2_MOD_ALIGN 0x00001000

//	 Alignment of the multiboot info structure.
#define MB2_INFO_ALIGN 0x00000008

//	 Flags set in the ’flags’ member of the multiboot header.
#define MB2_TAG_ALIGN				  8
#define MB2_TAG_TYPE_END			  0
#define MB2_TAG_TYPE_CMDLINE		  1
#define MB2_TAG_TYPE_BOOT_LOADER_NAME 2
#define MB2_TAG_TYPE_MODULE			  3
#define MB2_TAG_TYPE_BASIC_MEMINFO	  4
#define MB2_TAG_TYPE_BOOTDEV		  5
#define MB2_TAG_TYPE_MMAP			  6
#define MB2_TAG_TYPE_VBE			  7
#define MB2_TAG_TYPE_FRAMEBUFFER	  8
#define MB2_TAG_TYPE_ELF_SECTIONS	  9
#define MB2_TAG_TYPE_APM			  10
#define MB2_TAG_TYPE_EFI32			  11
#define MB2_TAG_TYPE_EFI64			  12
#define MB2_TAG_TYPE_SMBIOS			  13
#define MB2_TAG_TYPE_ACPI_OLD		  14
#define MB2_TAG_TYPE_ACPI_NEW		  15
#define MB2_TAG_TYPE_NETWORK		  16
#define MB2_TAG_TYPE_EFI_MMAP		  17
#define MB2_TAG_TYPE_EFI_BS			  18
#define MB2_TAG_TYPE_EFI32_IH		  19
#define MB2_TAG_TYPE_EFI64_IH		  20
#define MB2_TAG_TYPE_LOAD_BASE_ADDR	  21

#define MB2_HDR_TAG_END					0
#define MB2_HDR_TAG_INFORMATION_REQUEST 1
#define MB2_HDR_TAG_ADDRESS				2
#define MB2_HDR_TAG_ENTRY_ADDRESS		3
#define MB2_HDR_TAG_CONSOLE_FLAGS		4
#define MB2_HDR_TAG_FRAMEBUFFER			5
#define MB2_HDR_TAG_MODULE_ALIGN		6
#define MB2_HDR_TAG_EFI_BS				7
#define MB2_HDR_TAG_ENTRY_ADDRESS_EFI32 8
#define MB2_HDR_TAG_ENTRY_ADDRESS_EFI64 9
#define MB2_HDR_TAG_RELOCATABLE			10

#define MB2_ARCHITECTURE_I386	0
#define MB2_ARCHITECTURE_MIPS32 4
#define MB2_HDR_TAG_OPTIONAL	1

#define MB2_LOAD_PREFERENCE_NONE 0
#define MB2_LOAD_PREFERENCE_LOW	 1
#define MB2_LOAD_PREFERENCE_HIGH 2

#define MB2_CONSOLE_FLAGS_CONSOLE_REQUIRED	 1
#define MB2_CONSOLE_FLAGS_EGA_TEXT_SUPPORTED 2

typedef struct _PACKED_ {
	dword_t dwType;
	dword_t dwSize;
} Mb2Tag;

struct Mb2TagString : public Mb2Tag {
	char szString[0];
} _PACKED_;

struct Mb2TagMod : public Mb2Tag {
	dword_t dwModStart;
	dword_t dwModEnd;
	char *	lpszCmdLine;
} _PACKED_;

struct Mb2TagMemInfo : public Mb2Tag {
	dword_t dwMemLower;
	dword_t dwMemUpper;
} _PACKED_;

struct Mb2TagBootDev : public Mb2Tag {
	dword_t dwBiosDev;
	dword_t dwSlice;
	dword_t dwPart;
} _PACKED_;

struct Mb2MMapEntry {
	qword_t qwAddress;
	qword_t qwLength;
#define MB2_MEMORY_AVAILABLE		1
#define MB2_MEMORY_RESERVED			2
#define MB2_MEMORY_ACPI_RECLAIMABLE 3
#define MB2_MEMORY_NVS				4
#define MB2_MEMORY_BADRAM			5
	dword_t dwType;
	dword_t Reserved;

	bool IsEmpty() {
		return qwAddress == 0 && qwLength == 0 && dwType == 0 && Reserved == 0;
	}
} _PACKED_;

struct Mb2TagMMap : public Mb2Tag {
	dword_t		 dwEntrySize;
	dword_t		 dwEntryVersion;
	Mb2MMapEntry pEntries[0];
} _PACKED_;

struct Mb2TagVBE : public Mb2Tag {
	word_t wMode;
	word_t wInterfaceSeg;
	word_t wInterfaceOff;
	word_t wInterfaceLen;
	byte_t ControlInfo[512];
	byte_t ModeInfo[256];
} _PACKED_;

struct Mb2TagFrameBufferCommon : public Mb2Tag {
	qword_t qwAddress;
	dword_t dwPitch;
	dword_t dwWidth;
	dword_t dwHeight;
	byte_t	bBpp;
#define MB2_FRAMEBUFFER_TYPE_INDEXED  0
#define MB2_FRAMEBUFFER_TYPE_RGB	  1
#define MB2_FRAMEBUFFER_TYPE_EGA_TEXT 2
	byte_t bType;
	word_t Reserved;
} _PACKED_;

typedef struct _PACKED_ {
	byte_t red;
	byte_t green;
	byte_t blue;
} Mb2Color;

struct Mb2TagFrameBuffer {
	struct Mb2TagFrameBufferCommon common;

	union {
		struct {
			word_t	 wPaletteNumColors;
			Mb2Color Palette[0];
		} _PACKED_;
		struct {
			byte_t bRedFieldPosition;
			byte_t bRedMaskSize;
			byte_t bGreenFieldPosition;
			byte_t bGreenMaskSize;
			byte_t bBlueFieldPosition;
			byte_t bBlueMaskSize;
		} _PACKED_;
	} _PACKED_;
} _PACKED_;

struct Mb2TagElfSect : public Mb2Tag {
	dword_t	   dwNum;
	dword_t	   dwEntSize;
	dword_t	   dwShndx;
	dword_t	   Reserved;
	Elf32Shdr *pSection;
} _PACKED_;

struct Mb2TagAPM : public Mb2Tag {
	word_t	wVersion;
	word_t	wCSeg;
	dword_t dwOffset;
	word_t	wCSeg_16;
	word_t	wDSeg;
	word_t	wFlags;
	word_t	wCSegLen;
	word_t	wCSeg16Len;
	word_t	wDSegLen;
} _PACKED_;

struct Mb2TagEFI32 : public Mb2Tag {
	dword_t dwPointer;
} _PACKED_;

struct Mb2TagEFI64 : public Mb2Tag {
	qword_t dwPointer;
} _PACKED_;

struct Mb2TagSMBios : public Mb2Tag {
	byte_t bMajor;
	byte_t bMinor;
	byte_t bReserved[6];
	byte_t bTables[0];
} _PACKED_;

typedef struct _PACKED_ {
	char	 Signature[8];
	uint8_t	 Checksum;
	char	 OEMID[6];
	uint8_t	 Revision;
	uint32_t RsdtAddress;
} Mb2RSDPDescriptor;

typedef struct _PACKED_ {
	Mb2RSDPDescriptor firstPart;
	uint32_t		  Length;
	uint64_t		  XsdtAddress;
	uint8_t			  ExtendedChecksum;
	uint8_t			  reserved[3];
} Mb2RSDPDescriptor2;

struct Mb2TagOldACPI : public Mb2Tag {
	byte_t bRsdp[0];
} _PACKED_;

struct Mb2TagNewACPI : public Mb2Tag {
	byte_t bRsdp[0];
} _PACKED_;

typedef struct _PACKED_ {
	dword_t dwHeader;
	dword_t dwXId;
	word_t	wSecs;
	word_t	wFlags;
	dword_t dwCiAddr;
	dword_t dwYiAddr;
	dword_t dwSiAddr;
	dword_t dwGiAddr;
	dword_t dwChAddr[4];
	char	szSName[64];
	char	szFile[128];
	//	options not implemented, yet
} Mb2DhcpAck;

struct Mb2TagNetwork : public Mb2Tag {
	byte_t bDhcpAck[0];
} _PACKED_;

struct Mb2TagEFIMMap : public Mb2Tag {
	dword_t dwDescrSize;
	dword_t dwDescrVers;
	byte_t	bEfiMMap[0];
} _PACKED_;

struct Mb2TagEFI32IH : public Mb2Tag {
	dword_t dwPointer;
} _PACKED_;

struct Mb2TagEFI64IH : public Mb2Tag {
	qword_t qwPointer;
} _PACKED_;

struct Mb2TagLoadBaseAddr : public Mb2Tag {
	dword_t dwLoadBaseAddr;
} _PACKED_;

}	 // namespace qkrnl