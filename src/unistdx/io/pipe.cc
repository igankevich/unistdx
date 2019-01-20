#include "pipe"

#include <unistdx/base/check>
#include <unistdx/base/make_object>
#include <unistdx/bits/safe_calls>
#include <unistdx/config>

namespace {

	inline void
	safe_pipe(sys::fd_type fds[2]) {
		using namespace sys;
		#if defined(UNISTDX_HAVE_F_SETNOSIGPIPE) || \
			!defined(UNISTDX_HAVE_PIPE2)
		bits::global_lock_type lock(bits::fork_mutex);
		#endif
		#if defined(UNISTDX_HAVE_PIPE2)
		UNISTDX_CHECK(::pipe2(reinterpret_cast<int*>(fds), O_CLOEXEC | O_NONBLOCK));
		#else
		UNISTDX_CHECK(::pipe(fds));
		bits::set_mandatory_flags(fds[0]);
		bits::set_mandatory_flags(fds[1]);
		#endif
		#if defined(UNISTDX_HAVE_F_SETNOSIGPIPE)
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

ssize_t
sys::splice::operator()(fildes& in, fildes& out, size_t n) {
	ssize_t ret;
	#if defined(UNISTDX_HAVE_SPLICE)
	ret = ::splice(in.fd(), nullptr, out.fd(), nullptr, n, this->_flags);
	#endif
	UNISTDX_CHECK_IO(ret);
	return ret;
}

