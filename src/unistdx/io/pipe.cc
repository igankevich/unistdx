#include "pipe"

#include <unistdx/bits/safe_calls>
#include <unistdx/base/make_object>
#include <unistdx/config>
#include <unistdx/check>

namespace {

	sys::fd_type
	safe_pipe(sys::fd_type fds[2]) {
		using namespace sys;
		bits::global_lock_type lock(bits::__forkmutex);
		int ret = ::pipe(fds);
		if (ret != -1) {
			bits::set_mandatory_flags(fds[0]);
			bits::set_mandatory_flags(fds[1]);
			#if defined(UNISTDX_HAVE_SETNOSIGPIPE)
			fcntl(fds[1], F_SETNOSIGPIPE, 1);
			#endif
		}
		return ret;
	}

}

void
sys::pipe::open() {
	this->close();
	UNISTDX_CHECK(safe_pipe(this->_rawfds));
}

void
sys::pipe::close() {
	this->in().close();
	this->out().close();
}

std::ostream&
sys::operator<<(std::ostream& out, const pipe& rhs) {
	return out << make_object("in", rhs.out(), "out", rhs.in());
}
