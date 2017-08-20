#include "file_mutex"
#include <cstring>

namespace {

	inline void
	check_fd(const sys::fildes& rhs) {
		if (!rhs) {
			throw sys::bad_file_lock();
		}
	}

	struct flock_wrapper: public ::flock {
		flock_wrapper(sys::file_lock_type type) {
			std::memset(this, 0, sizeof(struct ::flock));
			this->l_type = type;
			this->l_whence = SEEK_SET;
		}
	};


	template<sys::file_lock_type LockType>
	struct lock_type_to_mode;

	template<>
	struct lock_type_to_mode<sys::read_lock> {
		constexpr static const int value = O_RDONLY;
	};

	template<>
	struct lock_type_to_mode<sys::write_lock> {
		constexpr static const int value = O_WRONLY;
	};

}

template <sys::file_lock_type LockType>
void
sys::file_mutex<LockType>::open(const char* filename, mode_type mode) {
	typedef lock_type_to_mode<LockType> open_mode;
	this->_fd = ::open(
		filename,
		::sys::fildes::create | ::sys::fildes::close_on_exec | open_mode::value,
		mode
	);
	check_fd(*this);
}

template<sys::file_lock_type LockType>
void
sys::file_mutex<LockType>::lock() {
	check_fd(*this);
	this->call_fcntl(F_SETLKW);
}

template<sys::file_lock_type LockType>
void
sys::file_mutex<LockType>::unlock() {
	check_fd(*this);
	this->call_fcntl(F_UNLCK);
}

template<sys::file_lock_type LockType>
bool
sys::file_mutex<LockType>::try_lock() {
	check_fd(*this);
	flock_wrapper lk(LockType);
	int ret = ::fcntl(this->_fd, F_SETLK, &lk);
	bool result = true;
	if (ret == -1) {
		if (errno == EACCES || errno == EAGAIN) {
			result = false;
		} else {
			throw bad_file_lock();
		}
	}
	return result;
}

template <sys::file_lock_type LockType>
void
sys::file_mutex<LockType>::call_fcntl(int cmd) {
	flock_wrapper lk(LockType);
	int ret = 0;
	do {
		ret = ::fcntl(this->_fd, cmd, &lk);
	} while (ret == -1 && errno == EINTR);
	if (ret == -1) {
		throw bad_file_lock();
	}
}

template class sys::file_mutex<sys::file_lock_type::read_lock>;
template class sys::file_mutex<sys::file_lock_type::write_lock>;
