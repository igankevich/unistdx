#include <unistdx/config>

#if defined(UNISTDX_HAVE_NETLINK)
#include <linux/rtnetlink.h>
#endif

#include <iostream>
#include <random>
#include <vector>

#include <unistdx/net/endpoint>

#include <unistdx/test/bstream_insert_extract>
#include <unistdx/test/make_types>

void
check_read(const char* str, sys::endpoint expected_result) {
	sys::endpoint addr;
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

	sys::endpoint
	random_addr() {
		return sys::endpoint(T(generator()), 0);
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
	sys::endpoint addr1 = this->random_addr();
	sys::endpoint addr2;
	std::stringstream s;
	s << addr1;
	s >> addr2;
	EXPECT_EQ(addr1, addr2);
}

TYPED_TEST(EndpointTest, WriteReadMultiple) {
	std::vector<sys::endpoint> addrs(10);
	this->generate_random(addrs.begin(), addrs.end());
	// write
	std::stringstream os;
	std::ostream_iterator<sys::endpoint> oit(os, "\n");
	std::copy(addrs.begin(), addrs.end(), oit);
	// read
	std::vector<sys::endpoint> addrs2;
	std::istream_iterator<sys::endpoint> iit(os), eos;
	std::copy(iit, eos, std::back_inserter(addrs2));
	EXPECT_EQ(addrs, addrs2);
}

TEST(EndpointIPv4Test, All) {
	EXPECT_EQ(
		sizeof(sys::sockinet4_type),
		sys::endpoint({127,0,0,1}, 0).sockaddrlen()
	);
	// basic functionality
	check_read("0.0.0.0:0", sys::endpoint({0,0,0,0}, 0));
	check_read("0.0.0.0:1234", sys::endpoint({0,0,0,0}, 1234));
	check_read("0.0.0.0:65535", sys::endpoint({0,0,0,0}, 65535));
	check_read("10.0.0.1:0", sys::endpoint({10,0,0,1}, 0));
	check_read("255.0.0.1:0", sys::endpoint({255,0,0,1}, 0));
	check_read(
		"255.255.255.255:65535",
		sys::endpoint(
			{255,255,255,255},
			65535
		)
	);
	// out of range ports
	check_read("0.0.0.0:65536", sys::endpoint({0,0,0,0}, 0));
	check_read("0.0.0.1:65536", sys::endpoint({0,0,0,0}, 0));
	check_read("10.0.0.1:100000", sys::endpoint({0,0,0,0}, 0));
	// out of range addrs
	check_read("1000.0.0.1:0", sys::endpoint({0,0,0,0}, 0));
	// good spaces
	check_read(" 10.0.0.1:100", sys::endpoint({10,0,0,1}, 100));
	check_read("10.0.0.1:100 ", sys::endpoint({10,0,0,1}, 100));
	check_read(" 10.0.0.1:100 ", sys::endpoint({10,0,0,1}, 100));
	// bad spaces
	check_read("10.0.0.1: 100", sys::endpoint({0,0,0,0}, 0));
	check_read("10.0.0.1 :100", sys::endpoint({0,0,0,0}, 0));
	check_read("10.0.0.1 : 100", sys::endpoint({0,0,0,0}, 0));
	check_read(" 10.0.0.1 : 100 ", sys::endpoint({0,0,0,0}, 0));
	// fancy addrs
	check_read("10:100", sys::endpoint({0,0,0,0}, 0));
	check_read("10.1:100", sys::endpoint({0,0,0,0}, 0));
	check_read("10.0.1:100", sys::endpoint({0,0,0,0}, 0));
	check_read("", sys::endpoint({0,0,0,0}, 0));
	check_read("anc:100", sys::endpoint({0,0,0,0}, 0));
	check_read(":100", sys::endpoint({0,0,0,0}, 0));
	check_read("10.0.0.0.1:100", sys::endpoint({0,0,0,0}, 0));
}

TEST(EndpointIPv6Test, All) {
	EXPECT_EQ(
		sizeof(sys::sockinet6_type),
		sys::endpoint({0x0,0,0,0,0,0,0,1}, 0).sockaddrlen()
	);
	// basic functionality
	check_read("[::1]:0", sys::endpoint({0x0,0,0,0,0,0,0,1}, 0));
	check_read("[1::1]:0", sys::endpoint({0x1,0,0,0,0,0,0,1}, 0));
	check_read("[::]:0", sys::endpoint({0x0,0,0,0,0,0,0,0}, 0));
	check_read(
		"[2001:1:0::123]:0",
		sys::endpoint(
			{0x2001,1,0,0,0,0,0,0x123},
			0
		)
	);
	check_read("[0:0:0:0:0:0:0:0]:0", sys::endpoint({0x0,0,0,0,0,0,0,0}, 0));
	check_read(
		"[0:0:0:0:0:0:0:0]:1234",
		sys::endpoint(
			{0x0,0,0,0,0,0,0,0},
			1234
		)
	);
	check_read(
		"[0:0:0:0:0:0:0:0]:65535",
		sys::endpoint(
			{0x0,0,0,0,0,0,0,0},
			65535
		)
	);
	check_read("[10:1:0:1:0:0:0:0]:0", sys::endpoint({0x10,1,0,1,0,0,0,0}, 0));
	check_read(
		"[255:0:0:1:1:2:3:4]:0",
		sys::endpoint(
			{0x255,0,0,1,1,2,3,4},
			0
		)
	);
	check_read(
		"[ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff]:65535",
		sys::endpoint(
			{0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff},
			65535
		)
	);
	// out of range ports
	check_read("[::1]:65536", sys::endpoint({0,0,0,0,0,0,0,0}, 0));
	check_read("[::0]:65536", sys::endpoint({0,0,0,0,0,0,0,0}, 0));
	check_read("[::0]:100000", sys::endpoint({0,0,0,0,0,0,0,0}, 0));
	// out of range addrs
	check_read("[1ffff::1]:0", sys::endpoint({0,0,0,0,0,0,0,0}, 0));
	// good spaces
	check_read(" [10::1]:100", sys::endpoint({0x10,0,0,0,0,0,0,1}, 100));
	check_read("[10::1]:100 ", sys::endpoint({0x10,0,0,0,0,0,0,1}, 100));
	check_read(" [10::1]:100 ", sys::endpoint({0x10,0,0,0,0,0,0,1}, 100));
	// bad spaces
	check_read("[10::1]: 100", sys::endpoint({0,0,0,0,0,0,0,0}, 0));
	check_read("[10::1 ]:100", sys::endpoint({0,0,0,0,0,0,0,0}, 0));
	check_read("[10::1 ]: 100", sys::endpoint({0,0,0,0,0,0,0,0}, 0));
	check_read(" [10::1 ]: 100 ", sys::endpoint({0,0,0,0,0,0,0,0}, 0));
	// fancy addrs
	check_read("[::1::1]:0", sys::endpoint({0,0,0,0,0,0,0,0}, 0));
	check_read("[:::]:0", sys::endpoint({0,0,0,0,0,0,0,0}, 0));
	check_read("[:]:0", sys::endpoint({0,0,0,0,0,0,0,0}, 0));
	check_read("[]:0", sys::endpoint({0,0,0,0,0,0,0,0}, 0));
	check_read("]:0", sys::endpoint({0,0,0,0,0,0,0,0}, 0));
	check_read("[:0", sys::endpoint({0,0,0,0,0,0,0,0}, 0));
	check_read("[10:0:0:0:0:0:0:0:1]:0", sys::endpoint({0,0,0,0,0,0,0,0}, 0));
	// IPv4 mapped addrs
	check_read(
		"[::ffff:127.1.2.3]:0",
		sys::endpoint(
			{0,0,0,0xffff,0x127,1,2,3},
			0
		)
	);
}

TEST(Endpoint, OperatorBool) {
	// operator bool
	EXPECT_FALSE(bool(sys::endpoint()));
	EXPECT_TRUE(!sys::endpoint());
	// operator bool (IPv4)
	EXPECT_FALSE(bool(sys::endpoint("0.0.0.0", 0)));
	EXPECT_TRUE(!sys::endpoint("0.0.0.0", 0));
	EXPECT_TRUE(bool(sys::endpoint("127.0.0.1", 100)));
	EXPECT_FALSE(!sys::endpoint("127.0.0.1", 100));
	EXPECT_TRUE(bool(sys::endpoint("127.0.0.1", 0)));
	EXPECT_FALSE(!sys::endpoint("127.0.0.1", 0));
	// operator bool (IPv6)
	EXPECT_FALSE(bool(sys::endpoint("0:0:0:0:0:0:0:0", 0)));
	EXPECT_TRUE(!sys::endpoint("0:0:0:0:0:0:0:0", 0));
	EXPECT_FALSE(bool(sys::endpoint("::", 0)));
	EXPECT_TRUE(!sys::endpoint("::", 0));
	EXPECT_TRUE(bool(sys::endpoint("::1", 0)));
	EXPECT_FALSE(!sys::endpoint("::1", 0));
}

TEST(Endpoint, OperatorEquals) {
	EXPECT_EQ(sys::endpoint("::", 0), sys::endpoint());
	EXPECT_EQ(sys::endpoint("0:0:0:0:0:0:0:0", 0), sys::endpoint());
	EXPECT_EQ(sys::endpoint("0.0.0.0", 0), sys::endpoint());
	EXPECT_NE(sys::endpoint("::", 0), sys::endpoint("0.0.0.0", 0));
}

TEST(Endpoint, OperatorOrder) {
	EXPECT_LT(sys::endpoint("10.0.0.1", 0), sys::endpoint("10.0.0.2", 0));
	EXPECT_GE(sys::endpoint("10.0.0.2", 0), sys::endpoint("10.0.0.1", 0));
	EXPECT_LT(sys::endpoint("10::1", 0), sys::endpoint("10::2", 0));
	EXPECT_GE(sys::endpoint("10::2", 0), sys::endpoint("10::1", 0));
	EXPECT_LT(sys::endpoint("10.0.0.1", 0), sys::endpoint("10::1", 0));
}

TEST(Endpoint, OperatorCopy) {
	EXPECT_EQ(
		sys::endpoint(sys::endpoint("10.0.0.1", 1234), 100),
		sys::endpoint("10.0.0.1", 100)
	);
}

TEST(Endpoint, Literals) {
	using sys::ipv4_addr;
	using sys::ipv6_addr;
	constexpr ipv4_addr any4;
	constexpr ipv6_addr any6;
	constexpr sys::endpoint endpU(sys::endpoint(ipv6_addr(), 1234), 100);
	constexpr sys::endpoint endpV(ipv6_addr(), 100);
	EXPECT_EQ(endpU, endpV);
}

TEST(endpoint, unix_domain) {
	EXPECT_EQ(sys::family_type::unix, sys::endpoint("/path").family());
	std::clog << sys::endpoint("/path/to/socket") << std::endl;
	std::clog << sys::endpoint("\0/path/to/socket") << std::endl;
	EXPECT_EQ(sys::endpoint("/path"), sys::endpoint("/path"));
	EXPECT_EQ(sys::endpoint("\0/path"), sys::endpoint("\0/path"));
	EXPECT_NE(sys::endpoint("\0/path"), sys::endpoint("/path"));
	EXPECT_NE(sys::endpoint("/path"), sys::endpoint("\0/path"));
	EXPECT_NE(sys::endpoint("/path"), sys::endpoint("\0/path"));
	EXPECT_FALSE(sys::endpoint("/path") < sys::endpoint("/path"));
}

struct endpoint_test: public ::testing::TestWithParam<sys::endpoint> {};

std::vector<sys::endpoint> all_endpoints{
	sys::endpoint(),
	sys::endpoint({192,168,0,1},1000),
	sys::endpoint({0,0,0,0,0,0,0,1},1000),
};

TEST_P(endpoint_test, bstream_insert_extract) {
	test::bstream_insert_extract(GetParam());
}

INSTANTIATE_TEST_CASE_P(
	for_all_endpoints,
	endpoint_test,
	::testing::ValuesIn(all_endpoints)
);

#if defined(UNISTDX_HAVE_NETLINK)
TEST(endpoint, netlink) {
	EXPECT_EQ(
		sys::family_type::netlink,
		sys::endpoint(RTMGRP_IPV4_IFADDR).family()
	);
	EXPECT_EQ(
		sys::endpoint(RTMGRP_IPV4_IFADDR),
		sys::endpoint(RTMGRP_IPV4_IFADDR)
	);
	EXPECT_FALSE(
		sys::endpoint(RTMGRP_IPV4_IFADDR)
		<
		sys::endpoint(RTMGRP_IPV4_IFADDR)
	);
	EXPECT_NE(sys::endpoint(), sys::endpoint(RTMGRP_IPV4_IFADDR));
}
#endif

TEST(endpoint, bad_family) {
	sys::endpoint a;
	a.sockaddr()->sa_family = 1111;
	sys::endpoint b;
	b.sockaddr()->sa_family = 1111;
	EXPECT_FALSE(a < b);
	EXPECT_NE(a, b);
	EXPECT_EQ(0, a.sockaddrlen());
	EXPECT_EQ(0, b.sockaddrlen());
}
