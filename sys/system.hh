#ifndef SYS_SYSTEM_HH
#define SYS_SYSTEM_HH

#include <unistd.h>

/// POSIX and Linux system call wrappers.
namespace sys {

	typedef size_t size_type;

	inline size_type
	page_size() noexcept {
		#if defined(PAGESIZE)
			#define UNISTDX_SYSCONF_PAGE_SIZE PAGESIZE
		#elif defined(PAGE_SIZE)
			#define UNISTDX_SYSCONF_PAGE_SIZE PAGE_SIZE
		#elif defined(_SC_PAGESIZE)
			#define UNISTDX_SYSCONF_PAGE_SIZE _SC_PAGESIZE
		#elif defined(_SC_PAGE_SIZE)
			#define UNISTDX_SYSCONF_PAGE_SIZE _SC_PAGE_SIZE
		#endif
		#if defined(UNISTDX_SYSCONF_PAGE_SIZE)
		long result = ::sysconf(UNISTDX_SYSCONF_PAGE_SIZE);
		return result < 1 ? 4096 : result;
		#else
		return 4096;
		#endif
		#undef UNISTDX_SYSCONF_PAGE_SIZE
	}

	extern unsigned
	thread_concurrency() noexcept;

	inline unsigned
	io_concurrency() noexcept {
		return 1u;
	}

}

#endif // SYS_SYSTEM_HH
