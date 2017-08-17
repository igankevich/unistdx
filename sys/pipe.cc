#include "pipe.hh"

#include <sys/bits/safe_calls.hh>
#include <stdx/debug.hh>
#include <unistdx_config>
#include <sys/check>

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
	return out << stdx::make_object("in", rhs.out(), "out", rhs.in());
}

std::ostream&
sys::operator<<(std::ostream& out, const two_way_pipe& rhs) {
	return out << stdx::make_object("pipe1", rhs._pipe1, "pipe2", rhs._pipe2);
}

void
sys::two_way_pipe::validate() {
	if (this->is_owner()) {
		this->parent_in().validate();
		this->parent_out().validate();
	} else {
		this->child_in().validate();
		this->child_out().validate();
	}
}

void
sys::two_way_pipe::open() {
	this->_pipe1.open();
	this->_pipe2.open();
}

void
sys::two_way_pipe::close() {
	this->_pipe1.close();
	this->_pipe2.close();
}

void
sys::two_way_pipe::close_in_child() {
	this->_pipe1.in().close();
	this->_pipe2.out().close();
}

void
sys::two_way_pipe::close_in_parent() {
	this->_pipe1.out().close();
	this->_pipe2.in().close();
}

void
sys::two_way_pipe::remap_in_child(fd_type in, fd_type out) {
	this->child_in().remap(in);
	this->child_out().remap(out);
}

void
sys::two_way_pipe::close_unused() {
	this->is_owner() ? this->close_in_parent() : this->close_in_child();
}

