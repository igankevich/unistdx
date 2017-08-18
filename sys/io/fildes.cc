#include "fildes"
#include <stdx/debug.hh>

void
sys::fildes::remap(fd_type new_fd) {
	fd_type ret_fd;
	UNISTDX_CHECK(ret_fd = ::dup2(_fd, new_fd));
	this->close();
	this->_fd = ret_fd;
}

std::ostream&
sys::operator<<(std::ostream& out, const fildes& rhs) {
	return out << stdx::make_object("fd", rhs._fd);
}
