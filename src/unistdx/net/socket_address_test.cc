/*
UNISTDX — C++ library for Linux system calls.
© 2018, 2019, 2020 Ivan Gankevich

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

#include <unistdx/config>

#if defined(UNISTDX_HAVE_LINUX_NETLINK_H)
#include <linux/rtnetlink.h>
#endif

#include <iostream>
#include <random>
#include <vector>

#include <unistdx/net/socket_address>

#include <unistdx/test/bstream_insert_extract>
#include <unistdx/test/make_types>

//void
//check_read(const char* str, sys::socket_address expected_result) {
    #define check_read(str, expected_result) { \
    sys::socket_address addr; \
    std::stringstream s; \
    s << str; \
    s >> addr; \
    EXPECT_EQ(expected_result, addr); \
}

#define EXPECT_UNIX_SOCKET_ADDRESS(str) { \
    sys::socket_address addr; \
    std::stringstream s; \
    s << str; \
    s >> addr; \
    EXPECT_EQ(sys::socket_address_family::unix, addr.family()); \
}

template <class T>
struct EndpointTest: public ::testing::Test {

    using traits_type = sys::ipaddr_traits<T>;
    using int_type = typename T::rep_type;
    typedef std::independent_bits_engine<
        std::random_device,
        8*sizeof(int_type),
        int_type> engine_type;

    sys::socket_address
    random_addr() {
        using sa = typename traits_type::socket_address_type;
        return sa{T{generator()}, 0};
    }

    template <class Iterator>
    void
    generate_random(Iterator first, Iterator last) {
        std::generate(
            first,
            last,
            std::bind(&EndpointTest<T>::random_addr, this)
        );
    }

    engine_type generator;
};

TYPED_TEST_CASE(EndpointTest, MAKE_TYPES(sys::ipv4_address, sys::ipv6_address));

TYPED_TEST(EndpointTest, WriteReadSingle) {
    sys::socket_address addr1 = this->random_addr();
    sys::socket_address addr2;
    std::stringstream s;
    s << addr1;
    s >> addr2;
    EXPECT_EQ(addr1, addr2);
}

TYPED_TEST(EndpointTest, WriteReadMultiple) {
    std::vector<sys::socket_address> addrs(10);
    this->generate_random(addrs.begin(), addrs.end());
    // write
    std::stringstream os;
    std::ostream_iterator<sys::socket_address> oit(os, "\n");
    std::copy(addrs.begin(), addrs.end(), oit);
    // read
    std::vector<sys::socket_address> addrs2;
    std::istream_iterator<sys::socket_address> iit(os), eos;
    std::copy(iit, eos, std::back_inserter(addrs2));
    EXPECT_EQ(addrs, addrs2);
}

TEST(EndpointIPv4Test, All) {
    EXPECT_EQ(
        sizeof(sys::sockinet4_type),
        sys::socket_address(sys::ipv4_socket_address{{127,0,0,1}, 0}).size()
    );
    // basic functionality
    check_read("0.0.0.0:0", sys::socket_address(sys::ipv4_socket_address{{0,0,0,0}, 0}));
    check_read("0.0.0.0:1234", sys::socket_address(sys::ipv4_socket_address{{0,0,0,0}, 1234}));
    check_read("0.0.0.0:65535", sys::socket_address(sys::ipv4_socket_address{{0,0,0,0}, 65535}));
    check_read("10.0.0.1:0", sys::socket_address(sys::ipv4_socket_address{{10,0,0,1}, 0}));
    check_read("255.0.0.1:0", sys::socket_address(sys::ipv4_socket_address{{255,0,0,1}, 0}));
    check_read(
        "255.255.255.255:65535",
        sys::socket_address(sys::ipv4_socket_address{{255,255,255,255}, 65535})
    );
    // out of range ports
    EXPECT_UNIX_SOCKET_ADDRESS("0.0.0.0:65536");
    EXPECT_UNIX_SOCKET_ADDRESS("0.0.0.1:65536");
    EXPECT_UNIX_SOCKET_ADDRESS("10.0.0.1:100000");
    // out of range addrs
    EXPECT_UNIX_SOCKET_ADDRESS("1000.0.0.1:0");
    // good spaces
    check_read(" 10.0.0.1:100", sys::socket_address(sys::ipv4_socket_address{{10,0,0,1}, 100}));
    check_read("10.0.0.1:100 ", sys::socket_address(sys::ipv4_socket_address{{10,0,0,1}, 100}));
    check_read(" 10.0.0.1:100 ", sys::socket_address(sys::ipv4_socket_address{{10,0,0,1}, 100}));
    // bad spaces
    EXPECT_UNIX_SOCKET_ADDRESS("10.0.0.1: 100");
    EXPECT_UNIX_SOCKET_ADDRESS("10.0.0.1 :100");
    EXPECT_UNIX_SOCKET_ADDRESS("10.0.0.1 : 100");
    EXPECT_UNIX_SOCKET_ADDRESS(" 10.0.0.1 : 100 ");
    // fancy addrs
    EXPECT_UNIX_SOCKET_ADDRESS("10:100");
    EXPECT_UNIX_SOCKET_ADDRESS("10.1:100");
    EXPECT_UNIX_SOCKET_ADDRESS("10.0.1:100");
    check_read("", sys::socket_address());
    EXPECT_UNIX_SOCKET_ADDRESS("anc:100");
    EXPECT_UNIX_SOCKET_ADDRESS(":100");
    EXPECT_UNIX_SOCKET_ADDRESS("10.0.0.0.1:100");
}

TEST(EndpointIPv6Test, All) {
    EXPECT_EQ(
        sizeof(sys::sockinet6_type),
        sys::socket_address(sys::ipv6_socket_address{{0x0,0,0,0,0,0,0,1}, 0}).size()
    );
    EXPECT_EQ(
        sys::socket_address(
            sys::socket_address(sys::ipv6_socket_address{{0x0,0,0,0,0,0,0,1}, 1234}),
            100
        ),
        sys::socket_address(sys::ipv6_socket_address{{0x0,0,0,0,0,0,0,1}, 100})
    );
    // basic functionality
    check_read("[::1]:0", sys::socket_address(sys::ipv6_socket_address{{0x0,0,0,0,0,0,0,1}, 0}));
    check_read("[1::1]:0", sys::socket_address(sys::ipv6_socket_address{{1,0,0,0,0,0,0,1}, 0}));
    check_read("[::]:0", sys::socket_address(sys::ipv6_socket_address{{0,0,0,0,0,0,0,0}, 0}));
    check_read(
        "[2001:1:0::123]:0",
        sys::socket_address(sys::ipv6_socket_address{{0x2001,1,0,0,0,0,0,0x123}, 0})
    );
    check_read(
        "[0:0:0:0:0:0:0:0]:0",
        sys::socket_address(sys::ipv6_socket_address{{0x0,0,0,0,0,0,0,0}, 0})
    );
    check_read(
        "[0:0:0:0:0:0:0:0]:1234",
        sys::socket_address(sys::ipv6_socket_address{{0x0,0,0,0,0,0,0,0}, 1234})
    );
    check_read(
        "[0:0:0:0:0:0:0:0]:65535",
        sys::socket_address(sys::ipv6_socket_address{{0x0,0,0,0,0,0,0,0}, 65535})
    );
    check_read(
        "[10:1:0:1:0:0:0:0]:0",
        sys::socket_address(sys::ipv6_socket_address{{0x10,1,0,1,0,0,0,0}, 0})
    );
    check_read(
        "[255:0:0:1:1:2:3:4]:0",
        sys::socket_address(sys::ipv6_socket_address{{0x255,0,0,1,1,2,3,4}, 0})
    );
    check_read(
        "[ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff]:65535",
        sys::socket_address(sys::ipv6_socket_address{{0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff}, 65535})
    );
    // out of range ports
    EXPECT_UNIX_SOCKET_ADDRESS("[::1]:65536");
    EXPECT_UNIX_SOCKET_ADDRESS("[::0]:65536");
    EXPECT_UNIX_SOCKET_ADDRESS("[::0]:100000");
    // out of range addrs
    EXPECT_UNIX_SOCKET_ADDRESS("[1ffff::1]:0");
    // good spaces
    check_read(" [10::1]:100", sys::socket_address(sys::ipv6_socket_address{{0x10,0,0,0,0,0,0,1}, 100}));
    check_read("[10::1]:100 ", sys::socket_address(sys::ipv6_socket_address{{0x10,0,0,0,0,0,0,1}, 100}));
    check_read(" [10::1]:100 ", sys::socket_address(sys::ipv6_socket_address{{0x10,0,0,0,0,0,0,1}, 100}));
    // bad spaces
    EXPECT_UNIX_SOCKET_ADDRESS("[10::1]: 100");
    EXPECT_UNIX_SOCKET_ADDRESS("[10::1 ]:100");
    EXPECT_UNIX_SOCKET_ADDRESS("[10::1 ]: 100");
    EXPECT_UNIX_SOCKET_ADDRESS(" [10::1 ]: 100 ");
    // fancy addrs
    EXPECT_UNIX_SOCKET_ADDRESS("[::1::1]:0");
    EXPECT_UNIX_SOCKET_ADDRESS("[:::]:0");
    EXPECT_UNIX_SOCKET_ADDRESS("[:]:0");
    EXPECT_UNIX_SOCKET_ADDRESS("[]:0");
    EXPECT_UNIX_SOCKET_ADDRESS("]:0");
    EXPECT_UNIX_SOCKET_ADDRESS("[:0");
    EXPECT_UNIX_SOCKET_ADDRESS("[10:0:0:0:0:0:0:0:1]:0");
    // IPv4 mapped addrs
    check_read(
        "[::ffff:127.1.2.3]:0",
        sys::socket_address(sys::ipv6_socket_address{{0,0,0,0xffff,0x127,1,2,3}, 0})
    );
}

TEST(Endpoint, OperatorBool) {
    // operator bool
    EXPECT_FALSE(bool(sys::socket_address()));
    EXPECT_TRUE(!sys::socket_address());
    // operator bool (IPv4)
    EXPECT_FALSE(bool(sys::socket_address("0.0.0.0:0")));
    EXPECT_TRUE(!sys::socket_address("0.0.0.0:0"));
    EXPECT_TRUE(bool(sys::socket_address("127.0.0.1:100")));
    EXPECT_FALSE(!sys::socket_address("127.0.0.1:100"));
    EXPECT_TRUE(bool(sys::socket_address("127.0.0.1:0")));
    EXPECT_FALSE(!sys::socket_address("127.0.0.1:0"));
    // operator bool (IPv6)
    EXPECT_FALSE(bool(sys::socket_address("[0:0:0:0:0:0:0:0]:0")));
    EXPECT_TRUE(!sys::socket_address("[0:0:0:0:0:0:0:0]:0"));
    EXPECT_FALSE(bool(sys::socket_address("[::]:0")));
    EXPECT_TRUE(!sys::socket_address("[::]:0"));
    EXPECT_TRUE(bool(sys::socket_address("[::1]:0")));
    EXPECT_FALSE(!sys::socket_address("[::1]:0"));
}

TEST(Endpoint, OperatorCopy) {
    EXPECT_EQ(
        sys::socket_address(sys::socket_address("10.0.0.1:1234"), 100),
        sys::socket_address("10.0.0.1:100")
    );
}

TEST(Endpoint, Literals) {
    using sys::ipv4_address;
    using sys::ipv6_address;
    constexpr ipv4_address any4;
    constexpr ipv6_address any6;
    sys::socket_address endpU(sys::ipv6_socket_address(ipv6_address(), 1234), 100);
    sys::socket_address endpV(sys::ipv6_socket_address(ipv6_address(), 100));
    EXPECT_EQ(endpU, endpV);
}

TEST(socket_address, unix_domain) {
    EXPECT_EQ(sys::socket_address_family::unix, sys::socket_address("/path").family());
    std::clog << sys::socket_address("/path/to/socket") << std::endl;
    std::clog << sys::socket_address("\0/path/to/socket") << std::endl;
    EXPECT_EQ(sys::socket_address("/path"), sys::socket_address("/path"));
    EXPECT_EQ(sys::socket_address("\0/path"), sys::socket_address("\0/path"));
    EXPECT_NE(sys::socket_address("\0/path"), sys::socket_address("/path"));
    EXPECT_NE(sys::socket_address("/path"), sys::socket_address("\0/path"));
    EXPECT_NE(sys::socket_address("/path"), sys::socket_address("\0/path"));
    EXPECT_FALSE(sys::socket_address("/path") < sys::socket_address("/path"));
    EXPECT_TRUE(sys::socket_address("/path"));
    EXPECT_TRUE(sys::socket_address("\0/path"));
    EXPECT_TRUE(sys::socket_address("\0"));
    EXPECT_FALSE(sys::socket_address());
}

struct socket_address_test: public ::testing::TestWithParam<sys::socket_address> {};

std::vector<sys::socket_address> all_addresses{
    sys::socket_address(),
    sys::socket_address(sys::ipv4_socket_address{{192,168,0,1},1000}),
    sys::socket_address(sys::ipv6_socket_address{{0,0,0,0,0,0,0,1},1000}),
};

TEST_P(socket_address_test, bstream_insert_extract) {
    test::bstream_insert_extract(GetParam());
}

INSTANTIATE_TEST_CASE_P(
    for_all_socket_addresses,
    socket_address_test,
    ::testing::ValuesIn(all_addresses)
);

#if defined(UNISTDX_HAVE_LINUX_NETLINK_H)
TEST(socket_address, netlink) {
    EXPECT_EQ(
        sys::socket_address_family::netlink,
        sys::netlink_socket_address(RTMGRP_IPV4_IFADDR).family()
    );
    EXPECT_EQ(
        sys::netlink_socket_address(RTMGRP_IPV4_IFADDR),
        sys::netlink_socket_address(RTMGRP_IPV4_IFADDR)
    );
    EXPECT_FALSE(
        sys::netlink_socket_address(RTMGRP_IPV4_IFADDR)
        <
        sys::netlink_socket_address(RTMGRP_IPV4_IFADDR)
    );
    EXPECT_NE(sys::netlink_socket_address(), sys::netlink_socket_address(RTMGRP_IPV4_IFADDR));
}
#endif

TEST(socket_address, bad_family) {
    sys::socket_address a;
    a.get()->sa_family = 1111;
    sys::socket_address b;
    b.get()->sa_family = 1111;
    EXPECT_FALSE(a < b);
    EXPECT_NE(a, b);
    EXPECT_EQ(0u, a.size());
    EXPECT_EQ(0u, b.size());
}

TEST(ipv4_socket_address, port) {
    using t = sys::ipaddr_traits<sys::ipv4_address>;
    sys::ipv4_socket_address sa{{127,0,0,1},1234};
    EXPECT_EQ(1234u, sa.port());
    EXPECT_EQ(1234u, t::port(sa));
}

TEST(ipv6_socket_address, port) {
    using t = sys::ipaddr_traits<sys::ipv6_address>;
    sys::ipv6_socket_address sa{{0,0,0,0,0,0,0,0},1234};
    EXPECT_EQ(1234u, sa.port());
    EXPECT_EQ(1234u, t::port(sa));
}
