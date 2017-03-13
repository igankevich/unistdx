#ifndef SYS_FILE_MUTEX_HH
#define SYS_FILE_MUTEX_HH

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdexcept>

#include "fildes.hh"

namespace sys {

	template<class LockType>
	class file_mutex: public fildes {

		struct flock_wrapper: public ::flock {
			flock_wrapper(short type) {
				std::memset(this, 0, sizeof(::flock));
				this->l_type = type;
				this->l_whence = SEEK_SET;
			}
		};

	public:
		inline void
		lock() {
			check_fd();
			call_fcntl(F_SETLKW);
		}

		inline void
		unlock() {
			check_fd();
			call_fcntl(F_UNLCK);
		}

		inline bool
		try_lock() {
			check_fd();
			flock_wrapper lk(LockType);
			int ret = ::fcntl(this->_fd, F_SETLK, &lk);
			bool result = true;
			if (ret == -1) {
				if (errno == EACCESS || errno == EAGAIN) {
					result = false;
				} else {
					throw std::runtime_error("bad file lock");
				}
			}
			return result;
		}

		file_mutex() noexcept = default;

		inline
		file_mutex(const char* filename, mode_type mode) noexcept:
		fildes()
	   	{ open(filename, mode); }

		void
		open(const char* filename, mode_type mode) {
			this->_fd = ::open(
				filename,
				::sys::fildes::create | ::sys::fildes::close_on_exec | O_RDWR,
				mode
			);
			check_fd();
		}

		// disallow copy & move operations
		file_mutex(const file_mutex&) = delete;
		file_mutex(file_mutex&&) = delete;
		file_mutex& operator=(const file_mutex&) = delete;
		file_mutex& operator=(file_mutex&&) = delete;

	private:
		inline void
		check_fd() {
			if (!*this) {
				throw std::runtime_error("bad file lock");
			}
		}

		inline void
		call_fcntl(int cmd) {
			flock_wrapper lk(LockType);
			int ret = 0;
			do {
				ret = ::fcntl(this->_fd, cmd, &lk);
			} while (ret == -1 && errno == EINTR);
			if (ret == -1) {
				throw std::runtime_error("bad file lock");
			}
		}

	};

}

#endif // SYS_FILE_MUTEX_HH
