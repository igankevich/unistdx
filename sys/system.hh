#ifndef SYS_SYSTEM_HH
#define SYS_SYSTEM_HH

#include <unistd.h>
#include <thread>
#include <cstdlib>

#if defined(PAGESIZE)
	#define UNISTDX_SYSCONF_PAGE_SIZE PAGESIZE
#elif defined(PAGE_SIZE)
	#define UNISTDX_SYSCONF_PAGE_SIZE PAGE_SIZE
#elif defined(_SC_PAGESIZE)
	#define UNISTDX_SYSCONF_PAGE_SIZE _SC_PAGESIZE
#elif defined(_SC_PAGE_SIZE)
	#define UNISTDX_SYSCONF_PAGE_SIZE _SC_PAGE_SIZE
#endif

/// POSIX and Linux system call wrappers.
namespace sys {

	typedef size_t size_type;

	size_type
	page_size() noexcept {
		#if defined(UNISTDX_SYSCONF_PAGE_SIZE)
		long result = ::sysconf(UNISTDX_SYSCONF_PAGE_SIZE);
		return result < 1 ? 4096 : result;
		#else
		return 4096;
		#endif
	}

	unsigned
	thread_concurrency() noexcept {
		#if defined(UNISTDX_SINGLE_THREAD)
		return 1u;
		#else
		unsigned concurrency = 0u;
		const char* cc = std::getenv("UNISTDX_CONCURRENCY");
		if (cc) {
			concurrency = std::atoi(cc);
		}
		if (concurrency <= 1u) {
			concurrency = std::thread::hardware_concurrency();
		}
		if (concurrency <= 1u) {
			concurrency = 1u;
		}
		return concurrency;
		#endif
	}

	unsigned
	io_concurrency() noexcept {
		return 1u;
	}

}

#endif // SYS_SYSTEM_HH
