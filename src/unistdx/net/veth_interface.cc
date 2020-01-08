#include <unistdx/net/netlink_poller>
#include <unistdx/net/veth_interface>

#include <iostream>

void sys::veth_interface::destroy() {
    if (!*this) { return; }
    sys::ifinfo_request request;
    using f = sys::netlink_message_flags;
    request.header().type(sys::ifinfo_message_type::delete_link);
    request.header().flags(f::request | f::acknowledge);
    request.message().family(sys::family_type::netlink);
    using a = sys::ifinfo_attribute;
    request.add(a::interface_name, this->_name);
    sys::netlink_socket sock(sys::netlink_protocol::route);
    request.write(sock);
    sys::ifinfo_message_container response;
    response.read(sock);
    for (auto& hdr : response) {
        auto m = hdr.sys::netlink_header::message<nlmsgerr>();
        if (hdr.error() && m->error != 0) {
            errno = -m->error;
            UNISTDX_THROW_BAD_CALL();
        }
    }
}

void sys::veth_interface::init() {
    sys::ifinfo_request request;
    using f = sys::netlink_message_flags;
    request.header().type(sys::ifinfo_message_type::new_link);
    request.header().flags(f::request | f::create | f::exclude | f::acknowledge);
    request.message().family(sys::family_type::netlink);
    using a = sys::ifinfo_attribute;
    request.add(a::interface_name, this->_name);
    auto link_info = request.begin(a::link_info);
    request.add(a::link_info_kind, "veth");
    auto link_info_data = request.begin(a::link_info_data);
    auto veth_info_peer = request.begin(a::veth_peer_info);
    request.skip(sizeof(sys::ifinfo_request::message_type));
    request.add(a::interface_name, this->_peer.name());
    request.end(veth_info_peer);
    request.end(link_info_data);
    request.end(link_info);
    sys::netlink_socket sock(sys::netlink_protocol::route);
    request.write(sock);
    sys::ifinfo_message_container response;
    response.read(sock);
    for (auto& hdr : response) {
        auto m = hdr.sys::netlink_header::message<nlmsgerr>();
        if (hdr.error() && m->error != 0) {
            errno = -m->error;
            UNISTDX_THROW_BAD_CALL();
        }
    }
}
