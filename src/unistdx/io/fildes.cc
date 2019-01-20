#include "fildes"

#include <unistdx/base/make_object>

sys::fildes&
sys::fildes::operator=(const fildes& rhs) {
	if (*this) {
		this->_fd = ::dup2(rhs._fd, this->_fd);
	} else {
		this->_fd = ::dup(rhs._fd);
	}
	UNISTDX_CHECK(this->_fd);
	return *this;
}
