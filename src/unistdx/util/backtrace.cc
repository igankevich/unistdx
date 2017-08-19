#include "backtrace"
#include "unistdx_config"
#include <unistd.h>
#if defined(UNISTDX_HAVE_BACKTRACE)
#include <execinfo.h>
#else
#endif
#if defined(UNISTDX_HAVE_CXXABI)
#include <cstring>
#include <cxxabi.h>
#include <string>
#endif


void
sys::backtrace(int fd) {
	#if defined(UNISTDX_HAVE_BACKTRACE)
	const size_t size = 4096 / sizeof(void*);
	void* buffer[size];
	const int nptrs = ::backtrace(buffer, size);
	bool success = false;
	#if defined(UNISTDX_HAVE_CXXABI)
	if (char** symbols = ::backtrace_symbols(buffer, nptrs)) {
		const char msg[] = "Backtrace:\n";
		(void)::write(fd, msg, sizeof(msg));
		for (int i=0; i<nptrs; ++i) {
			const char* name = symbols[i];
			int status = 0;
			char* buf = nullptr;
			try {
				// parse trace string
				std::string line(name);
				std::string::size_type pos0, pos1;
				pos0 = line.find_first_of('(');
				pos1 = line.find_last_of(')');
				pos1 = line.find_last_of('+', pos1);
				if (pos0 == std::string::npos || pos1 == std::string::npos) {
					throw 1;
				}
				std::string func = line.substr(pos0+1, pos1-pos0-1);
				buf = abi::__cxa_demangle(func.data(), 0, 0, &status);
				std::string s;
				s.push_back('\t');
				s.append(line.substr(0, pos0));
				s.push_back(' ');
				s.append(status == 0 ? buf : func.data());
				s.push_back('\n');
				(void)::write(fd, s.data(), s.size());
			} catch (...) {
				(void)::write(fd, "\t", 1);
				(void)::write(fd, name, std::strlen(name));
				(void)::write(fd, "\n", 1);
			}
			std::free(buf);
		}
		std::free(symbols);
		success = true;
	}
	#endif
	if (!success) {
		::backtrace_symbols_fd(buffer, nptrs, fd);
	}
	#else
	const char msg[] = "Backtrace: <none>\n";
	::write(fd, msg, sizeof(msg));
	#endif // if defined(UNISTDX_HAVE_BACKTRACE)
}
