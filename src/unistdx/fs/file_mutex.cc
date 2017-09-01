#include "file_mutex"
#include <cstring>

namespace {

	inline void
	check_fd(const sys::fildes& rhs) {
		if (!rhs) {
			throw sys::bad_file_lock();
		}
	}

}

void
sys::file_mutex::open(const char* filename, mode_type mode) noexcept {
	this->_fd = ::open(
		filename,
		::sys::fildes::create | ::sys::fildes::close_on_exec | O_RDWR,
		mode
	);
}

void
sys::file_mutex::lock(file_lock_type tp) {
	check_fd(*this);
	this->do_lock(int(tp));
}

void
sys::file_mutex::unlock() {
	check_fd(*this);
	this->do_lock(LOCK_UN);
}

bool
sys::file_mutex::try_lock(file_lock_type tp) {
	check_fd(*this);
	int ret = ::flock(this->_fd, int(tp) | LOCK_NB);
	bool result = true;
	if (ret == -1) {
		if (errno == EINTR || errno == EWOULDBLOCK) {
			result = false;
		} else {
			perror("try_lock");
			throw bad_file_lock();
		}
	}
	return result;
}

void
sys::file_mutex::do_lock(int cmd) {
	int ret = 0;
	do {
		ret = ::flock(this->_fd, cmd);
	} while (ret == -1 && errno == EINTR);
	if (ret == -1) {
		throw bad_file_lock();
	}
}
