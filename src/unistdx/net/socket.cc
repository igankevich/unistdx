#include "socket"
#include <unistdx/base/make_object>
#include <unistdx/base/log_message>
#include <unistdx/bits/safe_calls>
#include <netinet/tcp.h>

namespace {

	int
	safe_socket(int domain, int type, int protocol) {
		using namespace sys::bits;
		global_lock_type lock(__forkmutex);
		int sock;
		UNISTDX_CHECK(sock = ::socket(domain, type, protocol));
		#if !defined(UNISTDX_HAVE_SOCK_NONBLOCK) || \
		!defined(UNISTDX_HAVE_SOCK_CLOEXEC)
		set_mandatory_flags(sock);
		#endif
		return sock;
	}

	inline std::string
	status_message(const sys::socket& rhs) {
		return rhs.error() == 0
		       ? "ok"
			   : std::make_error_code(std::errc(errno)).message();
	}

}

sys::socket::socket(const endpoint& bind_addr) {
	this->bind(bind_addr);
	this->listen();
}

sys::socket::socket(const endpoint& bind_addr, const endpoint& conn_addr) {
	this->bind(bind_addr);
	this->connect(conn_addr);
}

void
sys::socket::bind(const endpoint& e) {
	this->create_socket_if_necessary(e);
	this->setopt(reuse_addr);
	#ifndef NDEBUG
	log_message("sys", "binding to _", e);
	#endif
	UNISTDX_CHECK(::bind(this->_fd, e.sockaddr(), e.sockaddrlen()));
}

void
sys::socket::listen() {
	#ifndef NDEBUG
	log_message("sys", "listen on _", this->name());
	#endif
	UNISTDX_CHECK(::listen(this->_fd, SOMAXCONN));
}

void
sys::socket::connect(const endpoint& e) {
	this->create_socket_if_necessary(e);
	#ifndef NDEBUG
	log_message("sys", "connect to _", e);
	#endif
	UNISTDX_CHECK_IF_NOT(
		EINPROGRESS,
		::connect(this->_fd, e.sockaddr(), e.sockaddrlen())
	);
}

void
sys::socket::accept(socket& sock, endpoint& addr) {
	socklen_type len = sizeof(endpoint);
	sock.close();
	UNISTDX_CHECK(sock._fd = ::accept(this->_fd, addr.sockaddr(), &len));
	bits::set_mandatory_flags(sock._fd);
	#ifndef NDEBUG
	log_message("sys", "accept connection from _", addr);
	#endif
}

void
sys::socket::shutdown(shutdown_how how) {
	if (*this) {
		UNISTDX_CHECK_IF_NOT(ENOTCONN, ::shutdown(this->_fd, how));
	}
}

void
sys::socket::close() {
	this->shutdown(shut_read_write);
	this->sys::fildes::close();
}

void
sys::socket::setopt(option opt) {
	int one = 1;
	UNISTDX_CHECK(
		::setsockopt(this->_fd, SOL_SOCKET, opt, &one, sizeof(one))
	);
}

#if defined(UNISTDX_HAVE_TCP_USER_TIMEOUT)
void
sys::socket::set_user_timeout(const duration& d) {
	using namespace std::chrono;
	const unsigned int ms = duration_cast<milliseconds>(d).count();
	UNISTDX_CHECK(
		::setsockopt(this->_fd, IPPROTO_TCP, TCP_USER_TIMEOUT, &ms, sizeof(ms))
	);
}
#endif

int
sys::socket::error() const noexcept {
	int ret = 0;
	int opt = 0;
	if (!*this) {
		ret = -1;
	} else {
		try {
			socklen_type sz = sizeof(opt);
			UNISTDX_CHECK(
				::getsockopt(
					this->_fd,
					SOL_SOCKET,
					SO_ERROR,
					&opt,
					&sz
				)
			);
		} catch (...) {
			ret = -1;
		}
	}
	// ignore EAGAIN since it is common 'error' in asynchronous programming
	if (opt == EAGAIN || opt == EINPROGRESS) {
		ret = 0;
	} else {
		/*
		   If one connects to localhost to a different port and the service is
		   offline then socket's local port can be chosen to be the same as the
		   port of the service. If this happens the socket connects to itself
		   and sends and replies to its own messages (at least on Linux). This
		   conditional solves the issue.
		*/
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

std::ostream&
sys::operator<<(std::ostream& out, const socket& rhs) {
	return out
	       << make_object("fd", rhs._fd, "status", status_message(rhs));
}

void
sys::socket::create_socket_if_necessary(const endpoint& e) {
	if (!*this) {
		this->_fd = safe_socket(e.sa_family(), default_flags, 0);
	}
}
