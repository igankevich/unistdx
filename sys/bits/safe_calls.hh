#ifndef SYS_BITS_SAFE_CALLS_HH
#define SYS_BITS_SAFE_CALLS_HH

#include <fcntl.h>

#include <mutex>
#include <stdx/mutex.hh>

namespace sys {

	namespace bits {

		typedef stdx::spin_mutex global_mutex_type;
		typedef std::lock_guard<global_mutex_type> global_lock_type;

		extern global_mutex_type __forkmutex;

		inline void
		set_mandatory_flags(int fd) {
			::fcntl(fd, F_SETFD, O_CLOEXEC);
			::fcntl(fd, F_SETFL, O_NONBLOCK);
		}

	}

}

#endif // SYS_BITS_SAFE_CALLS_HH
