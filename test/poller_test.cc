#include <chrono>
#include <mutex>


#include <functional>
#include <random>

#include <stdx/debug.hh>
#include <sys/poller>

#include <gtest/gtest.h>

namespace std {

	const char*
	to_string(std::cv_status rhs) {
		switch (rhs) {
		case std::cv_status::timeout: return "timeout";
		case std::cv_status::no_timeout: return "no_timeout";
		default: return "<unknown>";
		}
	}

	std::ostream&
	operator<<(std::ostream& out, std::cv_status rhs) {
		return out << to_string(rhs);
	}

}

template<class T>
void
test_paired_iterator() {
	std::vector<T> x(20), y(20);
	std::vector<T> z(20), w(20);
	std::default_random_engine rng;
	std::uniform_int_distribution<T> dist;
	std::generate(x.begin(), x.end(), std::bind(dist, rng));
	auto beg1 = stdx::make_paired(x.begin(), y.begin());
	auto end1 = stdx::make_paired(x.end(), y.end());
	auto beg2 = stdx::make_paired(z.begin(), w.begin());
	std::copy(beg1, end1, beg2);
	EXPECT_EQ(x, z);
	EXPECT_EQ(y, w);
}

TEST(PairedIterator, All) {
	test_paired_iterator<uint32_t>();
}

TEST(EventPoller, WaitUntil) {
	sys::event_poller<void*> poller;
	std::mutex mtx;
	std::unique_lock<std::mutex> lock(mtx);
	typedef std::chrono::system_clock clock_type;
	EXPECT_EQ(
		poller.wait_until(lock, clock_type::now()),
		std::cv_status::timeout
	);
	EXPECT_EQ(
		poller.wait_until(
			lock,
			clock_type::time_point(clock_type::duration::zero())
		),
		std::cv_status::timeout
	);
}
