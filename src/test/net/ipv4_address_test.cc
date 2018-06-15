#include <unistdx/net/ipv4_address>
#include <unistdx/test/operator>

TEST(IPv4Addr, Calculus) {
	using sys::ipv4_address;
	typedef ipv4_address::rep_type rep_type;
	EXPECT_EQ(rep_type(1), ipv4_address(127,0,0,1).position(ipv4_address(255,0,0,0)));
	EXPECT_EQ(rep_type(5), ipv4_address(127,0,0,5).position(ipv4_address(255,0,0,0)));
	EXPECT_EQ(ipv4_address(255,255,255,0).to_prefix(), sys::prefix_type(24));
	EXPECT_EQ(ipv4_address(255,255,0,0).to_prefix(), sys::prefix_type(16));
	EXPECT_EQ(ipv4_address(255,0,0,0).to_prefix(), sys::prefix_type(8));
}

typedef decltype(std::right) manipulator;

void
test_print(const char* expected, sys::ipv4_address addr, manipulator manip, int width) {
	std::stringstream str;
	str << std::setw(width) << manip << addr;
	std::string result = str.str();
	EXPECT_EQ(width, result.size());
	EXPECT_EQ(expected, str.str());
}

TEST(ipv4_address, print_padding) {
	test_print(" 127.0.0.1", sys::ipv4_address{127,0,0,1}, std::right, 10);
	test_print("127.0.0.1 ", sys::ipv4_address{127,0,0,1}, std::left, 10);
}
