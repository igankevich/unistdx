#include <unistdx/io/epoll_event>

#include <unistdx/base/make_object>

std::ostream&
sys::operator<<(std::ostream& out, const epoll_event& rhs) {
    const char ev[] = {
        (rhs.in() ? 'r' : '-'),
        (rhs.out() ? 'w' : '-'),
        (rhs.hup() ? 'c' : '-'),
        (rhs.err() ? 'e' : '-'),
        0
    };
    return out << make_object("fd", rhs.fd(), "ev", ev);
}
