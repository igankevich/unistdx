/*
UNISTDX — C++ library for Linux system calls.
© 2020 Ivan Gankevich

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

#include <unistdx/net/netlink_poller>
#include <unistdx/net/veth_interface>

void sys::veth_interface::destroy() {
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
