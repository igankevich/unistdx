#include <unistdx/net/netlink_poller>
#include <unistdx/net/netlink_socket>
#include <unistdx/net/network_interface>

sys::network_interface::network_interface(int index) {
    this->_name.resize(max_name_size());
    network_interface_request req{};
    req.ifr_ifindex = index;
    this->_socket.call(fildes::operation::interface_get_name, req);
    constexpr auto n = sizeof(req.ifr_name)-1;
    if (req.ifr_name[n] != 0) { req.ifr_name[n] = 0; }
    this->_name = req.ifr_name;
    this->_name.resize(traits_type::length(this->_name.data()));
}

void sys::network_interface::flags(flag f) {
    network_interface_request req{};
    traits_type::copy(req.ifr_name, this->_name.data(), this->_name.size());
    req.ifr_flags = static_cast<short>(f);
    this->_socket.call(fildes::operation::interface_set_flags, req);
}

auto sys::network_interface::flags() const -> flag {
    network_interface_request req{};
    traits_type::copy(req.ifr_name, this->_name.data(), this->_name.size());
    this->_socket.call(fildes::operation::interface_get_flags, req);
    return static_cast<flag>(req.ifr_flags);
}

void sys::network_interface::setf(flag f) {
    network_interface_request req{};
    traits_type::copy(req.ifr_name, this->_name.data(), this->_name.size());
    this->_socket.call(fildes::operation::interface_get_flags, req);
    flags(static_cast<flag>(req.ifr_flags) | f);
}

void sys::network_interface::unsetf(flag f) {
    network_interface_request req{};
    traits_type::copy(req.ifr_name, this->_name.data(), this->_name.size());
    this->_socket.call(fildes::operation::interface_get_flags, req);
    flags(static_cast<flag>(req.ifr_flags) & (~f));
}

int sys::network_interface::index() const {
    network_interface_request req{};
    traits_type::copy(req.ifr_name, this->_name.data(), this->_name.size());
    this->_socket.call(fildes::operation::interface_get_index, req);
    return req.ifr_ifindex;
}

auto sys::network_interface::address() const -> interface_address_type {
    network_interface_request req{};
    traits_type::copy(req.ifr_name, this->_name.data(), this->_name.size());
    this->_socket.call(fildes::operation::interface_get_address, req);
    socket_address addr(req.ifr_addr);
    this->_socket.call(fildes::operation::interface_get_network_mask, req);
    socket_address netmask(req.ifr_netmask);
    return interface_address_type(addr.addr4(), netmask.addr4());
}

void sys::network_interface::address(const interface_address_type& addr) {
    network_interface_request req{};
    traits_type::copy(req.ifr_name, this->_name.data(), this->_name.size());
    req.ifr_addr = *socket_address{addr.address(),0}.sockaddr();
    this->_socket.call(fildes::operation::interface_set_address, req);
    req.ifr_netmask = *socket_address{addr.netmask(),0}.sockaddr();
    this->_socket.call(fildes::operation::interface_set_network_mask, req);
}

void
sys::network_interface::set_namespace(fd_type ns) {
    sys::ifinfo_request request;
    using f = sys::netlink_message_flags;
    request.header().type(sys::ifinfo_message_type::new_link);
    request.header().flags(f::request | f::acknowledge);
    request.message().family(sys::family_type::netlink);
    using a = sys::ifinfo_attribute;
    request.add(a::network_namespace_fd, &ns, sizeof(ns));
    request.add(a::interface_name, this->_name);
    sys::netlink_socket sock(sys::netlink_protocol::route);
    request.write(sock);
    sys::ifinfo_message_container response;
    response.read(sock);
    // TODO
    /*
    for (auto& hdr : response) {
        auto m = hdr.sys::netlink_header::message<nlmsgerr>();
        if (hdr.error() && m->error != 0) {
            errno = -m->error;
            UNISTDX_THROW_BAD_CALL();
        }
    }
    */
}
