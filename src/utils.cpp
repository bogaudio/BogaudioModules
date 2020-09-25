
#include "utils.hpp"
#include <cstdarg>

std::string bogaudio::format(const char* fmt, ...) {
	const int n = 1024;
	char buf[n];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, n, fmt, args);
	va_end(args);
	return buf;
}
