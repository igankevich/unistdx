#include "system.hh"
#include <cstdlib>
#include <thread>

unsigned
sys::thread_concurrency() noexcept {
	#if defined(UNISTDX_SINGLE_THREAD)
	return 1u;
	#else
	int concurrency = 0;
	const char* cc = std::getenv("UNISTDX_CONCURRENCY");
	if (cc) {
		concurrency = std::atoi(cc);
	}
	if (concurrency < 1) {
		concurrency = std::thread::hardware_concurrency();
	}
	if (concurrency < 1) {
		concurrency = 1;
	}
	return static_cast<unsigned>(concurrency);
	#endif
}
