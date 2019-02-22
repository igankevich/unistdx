#include <unistdx/net/ethernet_address>
#include <unistdx/test/operator>

TEST(ethernet_address, Calculus) {
	using namespace sys;
	EXPECT_EQ(ethernet_address{}, ethernet_address{});
	EXPECT_NE(
		ethernet_address{},
		ethernet_address(0xff,0xff,0xff,0xff,0xff,0xff)
	);
	EXPECT_EQ(
		ethernet_address(0xff,0xff,0xff,0xff,0xff,0xff),
		ethernet_address(0xff,0xff,0xff,0xff,0xff,0xff)
	);
}

typedef decltype(std::right) manipulator;

void
test_print(
	const char* expected,
	sys::ethernet_address addr,
	manipulator manip = std::left,
	int width=17
) {
	std::stringstream str;
	str << std::setw(width) << manip << addr;
	std::string result = str.str();
	EXPECT_EQ(width, result.size());
	EXPECT_EQ(expected, str.str());
}

TEST(ethernet_address, print_padding) {
	using namespace sys;
	test_print("00:00:00:00:00:00", ethernet_address{});
	test_print("ff:ff:ff:ff:ff:ff", ethernet_address{0xff,0xff,0xff,0xff,0xff,0xff});
	test_print(" 00:00:00:00:00:00", ethernet_address{}, std::right, 18);
	test_print("00:00:00:00:00:00 ", ethernet_address{}, std::left, 18);
}

TEST(ethernet_address, io) {
	using namespace sys;
	test::io_operators(ethernet_address{0xff,0xff,0xff,0xff,0xff,0xff});
}

