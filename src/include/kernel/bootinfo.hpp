#pragma once

#include <kernel/setup.hpp>
#include <kernel/types.hpp>

namespace qkrnl {

//	Prototypes
struct Mb2MMapEntry;
struct Elf64Shdr;

struct BootParams {
	__vma_t Mb2Address;
	__vma_t KernelStart;
	__vma_t KernelEnd;
	__vma_t PageDirectory;
	size_t	VirtualOffset;
};

/**
 * Boot Device
 **/
typedef struct {
	dword_t dwBiosDev;
	dword_t dwSlice;
	dword_t dwPart;
} BiosBootDevice;

/**
 * Boot module info
 **/
typedef struct BootModule {
	__pma_t		BaseAddr;
	__pma_t		EndAddr;
	char *		lpszModString;
	BootModule *pNext;
};

/**
 * ELF Symbols
 **/
typedef struct {
	dword_t	   dwNum;
	dword_t	   dwShndx;
	Elf64Shdr *pSection;
} ElfSymbols;

/**
 * Basic Mem Info
 **/
typedef struct {
	dword_t dwMemLower;
	dword_t dwMemUpper;
} BasicMemInfo;

/**
 * BIOS Boot Device
 **/
typedef struct {
	dword_t dwBiosDev;
	dword_t dwPartition;
	dword_t dwSubPartition;
} BiosBootDev;

/**
 * Memory Map Entry type
 **/
enum class MMapEntryType {
	Available = 1,			// Can be used freely
	Reserved = 2,			//	Unusable
	AcpiReclaimable = 3,	// Usable memory holding ACPI information
	NVS = 4,	  // Reserved memory which needs to be preserved on hibernation
	BadRam = 5	  // Defective ram
};

class MemMapEntry {
public:
	MemMapEntry();
	MemMapEntry(Mb2MMapEntry &entry);

public:	   // Attributes
	__pma_t		  BaseAddress() noexcept { return m_qwBaseAddr; }
	size_t		  Length() noexcept { return m_qwLength; }
	MMapEntryType Type() noexcept { return m_eType; }
	MemMapEntry * GetNext() const noexcept { return m_pNext; }
	void		  SetNext(MemMapEntry *tNext) noexcept { m_pNext = tNext; }

public:	   // Operations
	void FromMb2(const Mb2MMapEntry *entry) noexcept;
	bool IsEmpty() noexcept {
		return m_eType == MMapEntryType::Available && m_qwBaseAddr == 0
			   && m_qwLength == 0;
	}

private:	// Operations
	static MMapEntryType GetEntryType(dword_t dwType) noexcept;

private:	//	Members
	MMapEntryType m_eType;
	__pma_t		  m_qwBaseAddr;
	size_t		  m_qwLength;
	MemMapEntry * m_pNext;
};

/**
 * Boot APM table
 **/
typedef struct _PACKED_ {
	word_t	wVersion;
	word_t	wCSeg;
	dword_t dwOffset;
	word_t	wCSeg16;
	word_t	wDSeg;
	word_t	wFlags;
	word_t	wCSegLen;
	word_t	wCSeg16Len;
	word_t	wDSegLen;
} BootApm;

/**
 * VBE Info
 **/
typedef struct {
	word_t wMode;
	word_t wInterfaceSeg;
	word_t wInterfaceOff;
	word_t wInterfaceLen;
	byte_t ControlInfo[512];
	byte_t ModeInfo[256];
} VBEInfo;

/**
 * MB2 Boot Info Struct
 **/
typedef struct _PACKED_ mb2_boot_info_t {
	char *		  lpszBootCmdLine;
	char *		  lpszBootLoaderName;
	BootModule *  pBootModules;
	BasicMemInfo *pBasicMemInfo;
	BiosBootDev * pBiosBootDevice;
	MemMapEntry * pMemMapEntry;
	BootApm *	  pBootApm;
	VBEInfo *	  pVbeInfo;

} Mb2BootInfo;

// typedef struct _PACKED_ mb2_info_t {
// 	Mb2TagMemInfo	   MemInfo;
// 	Mb2TagBootDev	   BootDevice;
// 	char *			   lpszBootCommandLine;
// 	Mb2TagMod		   Modules;
// 	Mb2TagElfSect	   ElfSymbols;
// 	Mb2TagMMap		   MemoryMap;
// 	char *			   lpszBootLoaderName;
// 	Mb2TagAPM		   APMTable;
// 	Mb2TagVBE		   VBEInfo;
// 	Mb2TagFrameBuffer  FrameBufferInfo;
// 	Mb2TagEFI32		   EFI32SysTable;
// 	Mb2TagEFI64		   EFI64SysTable;
// 	Mb2TagSMBios	   SMBiosTables;
// 	Mb2TagOldACPI	   OldAcpi;
// 	Mb2TagNewACPI	   NewAcpi;
// 	Mb2TagNetwork	   NetInfo;
// 	Mb2TagEFIMMap	   EFIMemMap;
// 	Mb2TagEFI32IH	   EFI32ImgHandlePt;
// 	Mb2TagEFI64IH	   EFI64ImgHandlePt;
// 	Mb2TagLoadBaseAddr ImageLoadPhysAddr;
//
// } Mb2Info;
//
// struct BOOT_INFO {
//
// 	Mb2Info *pMB2Info {nullptr};
// 	__vma_t	 vBmpStart {0};
// 	dword_t	 dwBmpSize {0};
// 	__vma_t	 vKernelStart {0};
// 	dword_t	 dwKernelSize {0};
// 	__vma_t	 vStackStart {0};
// 	dword_t	 dwStackSize {0};
//
// } _PACKED_;
//
// typedef struct {
// 	__pma_t ModStart;
// 	__pma_t ModEnd;
// 	char	szCmdLine[MAX_LEN];
// } BootModule;

/**
 * Initialize boot info.
 */
void BootInfoInit();

//	Parameters
//	tlpMemMap
//	-	A pointer to an array of MemMapEntry that receives the list of memory
// map.
//
//	tCb
//	-	The size of the tlpMemMap array, in bytes.
//
//	tlpCbNeeded
//	-	The number of bytes returned in the tlpMemMap array.
//
//	Return value
//	if the function succeeds, the return value is non zero.
//
//	Remarks
//	It is a good idea to use a large array, because it is hard to predict how
// many memory map entries there will be at the time you call
// BootInfoEnumMemMap.
int BootInfoEnumMemMap(MemMapEntry *tlpMemMap, int_t tCb, int_t *tlpCbNeeded);

int BootInfoGetBasicMemInfo(qword_t *tqwLoMem, qword_t *tqwHimem);

void BootInfoDebugPrint(uintptr_t tdwMb2Addr);

}	 // namespace qkrnl