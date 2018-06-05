#include <gtest/gtest.h>

#include <unistdx/net/bytes>

#include <unistdx/test/operator>

TEST(bytes, members) {
	const sys::bytes<sys::u32> b(0xff);
	EXPECT_EQ(sizeof(sys::u32), b.size());
	EXPECT_EQ(b.end() - b.begin(), b.size());
	EXPECT_EQ(0xff, b.value());
	sys::u32 v = b;
	EXPECT_EQ(0xff, v);
}

TEST(bytes, front_begin) {
	sys::bytes<sys::u32> b;
	for (int i=0; i<4; ++i) {
		b[i] = i+1;
	}
	EXPECT_EQ(1, b.front());
	EXPECT_EQ(4, b.back());
	sys::bytes<sys::u32> copy(b);
	b.to_network_format();
	if (sys::is_network_byte_order()) {
		EXPECT_EQ(copy, b);
	} else {
		EXPECT_NE(copy, b);
		EXPECT_EQ(4, b[0]);
		EXPECT_EQ(3, b[1]);
		EXPECT_EQ(2, b[2]);
		EXPECT_EQ(1, b[3]);
	}
	b.to_host_format();
	EXPECT_EQ(copy, b);
	test::stream_insert_equals("01 02 03 04", b);
}

TEST(bytes, constructors) {
	sys::bytes<sys::u64> a{};
	sys::bytes<sys::u64> b(0);
	EXPECT_EQ(a, b);
	char data[8] = {1, 2, 3, 4, 5, 6, 7, 8};
	sys::bytes<sys::u64> c(std::begin(data), std::end(data));
	EXPECT_EQ(1, c.front());
	EXPECT_EQ(8, c.back());
}
