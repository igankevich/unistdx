#include <execinfo.h>

char**
backtrace_symbols(void *const *, int) noexcept {
	return nullptr;
}
