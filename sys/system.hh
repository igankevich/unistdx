#ifndef SYS_SYSTEM_HH
#define SYS_SYSTEM_HH

#include <unistd.h>
#include <thread>

#if defined(PAGESIZE)
	#define FACTORY_SYSCONF_PAGE_SIZE PAGESIZE
#elif defined(PAGE_SIZE)
	#define FACTORY_SYSCONF_PAGE_SIZE PAGE_SIZE
#elif defined(_SC_PAGESIZE)
	#define FACTORY_SYSCONF_PAGE_SIZE _SC_PAGESIZE
#elif defined(_SC_PAGE_SIZE)
	#define FACTORY_SYSCONF_PAGE_SIZE _SC_PAGE_SIZE
#endif

/// POSIX and Linux system call wrappers.
namespace sys {

	typedef size_t size_type;

	size_type
	page_size() noexcept {
		#if defined(FACTORY_SYSCONF_PAGE_SIZE)
		long result = ::sysconf(FACTORY_SYSCONF_PAGE_SIZE);
		return result < 1 ? 4096 : result;
		#else
		return 4096;
		#endif
	}

	unsigned
	thread_concurrency() noexcept {
		#if defined(FACTORY_SINGLE_THREAD)
		return 1u;
		#else
		return std::thread::hardware_concurrency();
		#endif
	}

	unsigned
	io_concurrency() noexcept {
		return 1u;
	}

}

#endif // SYS_SYSTEM_HH
