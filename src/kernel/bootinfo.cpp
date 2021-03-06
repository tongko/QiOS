#include <kernel/Error.hpp>
#include <kernel/Logger.hpp>
#include <kernel/bootinfo.hpp>
#include <kernel/multiboot2.hpp>
#include <kernel/qkernel.hpp>
#include <new>
#include <stdio.hpp>
#include <stdlib.hpp>
#include <string.hpp>

using namespace qklib;
using namespace qkrnl;

//	Global variables
extern Logger *		g_pLogger;
extern BootParams * g_pBootParams;
extern Mb2BootInfo *g_pBootInfo;

namespace qkrnl {

//////////////////////////////////////////////
//	Private fields
static char *		 s_BootCmdLine {0};
static char *		 s_BootLoaderName {0};
static Mb2MMapEntry *s_aMemMap;
static bool			 s_ExitBootServiceNotCalled {false};
static dword_t		 s_dwEfi32ImgHandlePtr {0};
static qword_t		 s_qwEfi64ImgHandlePtr {0};
static dword_t		 s_dwLoadBaseAddr {0};
static dword_t		 s_dwEfiSysTablePtr {0};
static qword_t		 s_qwEfiSysTablePtr {0};
static bool			 s_Initialized {false};

static __vma_t s_vMb2Addr {0};

static Mb2TagVBE		  s_VBEInfo;
static Mb2DhcpAck		  s_DhcpAck;
static Mb2RSDPDescriptor  s_OldAcpi;
static Mb2RSDPDescriptor2 s_NewAcpi;
static BootApm			  s_BootApm;

/////////////////////////////////////////////
//	Extract Modules
static void ExtractModules(const Mb2TagMod *tpTagMod) {

	if (tpTagMod == nullptr) {
		return;
	}

	g_pLogger->Info("[BootInfo] Found boot module: '%s' (0x%x-0x%x)",
					tpTagMod->lpszCmdLine,
					tpTagMod->dwModStart,
					tpTagMod->dwModEnd);

	BootModule *pNew = new BootModule();
	pNew->BaseAddr = tpTagMod->dwModStart;
	pNew->EndAddr = tpTagMod->dwModEnd;
	size_t n = strlen(tpTagMod->lpszCmdLine);
	pNew->lpszModString = new char[n];
	memcpy((void *) pNew->lpszModString, tpTagMod->lpszCmdLine, n);
	pNew->pNext = nullptr;

	if (!g_pBootInfo->pBootModules) {
		g_pBootInfo->pBootModules = pNew;
	}
	else {
		BootModule *p = g_pBootInfo->pBootModules;
		while (p) {
			if (!p->pNext) {
				p->pNext = pNew;
				break;
			}
			p = p->pNext;
		}
	}
}

/////////////////////////////////////////////
//	Extract Boot Devices
static void ExtractBootDevices(const Mb2TagBootDev *tpTagBootDev) {

	if (tpTagBootDev == nullptr) {
		return;
	}

	BiosBootDev *pNew = new BiosBootDev();
	pNew->dwBiosDev = tpTagBootDev->dwBiosDev;
	pNew->dwPartition = tpTagBootDev->dwPart;
	pNew->dwSubPartition = tpTagBootDev->dwSlice;

	g_pBootInfo->pBiosBootDevice = pNew;
}

/////////////////////////////////////////////
//	Extract Memory Map
static void ExtractMemoryMap(const Mb2Tag *tpTagMMap) {

	if (tpTagMMap == nullptr) {
		return;
	}

	Mb2TagMMap *  pTag = (Mb2TagMMap *) tpTagMMap;
	MemMapEntry * pNew = nullptr;
	Mb2MMapEntry *pEntry;
	int			  i;
	for (pEntry = pTag->pEntries, i = 0;
		 (__vma_t) pEntry < (((__vma_t) tpTagMMap) + tpTagMMap->dwSize);
		 i++,
		pEntry = (Mb2MMapEntry *) ((__vma_t) pEntry + pTag->dwEntrySize)) {

		if (pNew) {
			pNew->SetNext(new MemMapEntry(*pEntry));
			pNew = pNew->GetNext();
		}
		else {
			g_pBootInfo->pMemMapEntry = new MemMapEntry(*pEntry);
			pNew = g_pBootInfo->pMemMapEntry;
		}

		pNew->SetNext(nullptr);
	}
}

/////////////////////////////////////////////
//	Extract Frame Buffer
static void ExtractFrameBuffer(const Mb2TagFrameBuffer *tpFrameBuff) {

	if (tpFrameBuff == nullptr)
		return;

	dword_t color;
	dword_t i;
	void *	fb = reinterpret_cast<void *>(tpFrameBuff->common.qwAddress);

	switch (tpFrameBuff->common.bType) {
	case MB2_FRAMEBUFFER_TYPE_INDEXED: {

		unsigned		bestDistance, distance;
		const Mb2Color *palette;

		palette = tpFrameBuff->Palette;

		color = 0;
		bestDistance = 4 * 256 * 256;

		for (i = 0; i < tpFrameBuff->wPaletteNumColors; i++) {
			distance = (0xff - palette[i].blue) * (0xff - palette[i].blue)
					   + palette[i].red * palette[i].red
					   + palette[i].green * palette[i].green;
			if (distance < bestDistance) {
				color = i;
				bestDistance = distance;
			}
		}
	} break;

	case MB2_FRAMEBUFFER_TYPE_RGB: {

		color = ((1 << tpFrameBuff->bBlueMaskSize) - 1)
				<< tpFrameBuff->bBlueFieldPosition;
	} break;

	case MB2_FRAMEBUFFER_TYPE_EGA_TEXT: {

		color = '\\' | 0x0100;
	} break;

	default: color = 0xffffffff; break;
	}

	for (i = 0;
		 i < tpFrameBuff->common.dwWidth && i < tpFrameBuff->common.dwHeight;
		 i++) {
		switch (tpFrameBuff->common.bBpp) {
		case 8: {

			byte_t *pixel = static_cast<byte_t *>((
				void *) ((uintptr_t) fb + tpFrameBuff->common.dwPitch * i + i));
			*pixel = color;
		} break;

		case 15:
		case 16: {

			word_t *pixel = static_cast<word_t *>(
				(void *) ((uintptr_t) fb + tpFrameBuff->common.dwPitch * i
						  + 2 * i));
			*pixel = color;
		} break;

		case 24: {

			dword_t *pixel = static_cast<dword_t *>(
				(void *) ((uintptr_t) fb + tpFrameBuff->common.dwPitch * i
						  + 3 * i));
			*pixel = color;
		} break;

		default: {

			dword_t *pixel = static_cast<dword_t *>(
				(void *) ((uintptr_t) fb + tpFrameBuff->common.dwPitch * i
						  + 4 * i));
			*pixel = color;
		} break;
		}	 // switch (tpFrameBuff->common.bBpp)
	}
}

/////////////////////////////////////////////
//	Extract EFL Sections
static void ExtractEflSections(const Mb2TagElfSect *tpElf) {}

/////////////////////////////////////////////
//	Extract SM BIOS
static void ExtractSMBios(const Mb2TagSMBios *tpSmBios) {}

/////////////////////////////////////////////
//	InitBootInfo
void BootInfoInit() {

	if (s_Initialized) {
		return;
	}

	Logger *pLog = g_pLogger;
	pLog->Info("[BootInfoInit] Initializing multiboot info structure...\n");
	BootParams *pBoot = g_pBootParams;

	if (pBoot->Mb2Address == 0) {
		KernelPanic("[BootInfoInit] Invalid multiboot info address.");
	}

	g_pBootInfo = new Mb2BootInfo();
	s_vMb2Addr = pBoot->Mb2Address;
	Mb2Tag *pTag = reinterpret_cast<Mb2Tag *>(s_vMb2Addr + 8);

	for (pTag = reinterpret_cast<Mb2Tag *>(s_vMb2Addr + 8);
		 pTag->dwType != MB2_TAG_TYPE_END;
		 pTag = (Mb2Tag *) ((byte_t *) pTag + ((pTag->dwSize + 7) & ~7))) {

		switch (pTag->dwType) {
		case MB2_TAG_TYPE_CMDLINE: {

			Mb2TagString *pStr;
			pStr = static_cast<Mb2TagString *>(pTag);
			// s_BootCmdLine = reinterpret_cast<char *>(g_LastUsed);
			// g_LastUsed += pStr->dwSize;
			s_BootCmdLine = new char[pStr->dwSize];
			memcpy(s_BootCmdLine,
				   static_cast<const void *>(pStr->szString),
				   pStr->dwSize);
			g_pBootInfo->lpszBootCmdLine = s_BootCmdLine;

		} break;

		case MB2_TAG_TYPE_BOOT_LOADER_NAME: {

			Mb2TagString *pStr;
			pStr = static_cast<Mb2TagString *>(pTag);
			// s_BootLoaderName = reinterpret_cast<char *>(g_LastUsed);
			// g_LastUsed += pStr->dwSize;
			s_BootLoaderName = new char[pStr->dwSize];
			memcpy(s_BootLoaderName,
				   static_cast<const void *>(pStr->szString),
				   pStr->dwSize);
			g_pBootInfo->lpszBootLoaderName = s_BootLoaderName;
		} break;

		case MB2_TAG_TYPE_MODULE: {

			ExtractModules(static_cast<const Mb2TagMod *>(pTag));
		} break;

		case MB2_TAG_TYPE_BASIC_MEMINFO: {

			Mb2TagMemInfo *pInfo = static_cast<Mb2TagMemInfo *>(pTag);
			BasicMemInfo * pNew = new BasicMemInfo();
			pNew->dwMemLower = pInfo->dwMemLower;
			pNew->dwMemUpper = pInfo->dwMemUpper;

			g_pBootInfo->pBasicMemInfo = pNew;

		} break;

		case MB2_TAG_TYPE_BOOTDEV: {

			ExtractBootDevices(static_cast<const Mb2TagBootDev *>(pTag));
		} break;

		case MB2_TAG_TYPE_MMAP: {

			ExtractMemoryMap(static_cast<const Mb2TagMMap *>(pTag));
		} break;

		case MB2_TAG_TYPE_VBE: {

			memcpy(static_cast<void *>(&s_VBEInfo),
				   static_cast<void *>(pTag),
				   sizeof(Mb2TagVBE));
		} break;

		case MB2_TAG_TYPE_FRAMEBUFFER: {

			// ExtractFrameBuffer(
			// 	reinterpret_cast<const Mb2TagFrameBuffer *>(pTag));
		} break;

		case MB2_TAG_TYPE_ELF_SECTIONS: {

			ExtractEflSections(static_cast<const Mb2TagElfSect *>(pTag));
		} break;

		case MB2_TAG_TYPE_APM: {

			Mb2TagAPM *apm = static_cast<Mb2TagAPM *>(pTag);
			g_pBootInfo->pBootApm = new BootApm();
			BootApm *pNew = g_pBootInfo->pBootApm;
			pNew->wVersion = apm->wVersion;
			pNew->wCSeg = apm->wCSeg;
			pNew->dwOffset = apm->dwOffset;
			pNew->wCSeg16 = apm->wCSeg_16;
			pNew->wDSeg = apm->wDSeg;
			pNew->wFlags = apm->wFlags;
			pNew->wCSegLen = apm->wCSegLen;
			pNew->wCSeg16Len = apm->wCSeg16Len;
			pNew->wDSegLen = apm->wDSegLen;
		} break;

		case MB2_TAG_TYPE_EFI32: {

			s_dwEfiSysTablePtr = (static_cast<Mb2TagEFI32 *>(pTag)->dwPointer);
		} break;

		case MB2_TAG_TYPE_EFI64: {

			s_qwEfiSysTablePtr = (static_cast<Mb2TagEFI64 *>(pTag)->dwPointer);
		} break;

		case MB2_TAG_TYPE_SMBIOS: {

			ExtractSMBios(static_cast<const Mb2TagSMBios *>(pTag));
		} break;

		case MB2_TAG_TYPE_ACPI_OLD: {

			memcpy(static_cast<void *>(&s_OldAcpi),
				   static_cast<const void *>(
					   static_cast<Mb2TagOldACPI *>(pTag)->bRsdp),
				   sizeof(Mb2RSDPDescriptor));
		} break;

		case MB2_TAG_TYPE_ACPI_NEW: {

			memcpy(static_cast<void *>(&s_NewAcpi),
				   static_cast<const void *>(
					   static_cast<Mb2TagNewACPI *>(pTag)->bRsdp),
				   sizeof(Mb2RSDPDescriptor2));
		} break;

		case MB2_TAG_TYPE_NETWORK: {

			memcpy(static_cast<void *>(&s_DhcpAck),
				   static_cast<const void *>(
					   static_cast<Mb2TagNetwork *>(pTag)->bDhcpAck),
				   sizeof(Mb2DhcpAck));
		} break;

		case MB2_TAG_TYPE_EFI_BS: {

			//	If this tag exists, it means ExitBootServices wasn't called
			s_ExitBootServiceNotCalled = true;
		} break;

		case MB2_TAG_TYPE_EFI32_IH: {

			s_dwEfi32ImgHandlePtr
				= (static_cast<Mb2TagEFI32IH *>(pTag))->dwPointer;
		} break;

		case MB2_TAG_TYPE_EFI64_IH: {

			s_qwEfi64ImgHandlePtr
				= (static_cast<Mb2TagEFI64IH *>(pTag))->qwPointer;
		} break;

		case MB2_TAG_TYPE_LOAD_BASE_ADDR: {

			s_dwLoadBaseAddr
				= (static_cast<Mb2TagLoadBaseAddr *>(pTag))->dwLoadBaseAddr;
		} break;

		default: break;
		}
	}

	s_Initialized = true;
}	 // namespace qkrnl

/////////////////////////////////////////////
//	GetMb2MemoryMapTag
int BootInfoEnumMemMap(MemMapEntry *tlpMemMap, int_t tCb, int_t *tlpCbNeeded) {

	if (!s_Initialized) {
		return 0;
	}

	int_t i = 0;

	MemMapEntry *p = tlpMemMap;	   //	Local copy
	int_t		 max = tCb / sizeof(MemMapEntry);
	while (!s_aMemMap[i].IsEmpty() && i < max) {

		p->FromMb2(&s_aMemMap[i]);
		p++;
		i++;
	}

	*tlpCbNeeded = i * sizeof(MemMapEntry);
	return 1;
}

/////////////////////////////////////////////
//	BootInfoGetBasicMemInfo
int BootInfoGetBasicMemInfo(qword_t *tqwLoMem, qword_t *tqwHiMem) {

	*tqwLoMem = g_pBootInfo->pBasicMemInfo->dwMemLower * 1024;
	*tqwHiMem = g_pBootInfo->pBasicMemInfo->dwMemUpper * 1024;

	return 1;
}

/////////////////////////////////////////////
//	Debug Print
void BootInfoDebugPrint(uintptr_t tdwMb2Addr) {

	Mb2Tag *tag;
	dword_t dwSize = *(dword_t *) tdwMb2Addr;
	Logger *pLogger = g_pLogger;
	pLogger->Info("Announce mbi size 0x%X\n", dwSize);
	for (tag = (Mb2Tag *) (tdwMb2Addr + 8); tag->dwType != MB2_TAG_TYPE_END;
		 tag = (Mb2Tag *) ((byte_t *) tag + ((tag->dwSize + 7) & ~7))) {
		pLogger->Info("Tag 0x%x, Size 0x%x\n", tag->dwType, tag->dwSize);
		switch (tag->dwType) {
		case MB2_TAG_TYPE_CMDLINE:
			pLogger->Info("Command line = %s\n",
						  ((Mb2TagString *) tag)->szString);
			break;
		case MB2_TAG_TYPE_BOOT_LOADER_NAME:
			pLogger->Info("Boot loader name = %s\n",
						  ((Mb2TagString *) tag)->szString);
			break;
		case MB2_TAG_TYPE_MODULE:
			pLogger->Info("Module at 0x%x-0x%x. Command line %s\n",
						  ((Mb2TagMod *) tag)->dwModStart,
						  ((Mb2TagMod *) tag)->dwModEnd,
						  ((Mb2TagMod *) tag)->lpszCmdLine);
			break;
		case MB2_TAG_TYPE_BASIC_MEMINFO:
			pLogger->Info("mem_lower = %uKB, mem_upper = %uKB\n",
						  ((Mb2TagMemInfo *) tag)->dwMemLower,
						  ((Mb2TagMemInfo *) tag)->dwMemUpper);
			break;
		case MB2_TAG_TYPE_BOOTDEV:
			pLogger->Info("Boot device 0x%x,%u,%u\n",
						  ((Mb2TagBootDev *) tag)->dwBiosDev,
						  ((Mb2TagBootDev *) tag)->dwSlice,
						  ((Mb2TagBootDev *) tag)->dwPart);
			break;
		case MB2_TAG_TYPE_MMAP: {
			Mb2MMapEntry *mmap;

			pLogger->Info("mmap\n");

			for (mmap = ((Mb2TagMMap *) tag)->pEntries;
				 (byte_t *) mmap < (byte_t *) tag + tag->dwSize;
				 mmap = (Mb2MMapEntry *) ((unsigned long) mmap
										  + ((Mb2TagMMap *) tag)->dwEntrySize))
				pLogger->Info(" base_addr = 0x%x%x,"
							  " length = 0x%x%x, type = 0x%x\n",
							  (unsigned) (mmap->qwAddress >> 32),
							  (unsigned) (mmap->qwAddress & 0xffffffff),
							  (unsigned) (mmap->qwLength >> 32),
							  (unsigned) (mmap->qwLength & 0xffffffff),
							  (unsigned) mmap->dwType);
		} break;
		case MB2_TAG_TYPE_FRAMEBUFFER: {
			dword_t			   color;
			unsigned		   i;
			Mb2TagFrameBuffer *tagfb = (Mb2TagFrameBuffer *) tag;
			void *fb = (void *) (unsigned long) tagfb->common.qwAddress;

			switch (tagfb->common.bType) {
			case MB2_FRAMEBUFFER_TYPE_INDEXED: {
				unsigned  best_distance, distance;
				Mb2Color *palette;

				palette = tagfb->Palette;

				color = 0;
				best_distance = 4 * 256 * 256;

				for (i = 0; i < tagfb->wPaletteNumColors; i++) {
					distance
						= (0xff - palette[i].blue) * (0xff - palette[i].blue)
						  + palette[i].red * palette[i].red
						  + palette[i].green * palette[i].green;
					if (distance < best_distance) {
						color = i;
						best_distance = distance;
					}
				}
			} break;

			case MB2_FRAMEBUFFER_TYPE_RGB:
				color = ((1 << tagfb->bBlueMaskSize) - 1)
						<< tagfb->bBlueFieldPosition;
				break;

			case MB2_FRAMEBUFFER_TYPE_EGA_TEXT: color = '\\' | 0x0100; break;

			default: color = 0xffffffff; break;
			}

			for (i = 0; i < tagfb->common.dwWidth && i < tagfb->common.dwHeight;
				 i++) {
				switch (tagfb->common.bBpp) {
				case 8: {
					byte_t *pixel
						= (byte_t *) ((uintptr_t) fb + tagfb->common.dwPitch * i
									  + i);
					*pixel = color;
				} break;
				case 15:
				case 16: {
					word_t *pixel
						= (word_t *) ((uintptr_t) fb + tagfb->common.dwPitch * i
									  + 2 * i);
					*pixel = color;
				} break;
				case 24: {
					dword_t *pixel
						= (dword_t *) ((uintptr_t) fb
									   + tagfb->common.dwPitch * i + 3 * i);
					*pixel = (color & 0xffffff) | (*pixel & 0xff000000);
				} break;

				case 32: {
					dword_t *pixel
						= (dword_t *) ((uintptr_t) fb
									   + tagfb->common.dwPitch * i + 4 * i);
					*pixel = color;
				} break;
				}
			}
			break;
		}
		}
	}
	tag = (Mb2Tag *) ((byte_t *) tag + ((tag->dwSize + 7) & ~7));
	pLogger->Info("Total mbi size 0x%x\n", (uintptr_t) tag - tdwMb2Addr);
}

/////////////////////////////////////////////
//	MemMapEntry implementation
MemMapEntry::MemMapEntry()
	: m_eType(MMapEntryType::Available)
	, m_qwBaseAddr(0)
	, m_qwLength(0) {}

MemMapEntry::MemMapEntry(Mb2MMapEntry &entry)
	: m_eType(GetEntryType(entry.dwType))
	, m_qwBaseAddr(entry.qwAddress)
	, m_qwLength(entry.qwLength) {}

MMapEntryType MemMapEntry::GetEntryType(dword_t dwType) noexcept {

	switch (dwType) {
	case 1: return MMapEntryType::Available;
	case 2: return MMapEntryType::Reserved;
	case 3: return MMapEntryType::AcpiReclaimable;
	case 4: return MMapEntryType::NVS;
	default: return MMapEntryType::BadRam;
	}
}

void MemMapEntry::FromMb2(const Mb2MMapEntry *entry) noexcept {

	m_qwBaseAddr = entry->qwAddress;
	m_qwLength = entry->qwLength;
	m_eType = GetEntryType(entry->dwType);
}

}	 // namespace qkrnl