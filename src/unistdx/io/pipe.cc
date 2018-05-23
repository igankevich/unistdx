#include "pipe"

#include <unistdx/base/check>
#include <unistdx/base/make_object>
#include <unistdx/bits/safe_calls>
#include <unistdx/config>

namespace {

	inline void
	safe_pipe(sys::fd_type fds[2]) {
		using namespace sys;
		#if defined(UNISTDX_HAVE_SETNOSIGPIPE) || \
			!defined(UNISTDX_HAVE_PIPE2)
		bits::global_lock_type lock(bits::__forkmutex);
		#endif
		#if defined(UNISTDX_HAVE_PIPE2)
		UNISTDX_CHECK(::pipe2(fds, O_CLOEXEC | O_NONBLOCK));
		#else
		UNISTDX_CHECK(::pipe(fds));
		bits::set_mandatory_flags(fds[0]);
		bits::set_mandatory_flags(fds[1]);
		#endif
		#if defined(UNISTDX_HAVE_SETNOSIGPIPE)
		UNISTDX_CHECK(::fcntl(fds[1], F_SETNOSIGPIPE, 1));
		#endif
	}

}

void
sys::pipe::open() {
	this->close();
	safe_pipe(this->_rawfds);
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
