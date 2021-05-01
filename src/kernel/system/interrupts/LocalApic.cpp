#include <kernel/Logger.hpp>
#include <kernel/mm/MemoryManager.hpp>
#include <kernel/mm/Paging.hpp>
#include <kernel/mm/mm_common.hpp>
#include <kernel/system/interrupts/LocalApic.hpp>
#include <kernel/system/interrupts/Pit.hpp>
#include <kernel/types.hpp>

using namespace qkrnl::mm;

extern qkrnl::Logger *g_pLogger;

namespace qkrnl { namespace system { namespace interrupts {

static bool	   s_fPrepared {false};
static __pma_t s_physBase {0};
static __vma_t s_virtBase {0};

void LocalApicPrepare(__pma_t tApicAddr) {

	s_physBase = tApicAddr;

	//	APIC not at expected location... well, not a big deal
	if (s_physBase != APIC_PHYSICAL_ADDRESS) {
		g_pLogger->Warn(
			"[WARN][APIC::Prepare] APIC at 0x%x%x, expected at 0x00000000%x\n",
			(dword_t)(s_physBase >> 32),
			(dword_t)(s_physBase & 0xFFFFFFFF),
			(dword_t)(APIC_PHYSICAL_ADDRESS & 0xFFFFFFFF));
	}

	g_pLogger->Debug("[DBG][APIC::Prepare] APIC base: 0x%x%x\n", s_physBase);

	LocalApicCreateMapping();
	s_fPrepared = true;
}

bool LocalApicIsPrepare() {

	return s_physBase;
}

void LocalApicInitialize() {

	auto verRegVal = LocalApicRead(APIC_REG_VERSION);
	auto localId = LocalApicReadId();
	auto version = verRegVal & 0xFF;
	auto maxRedirEntries = (verRegVal >> 16) & 0xFF;
	g_pLogger->Debug("[DBG][APIC::Initialize] ID: %d, Version: %x (%s), Max "
					 "Redir Entries: %d\n",
					 localId,
					 version,
					 version < 0x10 ? "82489DX discrete" : "intergrated",
					 maxRedirEntries);

	//	Init to well-known state
	LocalApicWrite(APIC_REG_DEST_FORMAT, 0xFFFFFFFF);
	LocalApicWrite(APIC_REG_LOGICAL_DEST,
				   (LocalApicRead(APIC_REG_LOGICAL_DEST) & 0x00FFFFFF) | 1);
	LocalApicWrite(APIC_REG_LVT_TIMER, APIC_LVT_INT_MASKED);
	LocalApicWrite(APIC_REG_LVT_PERFMON, APIC_LVT_DELIVERY_MODE_NMI);
	LocalApicWrite(APIC_REG_LVT_LINT0, APIC_LVT_INT_MASKED);
	LocalApicWrite(APIC_REG_LVT_LINT1, APIC_LVT_INT_MASKED);
	LocalApicWrite(APIC_REG_TASK_PRIO, 0);

	// Enable APIC
	LocalApicWrite(APIC_REG_SPURIOUS_IVT,
				   0xFF | APIC_SPURIOUS_IVT_SOFTWARE_ENABLE);

	// Set up timer
	LocalApicStartTimer();
}

dword_t LocalApicReadId() {

	if (!s_fPrepared) {
		return 0;
	}

	return (LocalApicRead(APIC_REG_ID) >> 24) & 0xFF;
}

void LocalApicCreateMapping() {

	s_virtBase
		= (__vma_t) MemoryManager::VirtualAlloc(1, PageSizes::FourK, true);
	if (!s_virtBase) {
		KernelPanic("[APIC::Mapping] Failed to allocate virtual address range "
					"for mapping.");
	}

	// Default APIC base address (physical) is 0xFEE00000, so we could map this
	// to allocated virtual address range.
	PagingMapPage(s_virtBase,
				  s_physBase,
				  DEFAULT_TABLE_FLAGS,
				  DEFAULT_PAGE_FLAGS | PG_CACHE_DISABLED);
}

void LocalApicStartTimer() {

	g_pLogger->Debug("[DBG][APIC::StartTimer] Starting timer.\n");

	//	Use divider 16
	LocalApicWrite(APIC_REG_TIMER_DIV, 0x3);

	// Sleep for 10ms
	Pit::PrepareSleep(10000);
	// Set APIC init counter to -1
	LocalApicWrite(APIC_REG_TIMER_INITCNT, 0xFFFFFFFF);

	// Sleep
	Pit::Sleep();

	// Stop APIC timer
	LocalApicWrite(APIC_REG_LVT_TIMER, APIC_LVT_INT_MASKED);

	auto ticksPer10ms = 0xFFFFFFFF - LocalApicRead(APIC_REG_TIMER_CURRCNT);

	// Start timer
	LocalApicWrite(APIC_REG_LVT_TIMER, 32 | APIC_LVT_TIMER_MODE_PERIODIC);
	LocalApicWrite(APIC_REG_TIMER_DIV, 0x3);
	LocalApicWrite(APIC_REG_TIMER_INITCNT, ticksPer10ms / 10);
}

dword_t LocalApicRead(dword_t tdwReg) {

	return *((volatile dword_t *) (s_virtBase + tdwReg));
}

void LocalApicWrite(dword_t tdwReg, dword_t tdwValue) {

	*((volatile dword_t *) (s_virtBase + tdwReg)) = tdwValue;
}

void LocalApicWaitForIcrSend() {

	while (
		APIC_LVT_GET_DELIVERY_STATUS(LocalApicRead(APIC_REG_INT_COMMAND_HIGH))
		== APIC_ICR_DELIVS_SEND_PENDING) {}
}

void LocalApicSendEndOfInt() {

	LocalApicWrite(APIC_REG_EOI, 0);
}

}}}	   // namespace qkrnl::system::interrupts