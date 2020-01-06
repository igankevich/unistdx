#include <unistdx/net/netlink_poller>
#include <unistdx/net/netlink_socket>
#include <unistdx/net/network_interface>

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
    for (auto& hdr : response) {
        auto m = hdr.sys::netlink_header::message<nlmsgerr>();
        if (hdr.error() && m->error != 0) {
            errno = -m->error;
            UNISTDX_THROW_BAD_CALL();
        }
    }
}
