#pragma once

#include <kernel/types.hpp>

namespace qklib {
//	Prototypes
template <typename T>
class LinkedList;
}	 // namespace qklib

namespace qkrnl { namespace system { namespace processor {

/**
 * CPUID feature flags (EDX)
 */
enum class CpuIdStandardEdxFeatures {
	FPU = 1 << 0,			   // Onboard x87 FPU
	VME = 1 << 1,			   // Virtual 8086 supported
	DE = 1 << 2,			   // Debugging extensions
	PSE = 1 << 3,			   // Page size extension
	TSC = 1 << 4,			   // Time stamp counter
	MSR = 1 << 5,			   // Model specific registers
	PAE = 1 << 6,			   // Physical address extension
	MCE = 1 << 7,			   // Machine check exception
	CX8 = 1 << 8,			   // CMPXCHG8 instruction
	APIC = 1 << 9,			   // APIC available
	SEP = 1 << 11,			   // SYSENTER / SYSEXIT
	MTRR = 1 << 12,			   // Memory type range registers
	PGE = 1 << 13,			   // Page global enable bit in CR4
	MCA = 1 << 14,			   // Machine check architecture
	CMOV = 1 << 15,			   // Cond. move / FCMOV instructions
	PAT = 1 << 16,			   // Page attribute table
	PSE36 = 1 << 17,		   // 36 bit page size extension
	PSN = 1 << 18,			   // Processor serial number
	CLF = 1 << 19,			   // CLFLUSH instruction
	DTES = 1 << 21,			   // Debug store available
	ACPI_THERMAL = 1 << 22,	   // on-board thermal control MSRs
	MMX = 1 << 23,			   // MMX instructions
	FXSR = 1 << 24,			   // FXSAVE, FXRESTOR instructions
	SSE = 1 << 25,			   // Streaming SIMD Extensions
	SSE2 = 1 << 26,			   // Streaming SIMD Extensions 2
	SS = 1 << 27,			   // CPU cache supports self-snoop
	HTT = 1 << 28,			   // Hyperthreading
	TM1 = 1 << 29,			   // Thermal monitor auto-limits temperature
	IA64 = 1 << 30,			   // Processor is IA64 that emulates x86
	PBE = 1 << 31,			   // Pending break enable wakeup support
};

/**
 * CPUID feature flags (ECX)
 */
enum class CpuIdExtendedEcxFeatures {
	SSE3 = 1 << 0,
	PCLMUL = 1 << 1,
	DTES64 = 1 << 2,
	MONITOR = 1 << 3,
	DS_CPL = 1 << 4,
	VMX = 1 << 5,
	SMX = 1 << 6,
	EST = 1 << 7,
	TM2 = 1 << 8,
	SSSE3 = 1 << 9,
	CID = 1 << 10,
	FMA = 1 << 12,
	CX16 = 1 << 13,
	ETPRD = 1 << 14,
	PDCM = 1 << 15,
	DCA = 1 << 18,
	SSE4_1 = 1 << 19,
	SSE4_2 = 1 << 20,
	x2APIC = 1 << 21,
	MOVBE = 1 << 22,
	POPCNT = 1 << 23,
	AES = 1 << 25,
	XSAVE = 1 << 26,
	OSXSAVE = 1 << 27,
	AVX = 1 << 28
};

struct Processor {
	dword_t dwId;
	dword_t dwHardwareId;
	dword_t dwApicId;
	bool	fIsBsp;
};

/**
 * Probing processor whether the processor supports CPUID instruction.
 * in kernel/arch/{target_arch}/processor.asm
 */
extern "C" bool _probeCPUID();

/**
 * Return result is true if processor supports CPUID.
 */
bool ProcessorSupportsCpuId();

/**
 * Execute CPUID instruction and returns values in the out parameters.
 * @param tdwCode CPUID tdwCode.
 * @param[out] tdwOutA Result return by CPUID in eax.
 * @param[out] tdwOutB Result return by CPUID in ebx.
 * @param[out] tdwOutC Result return by CPUID in ecx.
 * @param[out] tdwOutD Result return by CPUID in edx.
 */
void ProcessorCpuId(dword_t	 tdwCode,
					dword_t &tdwOutA,
					dword_t &tdwOutB,
					dword_t &tdwOutC,
					dword_t &tdwOutD);

/**
 * Enables SSE on current processor.
 * in kernel/arch/{target_arch}/processor.asm
 */
extern "C" void _enableSSE();

/**
 * Initialize the bootstrap processor.
 */
void ProcessorInitializeBsp();

/**
 * Initialize a processor. Called for all processors in the system.
 */
void ProcessorInitializes();

/**
 * Add processor to list of processors. This is called when MADT tables are
 * parsed. Each processor gets a consecutive id assigned. This might not be the
 * processors hardward id.
 *
 * @param tdwApicId Id of the local APIC.
 * @param tdwHardwareId Processor id provided in MADT.
 */
void ProcessorAdd(dword_t tdwApicId, dword_t tdwHardwareId);

// /**
//  * Returnes the list of processors.
//  */
// LinkedList<Processor> *ProcessorGetList(void);

/**
 * Get the number of processors.
 */
word_t ProcessorGetNumberOfProcessors();

/**
 * Get logical id of current processor.
 */
word_t ProcessorGetCurrentId();

/**
 * Creates APIC id to processor logical id mapping table.
 */
void ProcessorApicIdCreateMappingTable();

/**
 * Checks if processor supports specified EDX feature.
 */
bool ProcessorSupports(CpuIdStandardEdxFeatures features);

/**
 * Checks if processor supports specified ECX feature.
 */
bool ProcessorSupports(CpuIdExtendedEcxFeatures features);

/**
 * Get processor info, into specified buffer.
 */
void ProcessorGetInfo(char *tpBuffer);

/**
 * Enable SSE on current processor.
 */
void ProcessorEnableSSE();

/**
 * Get processor vendor. "out" must be a pointer to a buffer of at least 12
 * bytes long.
 */
void ProcessorGetVendor(char *tpVendor);

/**
 * Reads model-specific register.
 */
void ProcessorReadMsr(dword_t tdwMsr, dword_t *tdwLo, dword_t *tdwHi);

/**
 * Writes model-specific register
 */
void ProcessorWriteMsr(dword_t tdwMsr, dword_t tdwLo, dword_t tdwHhi);

/**
 * REads the EFLAGS register.
 */
dword_t ProcessorReadEFlags();

}}}	   // namespace qkrnl::system::processor