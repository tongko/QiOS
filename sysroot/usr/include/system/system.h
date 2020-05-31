#pragma once

#include <stdint.h>

namespace System {

enum class COLOR {
	BLACK	   = 0,
	BLUE	   = 1,
	GREEN	   = 2,
	CYNA	   = 3,
	RED		   = 4,
	MAGENTA	   = 5,
	BROWN	   = 6,
	GRAY	   = 7,
	DARK_GRAY  = 8,
	LI_BLUE	   = 9,
	LI_GREEN   = 10,
	LI_CYNA	   = 11,
	LI_RED	   = 12,
	LI_MAGENTA = 13,
	YELLOW	   = 14,
	WHITE	   = 15
};

typedef struct {
	uint32_t x;
	uint32_t y;
} COORD, *PCOORD;

class ITerm {

public:
	// Attributes
	virtual COLOR GetForeColor() const	   = 0;
	virtual void  SetForeColor(COLOR fore) = 0;
	virtual COLOR GetBackColor() const	   = 0;
	virtual void  SetBackColor(COLOR back) = 0;

public:
	// Operations
	virtual void Print(const int8_t *) = 0;
	virtual void Clear()			   = 0;
	virtual void MoveTo(const PCOORD)  = 0;
};

static inline void outb(uint16_t port, uint8_t val) {
	asm volatile("outb %1, %0" : : "a"(val), "Nd"(port));
	/* There's an outb %al, $imm8  encoding, for compile-time constant port
	 * numbers that fit in 8b.  (N constraint). Wider immediate constants would
	 * be truncated at assemble-time (e.g. "i" constraint). The  outb  %al, %dx
	 * encoding is the only option for all other cases. %1 expands to %dx
	 * because  port  is a uint16_t.  %w1 could be used if we had the port
	 * number a wider C type */
}

static inline uint8_t inb(uint16_t port) {
	uint8_t ret;
	asm volatile("inb %0, %1" : "=a"(ret) : "Nd"(port));
	return ret;
}

}	 // namespace System