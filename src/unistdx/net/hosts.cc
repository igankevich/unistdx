#include <unistdx/net/hosts>

sys::host_error_category sys::host_category;

std::string
sys::host_error_category::message(int ev) const noexcept {
    auto s = ::gai_strerror(ev);
    return s ? std::string(s) : std::string("unknown");
}

sys::string sys::host_name(const socket_address& address, host_name_flags flags) {
    string name(NI_MAXHOST);
    int ret = 0;
    while (true) {
        ret = ::getnameinfo(address.sockaddr(), address.sockaddrlen(),
                            &name[0], name.capacity(),
                            nullptr, 0, int(flags));
        if (ret != EAI_OVERFLOW) { break; }
        name.capacity(name.capacity()*2);
    }
    UNISTDX_GAI_CHECK(ret);
    return name;
}

sys::string sys::service_name(port_type port, host_name_flags flags) {
    socket_address address{{0,0,0,0},port};
    string name(NI_MAXSERV);
    int ret = 0;
    while (true) {
        ret = ::getnameinfo(address.sockaddr(), address.sockaddrlen(),
                            nullptr, 0,
                            &name[0], name.capacity(), int(flags));
        if (ret != EAI_OVERFLOW) { break; }
        name.capacity(name.capacity()*2);
    }
    UNISTDX_GAI_CHECK(ret);
    return name;
}
