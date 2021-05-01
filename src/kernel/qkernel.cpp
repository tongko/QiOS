#include <cdefs.h>
#include <kernel/KTextMode.hpp>
#include <kernel/Logger.hpp>
#include <kernel/bootinfo.hpp>
#include <kernel/mm/MemoryManager.hpp>
#include <kernel/qkernel.hpp>
#include <kernel/types.hpp>
#include <new>
#include <stdio.hpp>

using namespace qkrnl;
using namespace qklib;

//	Global variable
extern __vma_t	   g_LastUsed;
extern Logger *	   g_pLogger;
extern BootParams *g_pBootParams;

//	Functions prototype
void SetupMemoryManager(void);
void SetupLogging();

//	Main entry point of QiOS kernel
void KernelMain(__vma_t tBootParams) {

	// At this point, Text Mode initialized in 80x25 Gray On Black, paging
	// should have already been setup, temporary or permenantly, but we
	// still need to setup a paging by calling InitPaging in order to disabled
	// identity-mapped addresses.

	//	Missing boot info, abort!
	if (tBootParams == 0) {
		KernelPanic("[KernelMain] tBootParams not set.");
	}

	g_pBootParams = reinterpret_cast<BootParams *>(tBootParams);
	if (g_pBootParams->KernelEnd == 0 || g_pBootParams->Mb2Address == 0) {
		KernelPanic("[KernelMain] %s not set.",
					!g_pBootParams->KernelEnd ? "tBootParams->KernelEnd"
											  : "tBootParams->Mb2Address");
	}

	g_pHeap = nullptr;
	//	Last used address.
	g_LastUsed = g_pBootParams->KernelEnd;

	//	Setup logging
	SetupLogging();
	//	Initialize boot info
	BootInfoInit();
	// Initialize memory management
	qkrnl::mm::MemoryManager::Initialize();

	for (;;)
		_ASMV_("hlt");

	_CODE_UNREACHABLE_
}

void SetupLogging() {

	g_pLogger = new Logger();
	g_pLogger->SetConsoleEnabled(true);
	g_pLogger->SetSerialPortEnabled(true);
	g_pLogger->Info("QiOS (0.0.0.1)\n\n");
}

extern "C" void _kmain(__vma_t tBootParams) {

	KernelMain(tBootParams);
}