#include "QTerm.h"

#include <stdint.h>
#include <system/MemLib.h>

namespace System {

static const uintptr_t VIDEO_MEM  = 0xFFFFFFFF800B8000;
static const uint16_t  VGA_WIDTH  = 80;
static const uint16_t  VGA_HEIGHT = 25;

static void UpdateCursor(PCOORD pCoord) {
	uint16_t pos = pCoord->y * VGA_WIDTH + pCoord->x;

	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t)(pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

static uint16_t ToCharInfo(int8_t c, COLOR fore, COLOR back) {

	uint8_t b	   = static_cast<uint8_t>(back);
	uint8_t f	   = static_cast<uint8_t>(fore);
	uint8_t attrib = (b << 4) | (f & 0x0F);

	return c | (attrib << 8);
}

void QTerm::PutC(int8_t c, COLOR fore, COLOR back) {

	if (c == '\n') {
		m_pos.y++;
		m_pos.x = 0;
		return;
	}

	volatile uint16_t *where
		= (volatile uint16_t *) VIDEO_MEM + (m_pos.y * VGA_WIDTH + m_pos.x);
	*where = ToCharInfo(c, fore, back);

	m_pos.x++;
	if (m_pos.x >= VGA_WIDTH) {
		m_pos.x = 0;
		m_pos.y++;
	}

	if (m_pos.y >= VGA_HEIGHT) {

		// Scroll terminal
		volatile uint16_t *where = (volatile uint16_t *) VIDEO_MEM;
		volatile uint16_t *start
			= (volatile uint16_t *) VIDEO_MEM + (m_pos.y * VGA_WIDTH + m_pos.x);
		uint32_t len = 2 * 24 * VGA_WIDTH;

		CopyMemory((uintptr_t) start, (uintptr_t) where, len);
		//	Set last line to blank
		start = (volatile uint16_t *) VIDEO_MEM + (24 * VGA_WIDTH);
		SetMemoryWord((uintptr_t) start,
					  ToCharInfo(' ', m_fore, m_back),
					  VGA_WIDTH);
	}

	return;
}

void QTerm::Print(const int8_t *str) {

	int8_t *p = (int8_t *) str;
	while (*p) {
		PutC(*p++, m_fore, m_back);
	}
	UpdateCursor(&m_pos);
}

void QTerm::Clear() {

	m_pos.x		 = 0;
	m_pos.y		 = 0;
	uint32_t len = VGA_HEIGHT * VGA_WIDTH;

	volatile uint16_t *where = (volatile uint16_t *) VIDEO_MEM;

	//	Set lines to blank
	SetMemoryWord((uintptr_t) where, ToCharInfo(' ', m_fore, m_back), len);
	UpdateCursor(&m_pos);
}

void QTerm::MoveTo(const PCOORD newCoord) {

	if (newCoord->x >= VGA_WIDTH || newCoord->y >= VGA_HEIGHT)
		return;

	m_pos.x = newCoord->x;
	m_pos.y = newCoord->y;
}

}	 // namespace System