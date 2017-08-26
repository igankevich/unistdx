#include "fildes"
#include <unistdx/base/make_object>

void
sys::fildes::remap(fd_type new_fd) {
	fd_type ret_fd;
	UNISTDX_CHECK(ret_fd = ::dup2(_fd, new_fd));
	this->close();
	this->_fd = ret_fd;
}

