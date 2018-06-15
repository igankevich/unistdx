#include <unistdx/config>

#if defined(UNISTDX_HAVE_NETLINK)
#include <linux/rtnetlink.h>
#endif

#include <iostream>
#include <random>
#include <vector>

#include <unistdx/net/socket_address>

#include <unistdx/test/bstream_insert_extract>
#include <unistdx/test/make_types>

void
check_read(const char* str, sys::socket_address expected_result) {
	sys::socket_address addr;
	std::stringstream s;
	s << str;
	s >> addr;
	EXPECT_EQ(expected_result, addr);
}

template <class T>
struct EndpointTest: public ::testing::Test {

	typedef typename T::rep_type int_type;
	typedef std::independent_bits_engine<
		std::random_device,
		8*sizeof(int_type),
		int_type> engine_type;

	sys::socket_address
	random_addr() {
		return sys::socket_address(T(generator()), 0);
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

TYPED_TEST_CASE(EndpointTest, MAKE_TYPES(sys::ipv4_addr, sys::ipv6_addr));

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
		sys::socket_address({127,0,0,1}, 0).sockaddrlen()
	);
	// basic functionality
	check_read("0.0.0.0:0", sys::socket_address({0,0,0,0}, 0));
	check_read("0.0.0.0:1234", sys::socket_address({0,0,0,0}, 1234));
	check_read("0.0.0.0:65535", sys::socket_address({0,0,0,0}, 65535));
	check_read("10.0.0.1:0", sys::socket_address({10,0,0,1}, 0));
	check_read("255.0.0.1:0", sys::socket_address({255,0,0,1}, 0));
	check_read(
		"255.255.255.255:65535",
		sys::socket_address(
			{255,255,255,255},
			65535
		)
	);
	// out of range ports
	check_read("0.0.0.0:65536", sys::socket_address({0,0,0,0}, 0));
	check_read("0.0.0.1:65536", sys::socket_address({0,0,0,0}, 0));
	check_read("10.0.0.1:100000", sys::socket_address({0,0,0,0}, 0));
	// out of range addrs
	check_read("1000.0.0.1:0", sys::socket_address({0,0,0,0}, 0));
	// good spaces
	check_read(" 10.0.0.1:100", sys::socket_address({10,0,0,1}, 100));
	check_read("10.0.0.1:100 ", sys::socket_address({10,0,0,1}, 100));
	check_read(" 10.0.0.1:100 ", sys::socket_address({10,0,0,1}, 100));
	// bad spaces
	check_read("10.0.0.1: 100", sys::socket_address({0,0,0,0}, 0));
	check_read("10.0.0.1 :100", sys::socket_address({0,0,0,0}, 0));
	check_read("10.0.0.1 : 100", sys::socket_address({0,0,0,0}, 0));
	check_read(" 10.0.0.1 : 100 ", sys::socket_address({0,0,0,0}, 0));
	// fancy addrs
	check_read("10:100", sys::socket_address({0,0,0,0}, 0));
	check_read("10.1:100", sys::socket_address({0,0,0,0}, 0));
	check_read("10.0.1:100", sys::socket_address({0,0,0,0}, 0));
	check_read("", sys::socket_address({0,0,0,0}, 0));
	check_read("anc:100", sys::socket_address({0,0,0,0}, 0));
	check_read(":100", sys::socket_address({0,0,0,0}, 0));
	check_read("10.0.0.0.1:100", sys::socket_address({0,0,0,0}, 0));
}

TEST(EndpointIPv6Test, All) {
	EXPECT_EQ(
		sizeof(sys::sockinet6_type),
		sys::socket_address({0x0,0,0,0,0,0,0,1}, 0).sockaddrlen()
	);
	EXPECT_EQ(
		sys::socket_address(sys::socket_address({0x0,0,0,0,0,0,0,1}, 1234), 100),
		sys::socket_address({0x0,0,0,0,0,0,0,1}, 100)
	);
	// basic functionality
	check_read("[::1]:0", sys::socket_address({0x0,0,0,0,0,0,0,1}, 0));
	check_read("[1::1]:0", sys::socket_address({0x1,0,0,0,0,0,0,1}, 0));
	check_read("[::]:0", sys::socket_address({0x0,0,0,0,0,0,0,0}, 0));
	check_read(
		"[2001:1:0::123]:0",
		sys::socket_address(
			{0x2001,1,0,0,0,0,0,0x123},
			0
		)
	);
	check_read("[0:0:0:0:0:0:0:0]:0", sys::socket_address({0x0,0,0,0,0,0,0,0}, 0));
	check_read(
		"[0:0:0:0:0:0:0:0]:1234",
		sys::socket_address(
			{0x0,0,0,0,0,0,0,0},
			1234
		)
	);
	check_read(
		"[0:0:0:0:0:0:0:0]:65535",
		sys::socket_address(
			{0x0,0,0,0,0,0,0,0},
			65535
		)
	);
	check_read("[10:1:0:1:0:0:0:0]:0", sys::socket_address({0x10,1,0,1,0,0,0,0}, 0));
	check_read(
		"[255:0:0:1:1:2:3:4]:0",
		sys::socket_address(
			{0x255,0,0,1,1,2,3,4},
			0
		)
	);
	check_read(
		"[ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff]:65535",
		sys::socket_address(
			{0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff},
			65535
		)
	);
	// out of range ports
	check_read("[::1]:65536", sys::socket_address({0,0,0,0,0,0,0,0}, 0));
	check_read("[::0]:65536", sys::socket_address({0,0,0,0,0,0,0,0}, 0));
	check_read("[::0]:100000", sys::socket_address({0,0,0,0,0,0,0,0}, 0));
	// out of range addrs
	check_read("[1ffff::1]:0", sys::socket_address({0,0,0,0,0,0,0,0}, 0));
	// good spaces
	check_read(" [10::1]:100", sys::socket_address({0x10,0,0,0,0,0,0,1}, 100));
	check_read("[10::1]:100 ", sys::socket_address({0x10,0,0,0,0,0,0,1}, 100));
	check_read(" [10::1]:100 ", sys::socket_address({0x10,0,0,0,0,0,0,1}, 100));
	// bad spaces
	check_read("[10::1]: 100", sys::socket_address({0,0,0,0,0,0,0,0}, 0));
	check_read("[10::1 ]:100", sys::socket_address({0,0,0,0,0,0,0,0}, 0));
	check_read("[10::1 ]: 100", sys::socket_address({0,0,0,0,0,0,0,0}, 0));
	check_read(" [10::1 ]: 100 ", sys::socket_address({0,0,0,0,0,0,0,0}, 0));
	// fancy addrs
	check_read("[::1::1]:0", sys::socket_address({0,0,0,0,0,0,0,0}, 0));
	check_read("[:::]:0", sys::socket_address({0,0,0,0,0,0,0,0}, 0));
	check_read("[:]:0", sys::socket_address({0,0,0,0,0,0,0,0}, 0));
	check_read("[]:0", sys::socket_address({0,0,0,0,0,0,0,0}, 0));
	check_read("]:0", sys::socket_address({0,0,0,0,0,0,0,0}, 0));
	check_read("[:0", sys::socket_address({0,0,0,0,0,0,0,0}, 0));
	check_read("[10:0:0:0:0:0:0:0:1]:0", sys::socket_address({0,0,0,0,0,0,0,0}, 0));
	// IPv4 mapped addrs
	check_read(
		"[::ffff:127.1.2.3]:0",
		sys::socket_address(
			{0,0,0,0xffff,0x127,1,2,3},
			0
		)
	);
}

TEST(Endpoint, OperatorBool) {
	// operator bool
	EXPECT_FALSE(bool(sys::socket_address()));
	EXPECT_TRUE(!sys::socket_address());
	// operator bool (IPv4)
	EXPECT_FALSE(bool(sys::socket_address("0.0.0.0", 0)));
	EXPECT_TRUE(!sys::socket_address("0.0.0.0", 0));
	EXPECT_TRUE(bool(sys::socket_address("127.0.0.1", 100)));
	EXPECT_FALSE(!sys::socket_address("127.0.0.1", 100));
	EXPECT_TRUE(bool(sys::socket_address("127.0.0.1", 0)));
	EXPECT_FALSE(!sys::socket_address("127.0.0.1", 0));
	// operator bool (IPv6)
	EXPECT_FALSE(bool(sys::socket_address("0:0:0:0:0:0:0:0", 0)));
	EXPECT_TRUE(!sys::socket_address("0:0:0:0:0:0:0:0", 0));
	EXPECT_FALSE(bool(sys::socket_address("::", 0)));
	EXPECT_TRUE(!sys::socket_address("::", 0));
	EXPECT_TRUE(bool(sys::socket_address("::1", 0)));
	EXPECT_FALSE(!sys::socket_address("::1", 0));
}

TEST(Endpoint, OperatorEquals) {
	EXPECT_EQ(sys::socket_address("::", 0), sys::socket_address());
	EXPECT_EQ(sys::socket_address("0:0:0:0:0:0:0:0", 0), sys::socket_address());
	EXPECT_EQ(sys::socket_address("0.0.0.0", 0), sys::socket_address());
	EXPECT_NE(sys::socket_address("::", 0), sys::socket_address("0.0.0.0", 0));
}

TEST(Endpoint, OperatorOrder) {
	EXPECT_LT(sys::socket_address("10.0.0.1", 0), sys::socket_address("10.0.0.2", 0));
	EXPECT_GE(sys::socket_address("10.0.0.2", 0), sys::socket_address("10.0.0.1", 0));
	EXPECT_LT(sys::socket_address("10::1", 0), sys::socket_address("10::2", 0));
	EXPECT_GE(sys::socket_address("10::2", 0), sys::socket_address("10::1", 0));
	EXPECT_LT(sys::socket_address("10.0.0.1", 0), sys::socket_address("10::1", 0));
}

TEST(Endpoint, OperatorCopy) {
	EXPECT_EQ(
		sys::socket_address(sys::socket_address("10.0.0.1", 1234), 100),
		sys::socket_address("10.0.0.1", 100)
	);
}

TEST(Endpoint, Literals) {
	using sys::ipv4_addr;
	using sys::ipv6_addr;
	constexpr ipv4_addr any4;
	constexpr ipv6_addr any6;
	constexpr sys::socket_address endpU(sys::socket_address(ipv6_addr(), 1234), 100);
	constexpr sys::socket_address endpV(ipv6_addr(), 100);
	EXPECT_EQ(endpU, endpV);
}

TEST(socket_address, unix_domain) {
	EXPECT_EQ(sys::family_type::unix, sys::socket_address("/path").family());
	std::clog << sys::socket_address("/path/to/socket") << std::endl;
	std::clog << sys::socket_address("\0/path/to/socket") << std::endl;
	EXPECT_EQ(sys::socket_address("/path"), sys::socket_address("/path"));
	EXPECT_EQ(sys::socket_address("\0/path"), sys::socket_address("\0/path"));
	EXPECT_NE(sys::socket_address("\0/path"), sys::socket_address("/path"));
	EXPECT_NE(sys::socket_address("/path"), sys::socket_address("\0/path"));
	EXPECT_NE(sys::socket_address("/path"), sys::socket_address("\0/path"));
	EXPECT_FALSE(sys::socket_address("/path") < sys::socket_address("/path"));
}

struct socket_address_test: public ::testing::TestWithParam<sys::socket_address> {};

std::vector<sys::socket_address> all_addresses{
	sys::socket_address(),
	sys::socket_address({192,168,0,1},1000),
	sys::socket_address({0,0,0,0,0,0,0,1},1000),
};

TEST_P(socket_address_test, bstream_insert_extract) {
	test::bstream_insert_extract(GetParam());
}

INSTANTIATE_TEST_CASE_P(
	for_all_socket_addresses,
	socket_address_test,
	::testing::ValuesIn(all_addresses)
);

#if defined(UNISTDX_HAVE_NETLINK)
TEST(socket_address, netlink) {
	EXPECT_EQ(
		sys::family_type::netlink,
		sys::socket_address(RTMGRP_IPV4_IFADDR).family()
	);
	EXPECT_EQ(
		sys::socket_address(RTMGRP_IPV4_IFADDR),
		sys::socket_address(RTMGRP_IPV4_IFADDR)
	);
	EXPECT_FALSE(
		sys::socket_address(RTMGRP_IPV4_IFADDR)
		<
		sys::socket_address(RTMGRP_IPV4_IFADDR)
	);
	EXPECT_NE(sys::socket_address(), sys::socket_address(RTMGRP_IPV4_IFADDR));
}
#endif

TEST(socket_address, bad_family) {
	sys::socket_address a;
	a.sockaddr()->sa_family = 1111;
	sys::socket_address b;
	b.sockaddr()->sa_family = 1111;
	EXPECT_FALSE(a < b);
	EXPECT_NE(a, b);
	EXPECT_EQ(0, a.sockaddrlen());
	EXPECT_EQ(0, b.sockaddrlen());
}
