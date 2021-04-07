#include <cdefs.h>
#include <kernel/KTextMode.hpp>
#include <kernel/qkernel.hpp>
#include <kernel/system/interrupts/interrupts.hpp>
#include <stdarg.h>
#include <stdio.hpp>
#include <string.hpp>

namespace qkrnl {

#define KP_MAX_LEN 0x1000

#define PANIC_TEXT            \
	"        _________\n"     \
	"        |.-----.|\n"     \
	"        ||x . x||\n"     \
	"        ||_.-._||\n"     \
	"        `--)-(--`\n"     \
	"       __[=== o]___\n"   \
	"      |:::::::::::|\\\n" \
	"      `-=========-`()\n" \
	"\n\tUh-oh! Something went terribly wrong!\n"

_NORETURN_ void KernelPanic(const char *fmt, ...) {

	system::interrupts::InterruptsDisable();

	char	buff[KP_MAX_LEN] = {0};
	va_list args;
	va_start(args, fmt);

	qklib::vsprintf(buff, fmt, args);
	va_end(args);

	const char *disclaimer
		= "We apologize, QiOS has encountered a problem and has been shut down\n\
to prevent damage to your computer. Any unsaved work might be lost.\n\
We are sorry for the inconvenience this might have caused.\n\n\
Please report the following information and restart your computer.\n\
The system has been halted.\n\n";

	TextModeSetBackColor(TextModeColor::Red);
	TextModeSetForeColor(TextModeColor::White);
	TextModeClearScreen();
	TextModePutString(PANIC_TEXT);
	TextModePutString(disclaimer);

	char buff1[KP_MAX_LEN] = {0};
	qklib::sprintf(buff1, "\n%s\n", buff);
	TextModePutString(buff1);

	_ASMV_("hlt");

	_CODE_UNREACHABLE_
}

}	 // namespace qkrnl