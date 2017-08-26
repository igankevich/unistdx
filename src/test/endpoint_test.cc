#include <gtest/gtest.h>
#include <iostream>
#include <random>
#include <unistdx/base/n_random_bytes>
#include <unistdx/net/endpoint>

#include "make_types.hh"

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

	sys::endpoint
	random_addr() {
		return sys::endpoint(sys::n_random_bytes<T>(generator), 0);
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

	std::random_device generator;
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
