#ifndef TEST_COLOR_HH
#define TEST_COLOR_HH

#include <unistd.h>
#include <stdlib.h>

enum struct Color {
	RESET            = 0,
	FG_RED           = 31,
	FG_GREEN         = 32,
	FG_YELLOW        = 33,
	FG_BLUE          = 34,
	FG_MAGENTA       = 35,
	FG_CYAN          = 36,
	FG_LIGHT_GRAY    = 37,
	FG_DARK_GRAY     = 90,
	FG_LIGHT_RED     = 91,
	FG_LIGHT_GREEN   = 92,
	FG_LIGHT_YELLOW  = 93,
	FG_LIGHT_BLUE    = 94,
	FG_LIGHT_MAGENTA = 95,
	FG_LIGHT_CYAN    = 96,
	FG_WHITE         = 97,
	FG_DEFAULT       = 39,
	BG_RED           = 41,
	BG_GREEN         = 42,
	BG_BLUE          = 44,
	BG_DEFAULT       = 49
};

std::ostream&
operator<<(std::ostream& os, Color rhs) {
	if (!::isatty(STDOUT_FILENO)) return os;
    return os << "\033[" << static_cast<int>(rhs) << "m";
}

#endif // vim:filetype=cpp
