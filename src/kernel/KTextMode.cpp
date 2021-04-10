#include <cdefs.h>
#include <kernel/KTextMode.hpp>
#include <kernel/portio.hpp>
#include <string.hpp>

namespace qkrnl {

//	Default video mem address
static const dword_t VIDEO_MEM {0xB8000};
//	Video memory pointer
static volatile word_t *s_pVideoMem;
//	Cursor location
static word_t s_wPoint {0};
//	Color - default gray on black
static byte_t s_bColor {7};
//	Width
static byte_t s_wWidth {80};
//	Height
static byte_t s_wHeight {25};

static inline word_t MakeChar(char tCh) {

	return tCh | (s_bColor << 8);
}

static byte_t ColorToByte(TextModeColor teColor) {

	switch (teColor) {
	case TextModeColor::Blue: return 1;
	case TextModeColor::Green: return 2;
	case TextModeColor::Cyna: return 3;
	case TextModeColor::Red: return 4;
	case TextModeColor::Magenta: return 5;
	case TextModeColor::Brown: return 6;
	case TextModeColor::Gray: return 7;
	case TextModeColor::DarkGray: return 8;
	case TextModeColor::LightBlue: return 9;
	case TextModeColor::LightGreen: return 10;
	case TextModeColor::LightCyna: return 11;
	case TextModeColor::LightRed: return 12;
	case TextModeColor::LightMagenta: return 13;
	case TextModeColor::Yellow: return 14;
	case TextModeColor::White: return 15;
	default: return 0;
	}
}

static void Scroll() {

	word_t wBlank = MakeChar(' ');
	byte_t y = s_wPoint / s_wWidth;

	//	If y greater than s_wHeight, it means we need to scrool up
	if (y >= s_wHeight) {

		//	Move the current text chunk that makes up the screen
		//	back in the buffer by a line
		byte_t temp = y - s_wHeight + 1;
		qklib::memcpy((void *) s_pVideoMem,
					  (void *) (s_pVideoMem + temp * s_wWidth),
					  (s_wHeight - temp) * s_wWidth * 2);

		//	Finally, we set the last line to blank
		qklib::memsetw((void *) (s_pVideoMem + (s_wHeight - temp) * s_wWidth),
					   wBlank,
					   s_wWidth);
		//	Set y to s_wHeight - 1
		s_wPoint = (s_wHeight - 1) * s_wWidth;
	}
}

static void UpdateCursor() {

	outportb(0x3D4, 0x0F);
	outportb(0x3D5, (byte_t)(s_wPoint & 0xFF));
	outportb(0x3D4, 0x0E);
	outportb(0x3D5, (byte_t)(s_wPoint >> 8) & 0xFF);
}

void TextModeSetBuffer(__vma_t tNewBuffer) {

	s_pVideoMem = reinterpret_cast<word_t *>(tNewBuffer);
}

void TextModeSetForeColor(TextModeColor teFore) {

	s_bColor |= ColorToByte(teFore);
}

void TextModeSetBackColor(TextModeColor teBack) {

	s_bColor |= (ColorToByte(teBack) << 4);
}

void TextModeSetPosition(byte_t tX, byte_t tY) {

	if (tX > s_wWidth) {
		tX = s_wWidth;
	}
	if (tY > s_wHeight) {
		tY = s_wHeight;
	}

	s_wPoint = (tY * s_wWidth + tX);
}

void TextModePutChar(int_t tnCh, bool btUpdate /*=true*/) {

	if (tnCh < 0 || tnCh > 125) {
		return;
	}

	byte_t y = s_wPoint / s_wWidth;
	byte_t x = s_wPoint % s_wWidth;
	byte_t c = (byte_t) tnCh;

	if (c == 0x08) {
		if (x != 0) {
			x--;
		}
	}
	else if (c == '\r') {
		x = 0;
	}
	else if (c == '\n') {
		x = 0;
		y++;
	}
	else if (c == '\t') {
		x = (x + 8) & ~(8 - 1);
	}
	else if (c >= ' ') {
		s_pVideoMem[y * s_wWidth + x] = MakeChar(c);
		x++;
	}

	if (x >= s_wWidth) {

		x = 0;
		y++;
	}

	s_wPoint = y * s_wWidth + x;
	Scroll();
	if (btUpdate) {
		UpdateCursor();
	}
}

void TextModePutString(const char *tStr) {

	while (*tStr != '\0') {

		TextModePutChar(*tStr++, false);
	}

	UpdateCursor();
}

void TextModeClearScreen() {

	word_t w = MakeChar(' ');

	for (int_t i = 0; i < s_wHeight * s_wWidth; i++) {

		s_pVideoMem[i] = w;
	}

	s_wPoint = 0;	 //	reset to {0, 0}
}
}	 // namespace qkrnl

void TextModeInit() {

	qkrnl::s_pVideoMem = reinterpret_cast<volatile word_t *>(qkrnl::VIDEO_MEM);
	qkrnl::TextModeClearScreen();
}