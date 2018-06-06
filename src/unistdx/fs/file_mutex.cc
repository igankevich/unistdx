#include "file_mutex"

#include <unistdx/base/check>

void
sys::file_mutex::open(
	const char* filename,
	open_flag flags,
	mode_type mode
) noexcept {
	this->_fd = ::open(
		filename,
		flags | open_flag::create | open_flag::close_on_exec,
		mode
	);
}

void
sys::file_mutex::lock(file_lock_type tp) {
	this->do_lock(int(tp));
}

void
sys::file_mutex::unlock() {
	this->do_lock(LOCK_UN);
}

bool
sys::file_mutex::try_lock(file_lock_type tp) {
	int ret = ::flock(this->_fd, int(tp) | LOCK_NB);
	bool result = true;
	if (ret == -1) {
		if (errno == EINTR || errno == EWOULDBLOCK) {
			result = false;
		} else {
			UNISTDX_THROW_BAD_CALL();
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
		UNISTDX_THROW_BAD_CALL();
	}
}
