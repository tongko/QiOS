#include <LinkedList.hpp>
#include <cdefs.h>
#include <kernel/Error.hpp>
#include <kernel/Logger.hpp>
#include <kernel/system/interrupts/LocalApic.hpp>
#include <kernel/system/processor/Processor.hpp>
#include <kernel/types.hpp>
#include <string.hpp>

using namespace qklib;
using namespace qkrnl::system::interrupts;

extern qkrnl::Logger *g_pLogger;

namespace qkrnl { namespace system { namespace processor {

static LinkedList<Processor> *s_Processors {0};
static dword_t *s_IdMap {0};	//	Apic to processor logical id map.

bool ProcessorSupportsCpuId() {
	return _probeCPUID();
}

void ProcessorCpuId(dword_t	 tdwCode,
					dword_t &tdwOutA,
					dword_t &tdwOutB,
					dword_t &tdwOutC,
					dword_t &tdwOutD) {

	_ASMV_("cpuid"
		   : "=a"(tdwOutA), "=b"(tdwOutB), "=c"(tdwOutC), "=d"(tdwOutD)
		   : "a"(tdwCode));
}

void ProcessorInitializeBsp() {

	if (!ProcessorSupportsCpuId()) {
		KernelPanic("[qkrnl::Processor] Processor has no CPUID support.");
	}

	char *szBuffer = new char[200];
	ProcessorGetInfo(szBuffer);
	g_pLogger->Info(szBuffer);
	ProcessorEnableSSE();

	if (!ProcessorSupports(CpuIdStandardEdxFeatures::APIC)) {
		KernelPanic("[qkrnl::Processor] Processor has no APIC.");
	}
}

void ProcessorInitializes() {

	ProcessorEnableSSE();
}

void ProcessorAdd(dword_t tdwApicId, dword_t tdwHardwareId) {

	if (s_Processors == nullptr) {
		s_Processors = new LinkedList<Processor>();
	}

	// auto predicate
	// 	= [&](Processor &p) -> bool { return tdwApicId == p.dwApicId; };
	if (!s_Processors->Find(
			[&](Processor *x) { return x->dwApicId == tdwApicId; })) {
		g_pLogger->Warn("[qkrnl::Processor] WARN! Attempt to add core "
						"with duplicate apic id %d",
						tdwApicId);
		return;
	}

	Processor *pCore = new Processor();
	pCore->dwId = tdwHardwareId;
	pCore->dwHardwareId = tdwHardwareId;
	pCore->dwApicId = tdwApicId;

	if (tdwApicId == LocalApicReadId()) {
		pCore->fIsBsp = true;
	}

	s_Processors->AddNode(pCore);
}

// LinkedList<Processor> *ProcessorGetList(void) {
// 	return s_Processors;
// }

word_t ProcessorGetNumberOfProcessors() {

	return s_Processors->Count();
}

word_t ProcessorGetCurrentId() {

	if (s_IdMap == nullptr) {
		return 0;
	}

	return s_IdMap[LocalApicReadId()];
}

void ProcessorApicIdCreateMappingTable() {

	dword_t apicId = 0;
	for (int i = 0; i < s_Processors->Count(); i++) {
		Processor *ptr = (*s_Processors)[i];
		if (ptr->dwApicId > apicId) {
			apicId = ptr->dwApicId;
		}
	}

	if (apicId > 0x400) {
		KernelPanic("[qkrnl::Processor] Abnormally high apic id detected: %d",
					apicId);
	}

	dword_t *map = new dword_t[apicId + 1];
	memsetdw((void *) map, 0, apicId + 1);
	for (int i = 0; i < s_Processors->Count(); i++) {
		Processor *p = (*s_Processors)[i];
		map[p->dwApicId] = p->dwId;
	}

	s_IdMap = map;
}

bool ProcessorSupports(CpuIdStandardEdxFeatures feature) {

	dword_t eax;
	dword_t ebx;
	dword_t ecx;
	dword_t edx;
	ProcessorCpuId(1, eax, ebx, ecx, edx);
	return (edx & (qword_t) feature);
}

bool ProcessorSupports(CpuIdExtendedEcxFeatures feature) {

	dword_t eax;
	dword_t ebx;
	dword_t ecx;
	dword_t edx;
	ProcessorCpuId(1, eax, ebx, ecx, edx);
	return (ecx & (qword_t) feature);
}

void ProcessorGetInfo(char *tpBuffer) {

	char *p = tpBuffer;
	char  vendor[13];
	ProcessorGetVendor(vendor);
	vendor[12] = 0;

	strcat(strcat(strcat(p, "Vendor: "), vendor), "\n");

	dword_t eax, ebx, ecx, edx;
	ProcessorCpuId(1, eax, ebx, ecx, edx);

	strcat(p, "Available features:");
	if (edx & (dword_t) CpuIdStandardEdxFeatures::PAE) {
		strcat(p, " PAE");
	}
	if (edx & (dword_t) CpuIdStandardEdxFeatures::MMX) {
		strcat(p, " MMX");
	}
	if (edx & (dword_t) CpuIdStandardEdxFeatures::SSE) {
		strcat(p, " SSE");
	}
	if (edx & (dword_t) CpuIdStandardEdxFeatures::SSE2) {
		strcat(p, " SSE2");
	}
}

void ProcessorEnableSSE() {

	if (ProcessorSupports(CpuIdStandardEdxFeatures::SSE)) {
		_enableSSE();
		g_pLogger->Info("[qkrnl::Processor] SSE support enabled.");
	}
	else {
		g_pLogger->Info("[qkrnl::Processor] SSE not supported.");
	}
}

void ProcessorGetVendor(char *tpVendor) {

	dword_t eax, ebx, ecx, edx;
	ProcessorCpuId(0, eax, ebx, ecx, edx);

	dword_t *v = (dword_t *) tpVendor;
	v[0] = ebx;
	v[1] = edx;
	v[2] = ecx;
}

void ProcessorReadMsr(dword_t tdwMsr, dword_t *tdwLo, dword_t *tdwHi) {

	_ASMV_("rdmsr" : "=a"(*tdwLo), "=d"(*tdwHi) : "c"(tdwMsr));
}

void ProcessorWriteMsr(dword_t tdwMsr, dword_t tdwLo, dword_t tdwHi) {

	_ASMV_("wrmsr" : : "a"(tdwLo), "d"(tdwHi), "c"(tdwMsr));
}

dword_t ProcessorReadEFlags() {

	dword_t eflags;
	_ASMV_("pushf\n"
		   "pop [%0]"
		   : "=g"(eflags));

	return eflags;
}

}}}	   // namespace qkrnl::system::processor