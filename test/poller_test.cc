#include <stdx/debug.hh>

#include <stdx/iterator.hh>
#include <stdx/random.hh>

#include "test.hh"

template<class T>
void test_paired_iterator() {
	std::random_device rng;
	std::vector<T> x(20), y(20);
	std::vector<T> z(20), w(20);
	std::generate(x.begin(), x.end(), [&rng] () { return stdx::n_random_bytes<T>(rng); });
	auto beg1 = stdx::make_paired(x.begin(), y.begin());
	auto end1 = stdx::make_paired(x.end(), y.end());
	auto beg2 = stdx::make_paired(z.begin(), w.begin());
	std::copy(beg1, end1, beg2);
	test::compare(x, z, "x != z");
	test::compare(y, w, "y != w");
}

int main() {
	test_paired_iterator<uint32_t>();
	return 0;
}
