#include <gtest/gtest.h>
#include <unistdx/net/ifaddr>
#include <sstream>

TEST(Ifaddr, LocalhostIPv4) {
	typedef sys::ifaddr<sys::ipv6_addr> ifaddr_type;
	typedef sys::ipaddr_traits<sys::ipv6_addr> traits_type;
	ifaddr_type ifa(traits_type::localhost(), traits_type::loopback_mask());
	std::stringstream str;
	str << ifa;
	EXPECT_EQ(str.str(), "0:0:0:0:0:0:0:1/128");
}
