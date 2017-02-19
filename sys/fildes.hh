#ifndef SYS_FILDES_HH
#define SYS_FILDES_HH

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <vector>

#include <sys/bits/check.hh>
#include <sys/bits/safe_calls.hh>

#include <stdx/streambuf.hh>

namespace sys {

	typedef ::mode_t mode_type;
	typedef int fd_type;
	typedef int flag_type;

	fd_type
	safe_open(const char* path, flag_type oflag, mode_type mode) {
		bits::global_lock_type lock(bits::__forkmutex);
		fd_type ret = ::open(path, oflag, mode);
		if (ret != -1) {
			bits::set_mandatory_flags(ret);
		}
		return ret;
	}

	struct fildes {

		enum flag: flag_type {
			non_blocking = O_NONBLOCK,
			append = O_APPEND,
			async = O_ASYNC,
			dsync = O_DSYNC,
			sync = O_SYNC,
			create = O_CREAT,
			truncate = O_TRUNC
		};

		enum fd_flag: flag_type {
			close_on_exec = FD_CLOEXEC
		};

		#ifdef F_SETNOSIGPIPE
		enum pipe_flag: flag_type {
			no_sigpipe = 1
		};
		#endif

		static const fd_type
		bad = -1;

		fildes() = default;
		fildes(const fildes&) = delete;
		fildes& operator=(const fildes&) = delete;

		explicit
		fildes(fd_type rhs) noexcept:
			_fd(rhs) {}

		fildes(fildes&& rhs) noexcept: _fd(rhs._fd) {
			rhs._fd = bad;
		}

		~fildes() {
			this->close();
		}

		fildes&
		operator=(fildes&& rhs) {
			std::swap(_fd, rhs._fd);
			return *this;
		}

		void close() {
			if (*this) {
				bits::check(::close(this->_fd),
					__FILE__, __LINE__, __func__);
				this->_fd = bad;
			}
		}

		ssize_t
		read(void* buf, size_t n) const noexcept {
			return ::read(this->_fd, buf, n);
		}

		ssize_t
		write(const void* buf, size_t n) const noexcept {
			return ::write(this->_fd, buf, n);
		}

		fd_type
		get_fd() const noexcept {
			return this->_fd;
		}

		flag_type
		flags() const {
			return get_flags(F_GETFL);
		}

		flag_type
		fd_flags() const {
			return get_flags(F_GETFD);
		}

		void
		setf(flag_type rhs) {
			set_flag(F_SETFL, get_flags(F_GETFL) | rhs);
		}

		void
		unsetf(flag_type rhs) {
			set_flag(F_SETFL, get_flags(F_GETFL) & ~rhs);
		}

		#ifdef F_SETNOSIGPIPE
		void
		setf(pipe_flag rhs) {
			set_flag(F_SETNOSIGPIPE, 1);
		}

		void
		unsetf(pipe_flag rhs) {
			set_flag(F_SETNOSIGPIPE, 0);
		}
		#endif

		void
		setf(fd_flag rhs) {
			set_flag(F_SETFD, rhs);
		}

		bool
		operator==(const fildes& rhs) const noexcept {
			return this->_fd == rhs._fd;
		}

		explicit
		operator bool() const noexcept {
			return this->_fd >= 0;
		}

		bool
		operator !() const noexcept {
			return !operator bool();
		}

		bool
		operator==(fd_type rhs) const noexcept {
			return _fd == rhs;
		}

		friend bool
		operator==(fd_type lhs, const fildes& rhs) noexcept {
			return rhs._fd == lhs;
		}

		friend std::ostream&
		operator<<(std::ostream& out, const fildes& rhs) {
			return out << "{fd=" << rhs._fd << '}';
		}

		void
		remap(fd_type new_fd) {
			fd_type ret_fd = bits::check(::dup2(_fd, new_fd),
				__FILE__, __LINE__, __func__);
			this->close();
			_fd = ret_fd;
		}

		void
		validate() {
			get_flags(F_GETFD);
		}

	private:

		flag_type
		get_flags(int which) const {
			return bits::check(::fcntl(this->_fd, which),
				__FILE__, __LINE__, __func__);
		}

		void
		set_flag(int which, flag_type val) {
			bits::check(::fcntl(this->_fd, which, val),
				__FILE__, __LINE__, __func__);
		}

	protected:
		fd_type _fd = bad;
	};

	static_assert(sizeof(fildes) == sizeof(fd_type), "bad fd size");

	template<class T>
	struct fildes_streambuf_traits {

		typedef void char_type;

		static std::streamsize
		write(T& sink, const char_type* s, std::streamsize n) {
			return sink.write(s, n);
		}

		static std::streamsize
		read(T& src, char_type* s, std::streamsize n) {
			return src.read(s, n);
		}

		static bool
		is_blocking(const T& rhs) {
			try {
				return !bool(rhs.flags() & T::non_blocking);
			} catch (sys::bits::bad_call& err) {
				return false;
			}
		}

		static std::streamsize
		in_avail(T& rhs) {
			int nread;
			if (::ioctl(rhs.get_fd(), FIONREAD, &nread) < 0) {
				nread = 0;
			}
			return nread;
		}

	};
}

namespace stdx {

	template<>
	struct streambuf_traits<sys::fildes>:
	public sys::fildes_streambuf_traits<sys::fildes>
	{};

}

#endif // SYS_FILDES_HH
