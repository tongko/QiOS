#include <kernel/Logger.hpp>
#include <kernel/portio.hpp>
#include <kernel/system/interrupts/Pit.hpp>
#include <kernel/types.hpp>

using namespace qkrnl;

extern Logger *g_pLogger;

namespace qkrnl { namespace system { namespace interrupts {

const dword_t PIT_REG_COUNTER0 = 0x40;
const dword_t PIT_REG_COUNTER1 = 0x41;
const dword_t PIT_REG_COUNTER2 = 0x42;
const dword_t PIT_REG_COMMAND = 0x43;

//	Global tick count
static volatile dword_t s_pit_ticks {0};
static volatile dword_t s_SleepDivisor {0};

static const dword_t PIT_FREQ = 1193180;

dword_t Pit::GetTickCount() {

	return s_pit_ticks;
}

dword_t Pit::SetTickCount(dword_t tdwTicks) {

	qword_t ret = s_pit_ticks;
	s_pit_ticks = tdwTicks;
	return ret;
}

void Pit::SendCommand(byte_t tbCmd) {

	outportb(PIT_REG_COMMAND, tbCmd);
}

void Pit::SendData(word_t twCounter, byte_t tbData) {

	byte_t port = twCounter == PIT_OCW_COUNTER_0
					  ? PIT_REG_COUNTER0
					  : twCounter == PIT_OCW_COUNTER_1 ? PIT_REG_COUNTER1
													   : PIT_REG_COUNTER2;

	return outportb(port, tbData);
}

byte_t Pit::ReadData(word_t twCounter) {

	byte_t port = twCounter == PIT_OCW_COUNTER_0
					  ? PIT_REG_COUNTER0
					  : twCounter == PIT_OCW_COUNTER_1 ? PIT_REG_COUNTER1
													   : PIT_REG_COUNTER2;

	return inportb(port);
}

void Pit::StartCounter(dword_t tdwFreq, byte_t tbCounter, byte_t tbMode) {

	if (tdwFreq == 0) {
		g_pLogger->Debug("[DBG][PIT::StartCounter] Invalid frequency value.\n");
		return;
	}

	auto divisor = word_t(PIT_FREQ / (word_t) tdwFreq) & 0xFF;

	// First send the command word to PIT. Sets binary counting, read or load
	// LSB first then MSB, Channel 0
	dword_t ocw = (tbMode | PIT_OCW_RL_DATA | tbCounter) & 0xFF;
	SendCommand(ocw);

	// Set frequency rate
	SendData(divisor, 0);
	SendData((divisor >> 8) & 0xFF, 0);

	// Reset tick count
	s_pit_ticks = 0;
}

void Pit::PrepareSleep(dword_t tdwSleepSpan) {

	if (tdwSleepSpan > 54000) {
		g_pLogger->Warn("[WRN][PIT::PrepareSleep] Invalid sleep span: %d, must "
						"not greater than 54ms\n");
		tdwSleepSpan = 54000;
	}

	// Silence the speaker
	auto speakerByte = inportb(0x61);
	// Set bit 1 to 0 (Enable speaker bit)
	speakerByte &= ~2;
	outportb(0x61, speakerByte);

	// Start counter
	dword_t ocw = PIT_OCW_RL_DATA | PIT_OCW_COUNTER_2 | PIT_OCW_RL_LATCH;
	SendCommand(ocw);

	s_SleepDivisor = PIT_FREQ / 1000000 / tdwSleepSpan;
}

void Pit::Sleep() {

	//	Send the sleep divisor
	SendData(PIT_OCW_COUNTER_2, s_SleepDivisor & 0xFF);
	SendData(PIT_OCW_COUNTER_2, s_SleepDivisor >> 8);

	// Reset and start
	auto pitByte = inportb(0x61);
	// Clear bit 0
	outportb(0x61, pitByte & ~1);
	// Set bit 1
	outportb(0x61, pitByte | 1);

	// Block until PIT counter reach 0
	while (!(inportb(0x61) & 0x20)) {}
}

}}}	   // namespace qkrnl::system::interrupts