#pragma once

#include <kernel/types.hpp>

namespace qkrnl { namespace system { namespace interrupts {

//	Operational Command Bit masks
const dword_t PIT_OCW_MASK_BINCOUNT = 1;	  // 00000001
const dword_t PIT_OCW_MASK_MODE = 0xE;		  // 00001110
const dword_t PIT_OCW_MASK_RL = 0x30;		  // 00110000
const dword_t PIT_OCW_MASK_COUNTER = 0xC0;	  // 11000000

//	Operational Command control bits
//	Use when setting binary count mode
const dword_t PIT_OCW_BINCOUNT_BINARY = 0;	  // 0
const dword_t PIT_OCW_BINCOUNT_BCD = 1;		  // 1

//	Use when setting counter mode
const dword_t PIT_OCW_MODE_TERMINALCOUNT = 0;	   // 0000
const dword_t PIT_OCW_MODE_ONESHOT = 0x2;		   // 0010
const dword_t PIT_OCW_MODE_RATEGEN = 0x4;		   // 0100
const dword_t PIT_OCW_MODE_SQUAREWAVEGEN = 0x6;	   // 0110
const dword_t PIT_OCW_MODE_SOFTWARETRIG = 0x8;	   // 1000
const dword_t PIT_OCW_MODE_HARDWARETRIG = 0xA;	   // 1010

//	Use when setting data transfer
const dword_t PIT_OCW_RL_LATCH = 0;			// 000000
const dword_t PIT_OCW_RL_LSBONLY = 0x10;	// 010000
const dword_t PIT_OCW_RL_MSBONLY = 0x20;	// 100000
const dword_t PIT_OCW_RL_DATA = 0x30;		// 110000

//	Use when setting the counter we are working with
const dword_t PIT_OCW_COUNTER_0 = 0;	   // 00000000
const dword_t PIT_OCW_COUNTER_1 = 0x40;	   // 01000000
const dword_t PIT_OCW_COUNTER_2 = 0x80;	   // 10000000

class Pit {

public:	   // Attributes
	/**
	 * returns current tick count
	 */
	static dword_t GetTickCount();

	/**
	 * Sets new pit tick count and returns prev. value
	 */
	static dword_t SetTickCount(dword_t);

	/**
	 * Test if interface is initialized
	 */
	static bool GetIsInitialized() { return m_bInitialized; }

public:	   //	Operations
	/**
	 * send operational command to pit. Set up command by using the operational
	 * command bit masks and setting them with the control bits. Shouldn't need
	 * to use this outside the interface
	 */
	static void SendCommand(byte_t);
	/**
	 * write data byte_t to a counter
	 */
	static void SendData(word_t, byte_t);
	/**
	 * reads data from a counter
	 */
	static byte_t ReadData(word_t);
	/**
	 * starts a counter. Counter continues until another call to this routine
	 */
	static void StartCounter(dword_t, byte_t, byte_t);
	/**
	 * Prepare to sleep for number of microseconds specified by `tdwSleepSpan`
	 *
	 * @param tdwSleepSpan Number of microseconds to sleep for. Max value 54000
	 * micrseconds.
	 */
	static void PrepareSleep(dword_t tdwSleepSpan);
	/**
	 * Sleep for duration configure by `PrepareSleep` function. Can be executed
	 * as frequent as you like once configured.
	 */
	static void Sleep();

private:
	static bool m_bInitialized;

private:
	Pit() { m_bInitialized = false; };
};

}}}	   // namespace qkrnl::system::interrupts