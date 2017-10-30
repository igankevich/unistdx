#include "socket"

#include <netinet/tcp.h>

#include <unistdx/base/log_message>
#include <unistdx/base/make_object>
#include <unistdx/bits/safe_calls>

namespace {

	int
	safe_socket(int domain, int type, int protocol) {
		using namespace sys::bits;
		#if !defined(UNISTDX_HAVE_SOCK_NONBLOCK) || \
		!defined(UNISTDX_HAVE_SOCK_CLOEXEC)
		global_lock_type lock(__forkmutex);
		#endif
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

	template <class T>
	struct default_free {
		inline void operator()(T* ptr) {
			::free(ptr);
		}
	};

}

sys::socket::socket(const endpoint& bind_addr) {
	this->bind(bind_addr);
	this->listen();
}

sys::socket::socket(const endpoint& bind_addr, const endpoint& conn_addr) {
	this->bind(bind_addr);
	this->connect(conn_addr);
}

sys::socket::socket(family_type family, flag_type flags):
sys::fildes(safe_socket(int (family), flags, 0))
{}

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
	using namespace bits;
	socklen_type len = sizeof(endpoint);
	sock.close();
	#if defined(UNISTDX_HAVE_ACCEPT4) && \
	defined(UNISTDX_HAVE_SOCK_NONBLOCK) && \
	defined(UNISTDX_HAVE_SOCK_CLOEXEC)
	UNISTDX_CHECK(
		sock._fd =
			::accept4(
				this->_fd,
				addr.sockaddr(),
				&len,
				UNISTDX_SOCK_NONBLOCK | UNISTDX_SOCK_CLOEXEC
			)
	);
	#else
	global_lock_type lock(__forkmutex);
	UNISTDX_CHECK(sock._fd = ::accept(this->_fd, addr.sockaddr(), &len));
	set_mandatory_flags(sock._fd);
	#endif
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

#if defined(UNISTDX_HAVE_SCM_CREDENTIALS)
union user_credentials_message {
	sys::cmessage_header h;
	char bytes[CMSG_SPACE(sizeof(sys::user_credentials))];
};

void
sys::send_credentials(socket& sock, const void* data, size_t n) {
	message_header h;
	io_vector v {const_cast<void*>(data), n};
	h.msg_iov = &v;
	h.msg_iovlen = 1;
	h.msg_name = nullptr;
	h.msg_namelen = 0;
	h.msg_control = nullptr;
	h.msg_controllen = 0;
	sock.send(&h, 0);
}

void
sys::receive_credentials(
	socket& sock,
	message_header& h,
	void* data,
	size_t
	n
) {
	user_credentials_message m;
	m.h.cmsg_len = CMSG_LEN(sizeof(m));
	m.h.cmsg_level = SOL_SOCKET;
	m.h.cmsg_type = SCM_CREDENTIALS;
	io_vector v {data, n};
	h.msg_iov = &v;
	h.msg_iovlen = 1;
	h.msg_name = nullptr;
	h.msg_namelen = 0;
	h.msg_control = m.bytes;
	h.msg_controllen = sizeof(m.bytes);
	sock.receive(&h, 0);
}

#endif // if defined(UNISTDX_HAVE_SCM_CREDENTIALS)

#if defined(UNISTDX_HAVE_SO_PEERCRED)
sys::user_credentials
sys::socket::credentials() const {
	user_credentials uc {};
	socklen_type n = sizeof(uc);
	UNISTDX_CHECK(
		::getsockopt(this->_fd, SOL_SOCKET, SO_PEERCRED, &uc, &n)
	);
	return uc;
}

#endif

#if defined(UNISTDX_HAVE_SCM_RIGHTS)
union fds_message {
	sys::cmessage_header h;
	char bytes[CMSG_SPACE(64*sizeof(sys::fd_type))];
};

void
sys::socket::send_fds(const sys::fd_type* data, size_t n) {
	if (n > 64) {
		throw std::invalid_argument("too many fds to sent");
	}
	char dummy[1] = {0};
	const size_t size = n*sizeof(sys::fd_type);
	fds_message m;
	m.h.cmsg_len = CMSG_LEN(size);
	m.h.cmsg_level = SOL_SOCKET;
	m.h.cmsg_type = SCM_RIGHTS;
	message_header h;
	h.msg_control = m.bytes;
	h.msg_controllen = CMSG_SPACE(size);
	io_vector v {dummy, 1};
	h.msg_iov = &v;
	h.msg_iovlen = 1;
	h.msg_name = nullptr;
	h.msg_namelen = 0;
	sys::fd_type* fds = reinterpret_cast<sys::fd_type*>(CMSG_DATA(&m.h));
	std::copy_n(data, n, fds);
	UNISTDX_CHECK(this->send(h, 0));
}

void
sys::socket::receive_fds(sys::fd_type* data, size_t n) {
	if (n > 64) {
		throw std::invalid_argument("too many fds to receive");
	}
	const size_t size = n*sizeof(sys::fd_type);
	char dummy[1] = {0};
	io_vector v {dummy, 1};
	fds_message m;
	m.h.cmsg_len = CMSG_LEN(size);
	m.h.cmsg_level = 0;
	m.h.cmsg_type = 0;
	message_header h;
	h.msg_control = m.bytes;
	h.msg_controllen = CMSG_SPACE(size);
	h.msg_iov = &v;
	h.msg_iovlen = 1;
	h.msg_name = nullptr;
	h.msg_namelen = 0;
	UNISTDX_CHECK(this->receive(h))
	if (m.h.cmsg_level == SOL_SOCKET && m.h.cmsg_type == SCM_RIGHTS) {
		sys::fd_type* fds = reinterpret_cast<sys::fd_type*>(CMSG_DATA(&m.h));
		std::copy_n(fds, n, data);
	}
}
#endif
