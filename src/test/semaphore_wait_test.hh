#ifndef TEST_SEMAPHORE_WAIT_TEST_HH
#define TEST_SEMAPHORE_WAIT_TEST_HH

#include <algorithm>
#include <chrono>
#include <functional>
#include <gtest/gtest.h>
#include <mutex>
#include <random>
#include <thread>
#include <vector>
#include <condition_variable>

#include <unistdx/base/log_message>
#include <unistdx/ipc/process>

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

template <class Semaphore>
struct SemaphoreWaitTest: public ::testing::Test {

	typedef Semaphore semaphore_type;

	void
	test_wait_until() {
		semaphore_type sem;
		std::mutex mtx;
		std::unique_lock<std::mutex> lock(mtx);
		typedef std::chrono::system_clock clock_type;
		//std::clog << "wait1" << std::endl;
		EXPECT_EQ(
			sem.wait_until(lock, clock_type::now()),
			std::cv_status::timeout
		);
		//std::clog << "wait2" << std::endl;
		EXPECT_EQ(
			sem.wait_until(
				lock,
				clock_type::time_point(clock_type::duration::zero())
			),
			std::cv_status::timeout
		);
	}

	void
	test_producer_consumer_thread() {
		typedef int T;
		std::default_random_engine rng;
		std::uniform_int_distribution<T> dist('a', 'z');
		semaphore_type sem_read;
		semaphore_type sem_write;
		sem_read.notify_one();
		std::mutex mtx;
		const size_t ninputs = 10000;
		std::vector<size_t> buffer;
		size_t sum_consumer = 0;
		std::thread consumer([&] () {
			size_t cnt = 0;
			while (cnt < ninputs) {
				//sys::log_message("consumer", "cnt=_ lock", cnt);
				std::unique_lock<std::mutex> lock(mtx);
				//sys::log_message("consumer", "cnt=_ wait", cnt);
				sem_write.wait(lock, [&] () { return !buffer.empty(); });
				while (!buffer.empty()) {
					++cnt;
					sum_consumer += buffer.back();
					buffer.pop_back();
				}
				lock.unlock();
				sem_read.notify_one();
			}
		});
		size_t sum_producer = 0;
		size_t cnt = 0;
		while (cnt < ninputs) {
			//sys::log_message("producer", "cnt=_ lock", cnt);
			std::unique_lock<std::mutex> lock(mtx);
			//sys::log_message("producer", "cnt=_ wait", cnt);
			sem_read.wait(lock, [&] () { return buffer.empty(); });
			buffer.emplace_back(dist(rng));
			sum_producer += buffer.back();
			++cnt;
			lock.unlock();
			sem_write.notify_one();
		}
		consumer.join();
		EXPECT_EQ(sum_producer, sum_consumer);
	}

	void
	test_producer_consumer_process() {
		using namespace std::chrono;
		semaphore_type sem;
		sys::process child([&sem] () {
			std::cv_status ret = std::cv_status::timeout;
			try {
				semaphore_type sem_child(sem.id());
				std::mutex mtx;
				ret = sem_child.wait_for(mtx, milliseconds(100));
			} catch (const sys::bad_call& err) {
				std::cerr << err << std::endl;
			}
			return ret == std::cv_status::timeout ? EXIT_FAILURE : EXIT_SUCCESS;
		});
		using namespace std::this_thread;
		sem.notify_one();
		sys::proc_status status = child.wait();
		EXPECT_TRUE(status.exited());
		EXPECT_EQ(EXIT_SUCCESS, status.exit_code());
	}

};

#endif // TEST_SEMAPHORE_WAIT_TEST_HH vim:filetype=cpp
