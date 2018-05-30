#include <gtest/gtest.h>
#include <unistdx/net/ipv4_addr>

TEST(IPv4Addr, Calculus) {
	using sys::ipv4_addr;
	typedef ipv4_addr::rep_type rep_type;
	EXPECT_EQ(rep_type(1), ipv4_addr(127,0,0,1).position(ipv4_addr(255,0,0,0)));
	EXPECT_EQ(rep_type(5), ipv4_addr(127,0,0,5).position(ipv4_addr(255,0,0,0)));
	EXPECT_EQ(ipv4_addr(255,255,255,0).to_prefix(), sys::prefix_type(24));
	EXPECT_EQ(ipv4_addr(255,255,0,0).to_prefix(), sys::prefix_type(16));
	EXPECT_EQ(ipv4_addr(255,0,0,0).to_prefix(), sys::prefix_type(8));
}

