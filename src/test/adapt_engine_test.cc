#include <unistdx/base/adapt_engine>
#include <random>
#include <algorithm>
#include <functional>
#include <gtest/gtest.h>

TEST(AdaptEngineTest, Generate) {
	typedef char T;
	std::random_device rng;
	sys::adapt_engine<std::random_device,T> engine(rng);
	std::basic_string<T> str;
	std::generate_n(std::back_inserter(str), 100, std::ref(engine));
	EXPECT_EQ(100, str.size());
}
