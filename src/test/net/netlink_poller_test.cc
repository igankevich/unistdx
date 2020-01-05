#include <mutex>

#include <unistdx/net/netlink_poller>

#include <unistdx/test/operator>

TEST(NetlinkPoller, First) {
    sys::socket_address endp(RTMGRP_IPV4_IFADDR);
    sys::socket sock(
        sys::family_type::netlink,
        sys::socket_type::raw,
        NETLINK_ROUTE
    );
    sock.bind(endp);
    sys::event_poller poller;
    poller.insert({sock.fd(), sys::event::in});
    std::mutex mtx;
    poller.wait_for(mtx, std::chrono::milliseconds(1));
    for (const sys::epoll_event& ev : poller) {
        std::clog << "ev=" << ev << std::endl;
        if (ev.fd() == sock.fd()) {
            sys::ifaddr_message_container cont;
            cont.read(sock);
            for (sys::ifaddr_message_header& hdr : cont) {
                std::string action;
                if (hdr.new_address()) {
                    action = "add";
                } else if (hdr.delete_address()) {
                    action = "del";
                }
                if (hdr.new_address() || hdr.delete_address()) {
                    sys::ifaddr_message* m = hdr.message();
                    sys::ipv4_address address;
                    for (auto& attr : m->attributes(cont.length())) {
                        if (attr.type() == sys::ifaddr_attribute::address) {
                            address = *attr.data<sys::ipv4_address>();
                        }
                    }
                    std::clog << action << ' ' << address << std::endl;
                }
            }
        }
    }
}

TEST(netlink, get_address) {
    sys::socket sock(
        sys::family_type::netlink,
        sys::socket_type::raw,
        NETLINK_ROUTE
    );
    sock.unsetf(sys::open_flag::non_blocking);
    union {
        struct {
            sys::ifaddr_message_header hdr;
            sys::ifaddr_message payload;
        };
        char bytes[NLMSG_LENGTH(sizeof(sys::ifaddr_message))];
    } req;
    req.hdr.flags(
        sys::netlink_message_flags::request |
        sys::netlink_message_flags::dump
    );
    req.hdr.type(sys::ifaddr_message_type::get_address);
    req.hdr.length(sizeof(req));
    req.payload.family(sys::family_type::inet);
    ssize_t n = sock.send(&req, sizeof(req));
    std::clog << "n=" << n << std::endl;
    std::clog << "sizeof(req)=" << sizeof(req) << std::endl;
    sys::ifaddr_message_container cont;
    cont.read(sock);
    for (sys::ifaddr_message_header& hdr : cont) {
        EXPECT_FALSE(hdr.delete_address());
        if (hdr.new_address()) {
            sys::ifaddr_message* m = hdr.message();
            sys::ipv4_address address;
            std::string name;
            for (auto& attr : m->attributes(cont.length())) {
                if (attr.type() == sys::ifaddr_attribute::address) {
                    address = *attr.data<sys::ipv4_address>();
                } else if (attr.type() == sys::ifaddr_attribute::interface_name) {
                    name = attr.data<char>();
                }
            }
            std::clog
                << std::setw(3) << m->index() << ' '
                << std::setw(10) << name << ' '
                << address << '/' << m->prefix()
                << std::endl;
        }
    }
}

TEST(ifaddr_attribute, print) {
    test::stream_insert_equals(
        "interface_address",
        sys::ifaddr_attribute::address
    );
    test::stream_insert_equals(
        "multicast_address",
        sys::ifaddr_attribute::multicast_address
    );
    test::stream_insert_equals(
        "anycast_address",
        sys::ifaddr_attribute::anycast_address
    );
    test::stream_insert_equals(
        "unknown",
        sys::ifaddr_attribute(1111)
    );
}

struct ifaddr_attribute_test:
    public ::testing::TestWithParam<sys::ifaddr_attribute> {};

std::vector<sys::ifaddr_attribute> all_attributes{
    sys::ifaddr_attribute::unspecified,
    sys::ifaddr_attribute::address,
    sys::ifaddr_attribute::local_address,
    sys::ifaddr_attribute::interface_name,
    sys::ifaddr_attribute::broadcast_address,
    sys::ifaddr_attribute::anycast_address,
    sys::ifaddr_attribute::address_info,
    sys::ifaddr_attribute::multicast_address,
    sys::ifaddr_attribute::flags,
};

TEST_P(ifaddr_attribute_test, print) {
    EXPECT_NE("unknown", test::stream_insert(GetParam()));
}

INSTANTIATE_TEST_CASE_P(
    for_all_ifaddr_attributes,
    ifaddr_attribute_test,
    ::testing::ValuesIn(all_attributes)
);
