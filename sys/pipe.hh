#ifndef SYS_PIPE_HH
#define SYS_PIPE_HH

#include <unistd.h>

#include <sys/check>
#include <sys/bits/safe_calls.hh>
#include <sys/process.hh>

#include "fildes.hh"
#include <unistdx_config>

namespace sys {

	namespace bits {

		fd_type
		safe_pipe(fd_type fds[2]) {
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

	union pipe {

		inline
		pipe(): _fds{} {
			open();
		}

		inline
		pipe(pipe&& rhs) noexcept:
			_fds{std::move(rhs._fds[0]), std::move(rhs._fds[1])}
		{}

		pipe(fd_type in, fd_type out) noexcept:
		_fds{sys::fildes(in), sys::fildes(out)}
		{}

		inline
		~pipe() {}

		inline fildes&
		in() noexcept {
			return this->_fds[0];
		}

		inline fildes&
		out() noexcept {
			return this->_fds[1];
		}

		inline const fildes&
		in() const noexcept {
			return this->_fds[0];
		}

		inline const fildes&
		out() const noexcept {
			return this->_fds[1];
		}

		void open() {
			this->close();
			bits::check(bits::safe_pipe(this->_rawfds),
				__FILE__, __LINE__, __func__);
		}

		void close() {
			in().close();
			out().close();
		}

		friend std::ostream&
		operator<<(std::ostream& out, const pipe& rhs) {
			return out << stdx::make_object("in", rhs.out(), "out", rhs.in());
		}

	private:
		sys::fildes _fds[2] = {};
		fd_type _rawfds[2];

		static_assert(sizeof(_fds) == sizeof(_rawfds), "bad sys::fildes size");
	};

	struct two_way_pipe {

		two_way_pipe():
		_owner(sys::this_process::id())
		{}
		~two_way_pipe() = default;
		two_way_pipe(const two_way_pipe&) = delete;
		two_way_pipe(two_way_pipe&&) = default;
		two_way_pipe& operator=(two_way_pipe&) = delete;

		fildes& parent_in() noexcept { return _pipe1.in(); }
		fildes& parent_out() noexcept { return _pipe2.out(); }
		const fildes& parent_in() const noexcept { return _pipe1.in(); }
		const fildes& parent_out() const noexcept { return _pipe2.out(); }

		fildes& child_in() noexcept { return _pipe2.in(); }
		fildes& child_out() noexcept { return _pipe1.out(); }
		const fildes& child_in() const noexcept { return _pipe2.in(); }
		const fildes& child_out() const noexcept { return _pipe1.out(); }

		void open() {
			_pipe1.open();
			_pipe2.open();
		}

		void close() {
			_pipe1.close();
			_pipe2.close();
		}

		void close_in_child() {
			_pipe1.in().close();
			_pipe2.out().close();
		}

		void close_in_parent() {
			_pipe1.out().close();
			_pipe2.in().close();
		}

		void remap_in_child(fd_type in, fd_type out) {
			child_in().remap(in);
			child_out().remap(out);
		}

		bool
		is_owner() const {
			return sys::this_process::id() == _owner;
		}

		void
		close_unused() {
			is_owner() ? close_in_parent() : close_in_child();
		}

		void
		validate() {
			if (is_owner()) {
				parent_in().validate();
				parent_out().validate();
			} else {
				child_in().validate();
				child_out().validate();
			}
		}

		friend std::ostream&
		operator<<(std::ostream& out, const two_way_pipe& rhs) {
			return out << stdx::make_object("pipe1", rhs._pipe1, "pipe2", rhs._pipe2);
		}

	private:

		pipe _pipe1;
		pipe _pipe2;
		pid_type _owner;

	};

}

#endif // SYS_PIPE_HH
