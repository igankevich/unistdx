/*
UNISTDX — C++ library for Linux system calls.
© 2017, 2018, 2020 Ivan Gankevich

This file is part of UNISTDX.

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/

#include <unistdx/net/socket>

#include <unistdx/base/log_message>
#include <unistdx/base/make_object>
#include <unistdx/bits/safe_calls>
#include <unistdx/config>

namespace {

    constexpr const int default_flags =
        UNISTDX_SOCK_NONBLOCK | UNISTDX_SOCK_CLOEXEC;

    inline int
    safe_socket(int domain, int type, int protocol) {
        using namespace sys::bits;
        #if !defined(UNISTDX_HAVE_SOCK_NONBLOCK) || \
        !defined(UNISTDX_HAVE_SOCK_CLOEXEC)
        global_lock_type lock(fork_mutex);
        #endif
        int sock;
        UNISTDX_CHECK(sock = ::socket(domain, type, protocol));
        #if !defined(UNISTDX_HAVE_SOCK_NONBLOCK) || \
        !defined(UNISTDX_HAVE_SOCK_CLOEXEC)
        set_mandatory_flags(sock);
        #endif
        return sock;
    }

    inline int
    safe_accept(int server_fd, sys::socket_address& client_address) {
        using namespace sys::bits;
        sys::socklen_type len = sizeof(sys::socket_address);
        int fd = -1;
        #if defined(UNISTDX_HAVE_ACCEPT4) && \
            defined(UNISTDX_HAVE_SOCK_NONBLOCK) && \
            defined(UNISTDX_HAVE_SOCK_CLOEXEC)
        fd = ::accept4(server_fd, client_address.get(), &len,
                       UNISTDX_SOCK_NONBLOCK | UNISTDX_SOCK_CLOEXEC);
        #else
        global_lock_type lock(fork_mutex);
        fd = ::accept(server_fd, client_address.get(), &len);
        set_mandatory_flags(fd);
        #endif
        return fd;
    }

}

sys::socket::socket(const socket_address_view& bind_addr) {
    this->bind(bind_addr);
    this->listen();
}

sys::socket::socket(const socket_address_view& bind_addr, const socket_address_view& conn_addr) {
    this->bind(bind_addr);
    this->connect(conn_addr);
}

sys::socket::socket(family_type family, socket_type type, protocol_type proto):
sys::fildes(safe_socket(int (family), int(type)|default_flags, proto))
{}

void sys::socket::bind(const socket_address_view& e) {
    create_socket_if_necessary(e);
    set(options::reuse_address);
    UNISTDX_CHECK(::bind(this->_fd, e.data(), e.size()));
}

void
sys::socket::listen() {
    #ifndef NDEBUG
    log_message("sys", "listen on _", this->name());
    #endif
    UNISTDX_CHECK(::listen(this->_fd, SOMAXCONN));
}

void
sys::socket::connect(const socket_address_view& e) {
    this->create_socket_if_necessary(e);
    int ret = ::connect(this->_fd, e.data(), e.size());
    if (ret == -1 && errno != EINPROGRESS) {
        throw bad_call(__FILE__, __LINE__, __func__);
    }
}

bool
sys::socket::accept(socket& sock, socket_address& addr) {
    auto client_fd = safe_accept(this->_fd, addr);
    if (client_fd == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) { return false; }
        UNISTDX_THROW_BAD_CALL();
    }
    sock.close();
    sock._fd = client_fd;
    return true;
}

void
sys::socket::shutdown(shutdown_flag how) {
    if (*this) {
        int ret = ::shutdown(this->_fd, int(how));
        if (ret == -1 && errno != ENOTCONN && errno != ENOTSUP) {
            UNISTDX_THROW_BAD_CALL(); // LCOV_EXCL_LINE
        }
    }
}

void
sys::socket::close() {
    this->shutdown(shutdown_flag::read_write);
    this->sys::fildes::close();
}

void
sys::socket::setopt(option opt) {
    int one = 1;
    set(SOL_SOCKET, opt, one);
}

// LCOV_EXCL_START
int
sys::socket::error() const noexcept {
    int ret = 0;
    int opt = 0;
    if (!*this) {
        ret = -1;
    } else {
        try {
            opt = get<int>(options::error);
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
// LCOV_EXCL_STOP

std::ostream&
sys::operator<<(std::ostream& out, const socket& rhs) {
    return out
           << make_object("fd", rhs._fd, "status", rhs.status_message());
}

void sys::socket::create_socket_if_necessary(const socket_address_view& e) {
    if (!*this) {
        #if defined(UNISTDX_HAVE_LINUX_NETLINK_H)
        int type = e.family() == family_type::netlink ? SOCK_RAW : SOCK_STREAM;
        #else
        int type = SOCK_STREAM;
        #endif
        this->_fd = safe_socket(sa_family_type(e.family()), type | default_flags, 0);
    }
}

#if defined(UNISTDX_HAVE_SCM_RIGHTS)
namespace {
    union fds_message {
        sys::cmessage_header h;
        char bytes[CMSG_SPACE(64*sizeof(sys::fd_type))];
    };
}

void
sys::socket::send_fds(const sys::fd_type* data, size_t n) {
    if (n > 64) {
        throw std::invalid_argument("too many fds to send");
    }
    char dummy[1] = {0};
    const size_t size = n*sizeof(sys::fd_type);
    fds_message m{};
    m.h.cmsg_len = CMSG_LEN(size);
    m.h.cmsg_level = SOL_SOCKET;
    m.h.cmsg_type = SCM_RIGHTS;
    message_header h{};
    h.msg_control = m.bytes;
    h.msg_controllen = CMSG_SPACE(size);
    io_vector v{dummy, 1};
    h.msg_iov = &v;
    h.msg_iovlen = 1;
    h.msg_name = nullptr;
    h.msg_namelen = 0;
    sys::fd_type* fds = reinterpret_cast<sys::fd_type*>(CMSG_DATA(&m.h));
    std::copy_n(data, n, fds);
    this->send(h);
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
    this->receive(h);
    if (m.h.cmsg_level == SOL_SOCKET && m.h.cmsg_type == SCM_RIGHTS) {
        sys::fd_type* fds = reinterpret_cast<sys::fd_type*>(CMSG_DATA(&m.h));
        std::copy_n(fds, n, data);
    }
}
#endif
