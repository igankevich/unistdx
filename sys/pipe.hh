#ifndef SYS_PIPE_HH
#define SYS_PIPE_HH

#include <sys/process.hh>

#include "fildes.hh"

namespace sys {

	union pipe {

		inline
		pipe(): _fds {} {
			this->open();
		}

		inline
		pipe(pipe&& rhs) noexcept:
		_fds {std::move(rhs._fds[0]), std::move(rhs._fds[1])}
		{}

		inline
		pipe(fd_type in, fd_type out) noexcept:
		_fds {sys::fildes(in), sys::fildes(out)}
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

		void open();

		void close();

		friend std::ostream&
		operator<<(std::ostream& out, const pipe& rhs);

	private:
		sys::fildes _fds[2] = {};
		fd_type _rawfds[2];

		static_assert(sizeof(_fds) == sizeof(_rawfds), "bad sys::fildes size");
	};

	std::ostream&
	operator<<(std::ostream& out, const pipe& rhs);

	struct two_way_pipe {

		inline
		two_way_pipe():
		_owner(sys::this_process::id())
		{}

		~two_way_pipe() = default;
		two_way_pipe(const two_way_pipe&) = delete;
		two_way_pipe(two_way_pipe&&) = default;
		two_way_pipe&
		operator=(two_way_pipe&) = delete;

		inline fildes&
		parent_in() noexcept {
			return this->_pipe1.in();
		}

		inline fildes&
		parent_out() noexcept {
			return this->_pipe2.out();
		}

		inline const fildes&
		parent_in() const noexcept {
			return this->_pipe1.in();
		}

		inline const fildes&
		parent_out() const noexcept {
			return this->_pipe2.out();
		}

		inline fildes&
		child_in() noexcept {
			return this->_pipe2.in();
		}

		inline fildes&
		child_out() noexcept {
			return this->_pipe1.out();
		}

		inline const fildes&
		child_in() const noexcept {
			return this->_pipe2.in();
		}

		inline const fildes&
		child_out() const noexcept {
			return this->_pipe1.out();
		}

		inline bool
		is_owner() const noexcept {
			return sys::this_process::id() == this->_owner;
		}

		void
		open();

		void
		close();

		void
		close_in_child();

		void
		close_in_parent();

		void
		close_unused();

		void
		remap_in_child(fd_type in, fd_type out);

		void
		validate();

		friend std::ostream&
		operator<<(std::ostream& out, const two_way_pipe& rhs);

	private:

		pipe _pipe1;
		pipe _pipe2;
		pid_type _owner;

	};

	std::ostream&
	operator<<(std::ostream& out, const two_way_pipe& rhs);

}

#endif // SYS_PIPE_HH
