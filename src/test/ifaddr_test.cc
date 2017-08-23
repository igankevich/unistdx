#include <gtest/gtest.h>
#include <unistdx/net/ifaddr>
#include <sstream>
#include <random>

#include "io.hh"
#include "make_types.hh"

std::default_random_engine rng;

template <class T>
T
genrandom() {
	std::uniform_int_distribution<T> dist(0, std::numeric_limits<T>::max());
	return dist(rng);
}

template <class Addr>
sys::ifaddr<Addr>
random_ifaddr() {
	typedef typename Addr::rep_type rep;
	return sys::ifaddr<Addr>(Addr(genrandom<rep>()), genrandom<sys::prefix_type>());
}

TEST(Ifaddr, LocalhostIPv6) {
	typedef sys::ifaddr<sys::ipv6_addr> ifaddr_type;
	typedef sys::ipaddr_traits<sys::ipv6_addr> traits_type;
	ifaddr_type ifa(traits_type::localhost(), traits_type::loopback_mask());
	std::stringstream str;
	str << ifa;
	EXPECT_EQ(str.str(), "0:0:0:0:0:0:0:1/128");
}

template <class Addr>
struct IfaddrTest: public ::testing::Test {};

TYPED_TEST_CASE(IfaddrTest, MAKE_TYPES(sys::ipv4_addr, sys::ipv6_addr));

TYPED_TEST(IfaddrTest, InputOutputOperatorsLocalHost) {
	typedef sys::ifaddr<TypeParam> ifaddr_type;
	typedef sys::ipaddr_traits<TypeParam> traits_type;
	ifaddr_type ifa(traits_type::localhost(), traits_type::loopback_mask());
	test::io_operators(ifa);
}

TYPED_TEST(IfaddrTest, InputOutputOperatorsRandom) {
	for (int i=0; i<100; ++i) {
		test::io_operators(random_ifaddr<TypeParam>());
	}
}

TYPED_TEST(IfaddrTest, Loopback) {
	typedef typename TypeParam::rep_type rep;
	typedef sys::ifaddr<TypeParam> ifaddr_type;
	typedef sys::ipaddr_traits<TypeParam> traits_type;
	EXPECT_TRUE(
		ifaddr_type(
			traits_type::localhost(),
			traits_type::loopback_mask()
		).is_loopback()
	);
	EXPECT_TRUE(
		ifaddr_type(
			TypeParam(genrandom<rep>()),
			traits_type::widearea_mask()
		).is_widearea()
	);
}
