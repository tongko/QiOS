#pragma once

#include <kernel/types.hpp>

namespace qkrnl { namespace system { namespace interrupts {

#define APIC_PHYSICAL_ADDRESS 0xFEE00000	//	??

/**
 * APIC register offsets (to APICs base address)
 */
#define APIC_REG_ID					0x20
#define APIC_REG_VERSION			0x30
#define APIC_REG_TASK_PRIO			0x80
#define APIC_REG_ARBITRATION_PRIO	0x90
#define APIC_REG_EOI				0xB0
#define APIC_REG_LOGICAL_DEST		0xD0
#define APIC_REG_DEST_FORMAT		0xE0
#define APIC_REG_SPURIOUS_IVT		0xF0
#define APIC_REG_ERROR_STATUS		0x280
#define APIC_REG_LVT_CMCI			0x2F0
#define APIC_REG_INT_COMMAND_LOW	0x300
#define APIC_REG_INT_COMMAND_HIGH	0x310
#define APIC_REG_LVT_TIMER			0x320
#define APIC_REG_LVT_THERMAL_SENSOR 0x320
#define APIC_REG_LVT_PERFMON		0x340
#define APIC_REG_LVT_LINT0			0x350
#define APIC_REG_LVT_LINT1			0x360
#define APIC_REG_LVT_ERROR			0x370
#define APIC_REG_TIMER_INITCNT		0x380
#define APIC_REG_TIMER_CURRCNT		0x390
#define APIC_REG_TIMER_DIV			0x3E0

/**
 * Spurious interrupt entry flags
 */
#define APIC_SPURIOUS_IVT_SOFTWARE_ENABLE 0x100

/**
 * LVT flags
 */
#define APIC_LVT_DELIVERY_MODE_FIXED	 (0 << 8)	 // 000........
#define APIC_LVT_DELIVERY_MODE_SMI		 (2 << 8)	 // 010........
#define APIC_LVT_DELIVERY_MODE_NMI		 (4 << 8)	 // 100........
#define APIC_LVT_DELIVERY_MODE_INIT		 (5 << 8)	 // 101........
#define APIC_LVT_DELIVERY_MODE_EXTINT	 (7 << 8)	 // 111........
#define APIC_LVT_GET_DELIVERY_STATUS(i)	 ((i >> 12) & 1)
#define APIC_LVT_INT_POLARITY_HIGH		 (1 << 13)
#define APIC_LVT_INT_POLARITY_LOW		 (0 << 13)
#define APIC_LVT_GET_REMOTE_IRR(i)		 ((i << 14) & 1)
#define APIC_LVT_TRIGGER_MODE_EDGE		 (0 << 15)
#define APIC_LVT_TRIGGER_MODE_LEVEL		 (1 << 15)
#define APIC_LVT_INT_UNMASKED			 (0 << 16)
#define APIC_LVT_INT_MASKED				 (1 << 16)
#define APIC_LVT_TIMER_MODE_ONESHOT		 (0 << 17)	  // 00
#define APIC_LVT_TIMER_MODE_PERIODIC	 (1 << 17)	  // 01
#define APIC_LVT_TIMER_MODE_TSC_DEADLINE (2 << 17)	  // 10

/**
 * Interrupt command register (ICR) parts
 */
#define APIC_ICR_INTVEC_MAKE				  (0xFF)
#define APIC_ICR_DELMOD_FIXED				  (0 << 8)	  // 000........
#define APIC_ICR_DELMOD_LOWEST				  (1 << 8)	  // 001........
#define APIC_ICR_DELMOD_SMI					  (2 << 8)	  // 010........
#define APIC_ICR_DELMOD_NMI					  (4 << 8)	  // 100........
#define APIC_ICR_DELMOD_INIT				  (5 << 8)	  // 101........
#define APIC_ICR_DELMOD_SIPI				  (6 << 8)	  // 110........
#define APIC_ICR_DESTMOD_PHYSICAL			  (0 << 11)
#define APIC_ICR_DESTMOD_LOGICAL			  (1 << 11)
#define APIC_ICR_DELIVS_IDLE				  (0 << 12)
#define APIC_ICR_DELIVS_SEND_PENDING		  (1 << 12)
#define APIC_ICR_LEVEL_DEASSERT				  (0 << 14)
#define APIC_ICR_LEVEL_ASSERT				  (1 << 14)
#define APIC_ICR_TRIGGERMOD_EDGE			  (0 << 15)
#define APIC_ICR_TRIGGERMOD_LEVEL			  (1 << 15)
#define APIC_ICR_DEST_SHORTHAND_NONE		  (0 << 18)
#define APIC_ICR_DEST_SHORTHAND_SELF		  (1 << 18)
#define APIC_ICR_DEST_SHORTHAND_ALL_WITH_SELF (2 << 18)
#define APIC_ICR_DEST_SHORTHAND_ALL_NOT_SELF  (3 << 18)
#define APIC_ICR_DESTINATION_MAKE(i)		  (((uint64_t) i & 0xFF) << 56)

// the APIC timer is configured to tick every 1 millisecond.
#define APIC_MILLISECONDS_PER_TICK 1

void LocalApicPrepare(__pma_t tApicAddr);

bool LocalApicIsPrepare();

void LocalApicInitialize();

dword_t LocalApicReadId();

void LocalApicCreateMapping();

void LocalApicStartTimer();

dword_t LocalApicRead(dword_t tdwReg);

void LocalApicWrite(dword_t tdwReg, dword_t tdwValue);

void LocalApicWaitForIcrSend();

void LocalApicSendEndOfInt();

}}}	   // namespace qkrnl::system::interrupts