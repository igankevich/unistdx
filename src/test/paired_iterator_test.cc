#include <algorithm>
#include <functional>
#include <random>
#include <vector>

#include <unistdx/bits/paired_iterator>

#include <gtest/gtest.h>

TEST(PairedIteratorTest, Copy) {
	typedef uint32_t T;
	std::vector<T> x(20), y(20);
	std::vector<T> z(20), w(20);
	std::default_random_engine rng;
	std::uniform_int_distribution<T> dist;
	std::generate(x.begin(), x.end(), std::bind(dist, rng));
	auto beg1 = sys::bits::make_paired(x.begin(), y.begin());
	auto end1 = sys::bits::make_paired(x.end(), y.end());
	auto beg2 = sys::bits::make_paired(z.begin(), w.begin());
	std::copy(beg1, end1, beg2);
	EXPECT_EQ(x, z);
	EXPECT_EQ(y, w);
}
