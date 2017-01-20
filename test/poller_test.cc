#include <chrono>
#include <mutex>
#include <cassert>

#include <stdx/debug.hh>

#include <stdx/iterator.hh>
#include <stdx/random.hh>

#include <sys/event.hh>

#include "test.hh"

namespace std {

	const char* to_string(std::cv_status rhs) {
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

void
test_wait_until() {
	sys::event_poller<void*> poller;
	std::mutex mtx;
	std::unique_lock<std::mutex> lock(mtx);
	typedef std::chrono::system_clock clock_type;
	test::equal(
		poller.wait_until(lock, clock_type::now()),
		std::cv_status::timeout,
		"bad sys::event_poller::wait_until return value"
	);
	test::equal(
		poller.wait_until(lock, clock_type::time_point(clock_type::duration::zero())),
		std::cv_status::timeout,
		"bad sys::event_poller::wait_until return value"
	);
}

int main() {
	test_paired_iterator<uint32_t>();
	test_wait_until();
	return 0;
}
