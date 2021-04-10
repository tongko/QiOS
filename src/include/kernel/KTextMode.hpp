#pragma once

#include <kernel/types.hpp>

namespace qkrnl {

enum class TextModeColor {
	Black = 0,
	Blue = 1,
	Green = 2,
	Cyna = 3,
	Red = 4,
	Magenta = 5,
	Brown = 6,
	Gray = 7,
	DarkGray = 8,
	LightBlue = 9,
	LightGreen = 10,
	LightCyna = 11,
	LightRed = 12,
	LightMagenta = 13,
	Yellow = 14,
	White = 15
};

//	Clear screen with current back and fore-gound color, default is gray on blue
void TextModeClearScreen();
//	Set foreground color
void TextModeSetForeColor(TextModeColor teFore);
//	Set background color
void TextModeSetBackColor(TextModeColor teBack);
//	Set cursor position
void TextModeSetPosition(byte_t tX, byte_t tY);
//	Put character to cursor position
void TextModePutChar(int_t tnCh, bool btUpdate = true);
//	Put string to cursor position
void TextModePutString(const char *tStr);
//	Set the text mode video buffer
void TextModeSetBuffer(__vma_t tNewBuffer);

}	 // namespace qkrnl

extern "C" {

//	Initialize 80x25 text mode.
void TextModeInit();
}