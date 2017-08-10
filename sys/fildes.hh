#ifndef SYS_FILDES_HH
#define SYS_FILDES_HH

#include <unistd.h>
#include <fcntl.h>
#include <unistdx_config>
#if defined(UNISTDX_HAVE_FIONREAD)
#include <sys/ioctl.h>
#endif

#include <sys/check>
#include <sys/bits/safe_calls.hh>

#include <stdx/streambuf.hh>

namespace sys {

	typedef ::mode_t mode_type;
	typedef int fd_type;
	typedef int flag_type;

	inline fd_type
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

		#if defined(UNISTDX_HAVE_SETNOSIGPIPE)
		enum pipe_flag: flag_type {
			no_sigpipe = 1
		};
		#endif

		static const fd_type
		bad = -1;

		inline fildes() = default;
		fildes(const fildes&) = delete;
		fildes& operator=(const fildes&) = delete;

		inline explicit
		fildes(fd_type rhs) noexcept:
		_fd(rhs)
		{}

		inline
		fildes(fildes&& rhs) noexcept: _fd(rhs._fd) {
			rhs._fd = bad;
		}

		inline
		~fildes() {
			this->close();
		}

		inline fildes&
		operator=(fildes&& rhs) {
			std::swap(_fd, rhs._fd);
			return *this;
		}

		inline void
		close() {
			if (*this) {
				bits::check(::close(this->_fd),
					__FILE__, __LINE__, __func__);
				this->_fd = bad;
			}
		}

		inline ssize_t
		read(void* buf, size_t n) const noexcept {
			return ::read(this->_fd, buf, n);
		}

		inline ssize_t
		write(const void* buf, size_t n) const noexcept {
			return ::write(this->_fd, buf, n);
		}

		inline fd_type
		get_fd() const noexcept {
			return this->_fd;
		}

		inline flag_type
		flags() const {
			return get_flags(F_GETFL);
		}

		inline flag_type
		fd_flags() const {
			return get_flags(F_GETFD);
		}

		inline void
		setf(flag_type rhs) {
			set_flag(F_SETFL, get_flags(F_GETFL) | rhs);
		}

		inline void
		unsetf(flag_type rhs) {
			set_flag(F_SETFL, get_flags(F_GETFL) & ~rhs);
		}

		#if defined(UNISTDX_HAVE_SETNOSIGPIPE)
		inline void
		setf(pipe_flag rhs) {
			set_flag(F_SETNOSIGPIPE, 1);
		}

		inline void
		unsetf(pipe_flag rhs) {
			set_flag(F_SETNOSIGPIPE, 0);
		}
		#endif

		inline void
		setf(fd_flag rhs) {
			set_flag(F_SETFD, rhs);
		}

		inline bool
		operator==(const fildes& rhs) const noexcept {
			return this->_fd == rhs._fd;
		}

		inline explicit
		operator bool() const noexcept {
			return this->_fd >= 0;
		}

		inline bool
		operator !() const noexcept {
			return !operator bool();
		}

		inline bool
		operator==(fd_type rhs) const noexcept {
			return _fd == rhs;
		}

		inline friend bool
		operator==(fd_type lhs, const fildes& rhs) noexcept {
			return rhs._fd == lhs;
		}

		inline friend std::ostream&
		operator<<(std::ostream& out, const fildes& rhs) {
			return out << "{fd=" << rhs._fd << '}';
		}

		inline void
		remap(fd_type new_fd) {
			fd_type ret_fd = bits::check(::dup2(_fd, new_fd),
				__FILE__, __LINE__, __func__);
			this->close();
			_fd = ret_fd;
		}

		inline void
		validate() {
			get_flags(F_GETFD);
		}

	private:

		inline flag_type
		get_flags(int which) const {
			return bits::check(::fcntl(this->_fd, which),
				__FILE__, __LINE__, __func__);
		}

		inline void
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

		inline static std::streamsize
		write(T& sink, const char_type* s, std::streamsize n) {
			return sink.write(s, n);
		}

		inline static std::streamsize
		read(T& src, char_type* s, std::streamsize n) {
			return src.read(s, n);
		}

		inline static bool
		is_blocking(const T& rhs) {
			try {
				return !bool(rhs.flags() & T::non_blocking);
			} catch (sys::bits::bad_call& err) {
				return false;
			}
		}

		inline static std::streamsize
		in_avail(T& rhs) {
			#if defined(UNISTDX_HAVE_FIONREAD)
			int nread;
			if (::ioctl(rhs.get_fd(), FIONREAD, &nread) < 0) {
				nread = 0;
			}
			return nread;
			#else
			return 0;
			#endif
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
