#include "system"
#include <cstdlib>
#include <thread>

namespace {

	inline sys::size_type
	get_size(int name) {
		long result = ::sysconf(name);
		return result > 0 ? result : 0;
	}

}

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
		concurrency = 1; // LCOV_EXCL_LINE
	}
	return static_cast<unsigned>(concurrency);
	#endif
}

sys::cache::cache() {
	const int names[4][3] = {
		{_SC_LEVEL1_DCACHE_SIZE, _SC_LEVEL1_DCACHE_LINESIZE, _SC_LEVEL1_DCACHE_ASSOC},
		{_SC_LEVEL2_CACHE_SIZE, _SC_LEVEL2_CACHE_LINESIZE, _SC_LEVEL2_CACHE_ASSOC},
		{_SC_LEVEL3_CACHE_SIZE, _SC_LEVEL3_CACHE_LINESIZE, _SC_LEVEL3_CACHE_ASSOC},
		{_SC_LEVEL4_CACHE_SIZE, _SC_LEVEL4_CACHE_LINESIZE, _SC_LEVEL4_CACHE_ASSOC},
	};
	int nlevels = 0;
	for (int i=0; i<4; ++i) {
		size_type s = get_size(names[i][0]);
		if (s == 0) {
			break;
		}
		this->_levels[i]._level = i + 1;
		this->_levels[i]._size = s;
		this->_levels[i]._linesize = get_size(names[i][1]);
		this->_levels[i]._assoc = get_size(names[i][2]);
		++nlevels;
	}
	this->_nlevels = nlevels;
}
