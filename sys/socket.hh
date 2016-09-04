#ifndef SYS_SOCKET_HH
#define SYS_SOCKET_HH

#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#if !defined(SOCK_NONBLOCK)
	#define FACTORY_SOCK_NONBLOCK 0
#else
	#define FACTORY_SOCK_NONBLOCK SOCK_NONBLOCK
#endif
#if !defined(SOCK_CLOEXEC)
	#define FACTORY_SOCK_CLOEXEC 0
#else
	#define FACTORY_SOCK_CLOEXEC SOCK_CLOEXEC
#endif

#include <stdx/mutex.hh>

#include <sys/endpoint.hh>

#include "fildes.hh"


namespace sys {

	namespace bits {

		int
		safe_socket(int domain, int type, int protocol) {
			global_lock_type lock(__forkmutex);
			int sock = check(
				::socket(domain, type, protocol),
				__FILE__, __LINE__, __func__);
			#if !defined(SOCK_NONBLOCK) || !defined(SOCK_CLOEXEC)
			set_mandatory_flags(sock);
			#endif
			return sock;
		}

	}

	struct socket: public fildes {

		typedef int opt_type;

		enum option: opt_type {
			reuse_addr = SO_REUSEADDR,
			keep_alive = SO_KEEPALIVE
		};

		enum shutdown_how {
			shut_read = SHUT_RD,
			shut_write = SHUT_WR,
			shut_read_write = SHUT_RDWR
		};

		static const flag_type
		default_flags = SOCK_STREAM | FACTORY_SOCK_NONBLOCK | FACTORY_SOCK_CLOEXEC;

		socket() = default;
		socket(const socket&) = delete;
		socket& operator=(const socket&) = delete;

		explicit
		socket(socket&& rhs) noexcept:
			sys::fildes(std::move(rhs)) {}

		explicit
		socket(fildes&& rhs) noexcept:
			sys::fildes(std::move(rhs)) {}

		/// Bind on @bind_addr and listen.
		explicit
		socket(const endpoint& bind_addr) {
			this->bind(bind_addr);
			this->listen();
		}

		/// Bind on @bind_addr and connect to a server on @conn_addr.
		explicit
		socket(const endpoint& bind_addr, const endpoint& conn_addr) {
			this->bind(bind_addr);
			this->connect(conn_addr);
		}

		~socket() {
			this->close();
		}

		socket&
		operator=(socket&& rhs) {
			sys::fildes::operator=(std::move(static_cast<sys::fildes&&>(rhs)));
			return *this;
		}

		void
		create_socket_if_necessary() {
			if (!*this) {
				this->_fd = bits::safe_socket(AF_INET, default_flags, 0);
			}
		}

		void
		bind(const endpoint& e) {
			this->create_socket_if_necessary();
			this->setopt(reuse_addr);
			#ifndef NDEBUG
			stdx::debug_message("sys", "binding to _", e);
			#endif
			bits::check(::bind(this->_fd, e.sockaddr(), e.sockaddrlen()),
				__FILE__, __LINE__, __func__);
		}

		void
		listen() {
			#ifndef NDEBUG
			stdx::debug_message("sys", "listen on _", this->name());
			#endif
			bits::check(::listen(this->_fd, SOMAXCONN),
				__FILE__, __LINE__, __func__);
		}

		void
		connect(const endpoint& e) {
			this->create_socket_if_necessary();
			#ifndef NDEBUG
			stdx::debug_message("sys", "connect to _", e);
			#endif
			bits::check_if_not<std::errc::operation_in_progress>(
				::connect(this->_fd, e.sockaddr(), e.sockaddrlen()),
				__FILE__, __LINE__, __func__);
		}

		void
		accept(socket& sock, endpoint& addr) {
			socklen_type len = sizeof(endpoint);
			sock.close();
			sock._fd = bits::check(::accept(this->_fd, addr.sockaddr(), &len),
				__FILE__, __LINE__, __func__);
			bits::set_mandatory_flags(sock._fd);
			#ifndef NDEBUG
			stdx::debug_message("sys", "accept connection from _", addr);
			#endif
		}

		void
		shutdown(shutdown_how how) {
			if (*this) {
				bits::check_if_not<std::errc::not_connected>(
					::shutdown(this->_fd, how),
					__FILE__, __LINE__, __func__);
			}
		}

		void
		close() {
			this->shutdown(shut_read_write);
			this->sys::fildes::close();
		}

		void
		setopt(option opt) {
			int one = 1;
			bits::check(::setsockopt(this->_fd,
				SOL_SOCKET, opt, &one, sizeof(one)),
				__FILE__, __LINE__, __func__);
		}

		/// @deprecated We use event-based error notifications.
		int
		error() const {
			int ret = 0;
			int opt = 0;
			if (!*this) {
				ret = -1;
			} else {
				try {
					socklen_type sz = sizeof(opt);
					bits::check(::getsockopt(this->_fd, SOL_SOCKET, SO_ERROR, &opt, &sz),
						__FILE__, __LINE__, __func__);
				} catch (...) {
					ret = -1;
				}
			}
			// ignore EAGAIN since it is common 'error' in asynchronous programming
			if (opt == EAGAIN || opt == EINPROGRESS) {
				ret = 0;
			} else {
				// If one connects to localhost to a different port and the service is offline
				// then socket's local port can be chosen to be the same as the port of the service.
				// If this happens the socket connects to itself and sends and replies to
				// its own messages (at least on Linux). This conditional solves the issue.
				try {
					if (ret == 0 && this->name() == this->peer_name()) {
						ret = -1;
					}
				} catch (...) {
					ret = -1;
				}
			}
			return ret;
		}

		endpoint
		bind_addr() const {
			endpoint addr;
			socklen_type len = sizeof(endpoint);
			::getsockname(this->_fd, addr.sockaddr(), &len);
			return addr;
		}

		endpoint
		name() const {
			endpoint addr;
			socklen_type len = sizeof(endpoint);
			bits::check(::getsockname(this->_fd, addr.sockaddr(), &len),
				__FILE__, __LINE__, __func__);
			return addr;
		}

		endpoint
		peer_name() const {
			endpoint addr;
			socklen_type len = sizeof(endpoint);
			bits::check(::getpeername(this->_fd, addr.sockaddr(), &len),
				__FILE__, __LINE__, __func__);
			return addr;
		}

		fd_type
		fd() const noexcept {
			return this->_fd;
		}

		friend std::ostream&
		operator<<(std::ostream& out, const socket& rhs) {
			return out << stdx::make_object("fd", rhs._fd, "status", rhs.status_message());
		}

	protected:

		explicit
		socket(fd_type sock) noexcept:
		sys::fildes(sock) {}

	private:

		std::string
		status_message() const {
			return error() == 0 ? "ok" : std::make_error_code(std::errc(errno)).message();
		}

	};

}

namespace stdx {

	template<>
	struct streambuf_traits<sys::socket> {

		typedef void char_type;

		static std::streamsize
		write(sys::socket& sink, const char_type* s, std::streamsize n) {
			return sink.write(s, n);
		}

		static std::streamsize
		read(sys::socket& src, char_type* s, std::streamsize n) {
			return src.read(s, n);
		}

	};

}

#endif // SYS_SOCKET_HH
