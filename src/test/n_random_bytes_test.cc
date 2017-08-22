#include <unistdx/base/n_random_bytes>
#include <random>
#include <gtest/gtest.h>

TEST(NRandomBytesTest, Generate) {
	typedef uint64_t T;
	std::random_device rng;
	T result = sys::n_random_bytes<T>(rng);
	EXPECT_NE(0, result);
}

