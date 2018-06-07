#include <gtest/gtest.h>

#include <iterator>
#include <string>
#include <vector>

#include <unistdx/it/cstring_iterator>

TEST(cstring_iterator, basic) {
	const char* args[] = {
		"first",
		"second",
		nullptr
	};
	sys::cstring_iterator<const char*> first(args), last;
	std::vector<std::string> actual;
	std::copy(first, last, std::back_inserter(actual));
	EXPECT_EQ(2, actual.size());
	EXPECT_EQ("first", actual[0]);
	EXPECT_EQ("second", actual[1]);
}
