#ifndef TEST_BASIC_MUTEX_TEST_HH
#define TEST_BASIC_MUTEX_TEST_HH

#include <gtest/gtest.h>
#include <thread>

template <class T>
struct BasicMutexTest: public ::testing::Test {

	template <class Func>
	void
	run(Func func) {
		const unsigned max_threads = std::max(
			std::thread::hardware_concurrency(),
			2*this->_minthreads
		);
		for (unsigned j=_minthreads; j<=max_threads; ++j) {
			for (uint64_t i=0; i<=_maxpower; ++i) {
				func(j, uint64_t(1) << i);
			}
		}
	}

	const unsigned _minthreads = 2;
	const uint64_t _maxpower = 10;

};

#endif // vim:filetype=cpp
