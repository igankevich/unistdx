#ifndef TEST_IO_HH
#define TEST_IO_HH

#include <gtest/gtest.h>

namespace test {

	template<class T>
	void
	io_operators(const T& expected) {
		T actual;
		std::stringstream s;
		EXPECT_TRUE(s.good());
		s << expected;
		EXPECT_TRUE(s.good());
		s >> actual;
		EXPECT_FALSE(s.fail());
		EXPECT_EQ(expected, actual);
	}


}

#endif // TEST_IO_HH vim:filetype=cpp
