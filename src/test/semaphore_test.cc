#include <unistdx/base/make_object>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>
#include <cmath>
#include <numeric>
#include <algorithm>

#include <unistdx/base/delete_each>
#include <unistdx/ipc/semaphore>

#include <gtest/gtest.h>

#include "basic_mutex_test.hh"
#include "thread_mutex_test.hh"
#include "semaphore_wait_test.hh"
#include "make_types.hh"

struct posix_process_semaphore: public sys::posix_semaphore {
	inline
	posix_process_semaphore():
	sys::posix_semaphore(sys::semaphore_type::process)
	{}
};

struct posix_thread_semaphore: public sys::posix_semaphore {
	inline
	posix_thread_semaphore():
	sys::posix_semaphore(sys::semaphore_type::thread)
	{}
};

template <class T>
struct SemaphoreTest: public BasicMutexTest<T> {};

TYPED_TEST_CASE(
	SemaphoreTest,
	MAKE_TYPES(
		std::condition_variable
		#if defined(UNISTDX_HAVE_SYSV_SEMAPHORES)
		, sys::sysv_semaphore
		#endif
		#if defined(UNISTDX_HAVE_POSIX_SEMAPHORES)
		, posix_process_semaphore
		#endif
	)
);

template<class Q, class Mutex, class Semaphore=std::condition_variable, class Thread=std::thread>
struct Thread_pool {

	static constexpr const Q sval = std::numeric_limits<Q>::max();

	Thread_pool():
		queue(),
		cv(),
		mtx(),
		thread([this] () {
		while (!stopped) {
			Q val;
			{
				std::unique_lock<Mutex> lock(mtx);
				cv.wait(lock, [this] () { return stopped || !queue.empty(); });
				if (stopped) break;
				val = queue.front();
				queue.pop();
			}
			if (val == sval) {
//				std::clog << "Stopping thread pool" << std::endl;
				stopped = true;
			} else {
				sum += val;
			}
		}
	}) {}

	void submit(Q q) {
		{
			std::lock_guard<Mutex> lock(mtx);
			queue.push(q);
		}
		cv.notify_one();
	}

	void wait() {
		if (thread.joinable()) {
			thread.join();
		}
	}

	Q result() const { return sum; }

private:
	std::queue<Q> queue;
	Semaphore cv;
	Mutex mtx;
	volatile bool stopped = false;
	Thread thread;
	Q sum = 0;
};

TYPED_TEST(SemaphoreTest, Semaphore) {
	typedef std::mutex Mutex;
	typedef TypeParam Semaphore;
	this->run([&] (unsigned nthreads, uint64_t max) {
		typedef uint64_t I;
		typedef Thread_pool<I, Mutex, Semaphore> Pool;
		std::vector<Pool*> thread_pool(nthreads);
		std::for_each(thread_pool.begin(), thread_pool.end(), [] (Pool*& ptr) {
			ptr = new Pool;
		});
		I expected_sum = (max + I(1))*max/I(2);
		for (I i=1; i<=max; ++i) {
			thread_pool[i%thread_pool.size()]->submit(i);
		}
		for (Pool* pool : thread_pool) {
			pool->submit(Pool::sval);
		}
		std::for_each(
			thread_pool.begin(),
			thread_pool.end(),
			std::mem_fn(&Pool::wait)
		);
		I sum = std::accumulate(
			thread_pool.begin(),
			thread_pool.end(),
			I(0),
			[] (I sum, Pool* ptr) {
				return sum + ptr->result();
			}
		);
	//	for (Pool* pool : thread_pool) {
	//		std::cout << pool->result() << std::endl;
	//	}
	//	std::cout << max << ": " << sum << std::endl;
		sys::delete_each(thread_pool.begin(), thread_pool.end());
		EXPECT_EQ(expected_sum, sum);
	});
}

TYPED_TEST_CASE(
	SemaphoreWaitTest,
	MAKE_TYPES(
		std::condition_variable
		#if defined(UNISTDX_HAVE_POSIX_SEMAPHORES)
		, posix_thread_semaphore
		#endif
		#if defined(UNISTDX_HAVE_SYSV_SEMAPHORES)
		, sys::sysv_semaphore
		#endif
	)
);

TYPED_TEST(SemaphoreWaitTest, WaitUntil) {
	this->test_wait_until();
}

TYPED_TEST(SemaphoreWaitTest, ProducerConsumer) {
	this->test_producer_consumer_thread();
}

#if defined(UNISTDX_HAVE_SYSV_SEMAPHORES)
template <class T>
using SemaphoreProcessTest = SemaphoreWaitTest<T>;

TYPED_TEST_CASE(
	SemaphoreProcessTest,
	MAKE_TYPES(sys::sysv_semaphore)
);

TYPED_TEST(SemaphoreProcessTest, ProducerConsumer) {
	this->test_producer_consumer_process();
}
#endif

//template<class Integer>
//void test_perf_x(Integer m) {
//	Time t0 = current_time_nano();
//	run_multiple_times<Integer>(Test_semaphore<Integer, spin_mutex>, 1, m);
//	run_multiple_times<Integer>(Test_semaphore<Integer, spin_mutex>, 1, m);
//	Time t1 = current_time_nano();
//	run_multiple_times<Integer>(Test_semaphore<Integer, std::mutex>, 1, m);
//	run_multiple_times<Integer>(Test_semaphore<Integer, std::mutex>, 1, m);
//	Time t2 = current_time_nano();
//	std::cout << "Time(spin_mutex, " << m << ") = " << t1-t0 << "ns" << std::endl;
//	std::cout << "Time(std::mutex, " << m << ") = " << t2-t1 << "ns" << std::endl;
//}

//template<class Mutex, class Integer=uint64_t>
//void
//test_perf(Integer m) {
//	Time t0 = current_time_nano();
//	run_multiple_times<Integer>(Test_semaphore<Integer, Mutex>, 1, m);
//	run_multiple_times<Integer>(Test_semaphore<Integer, Mutex>, 1, m);
//	Time t1 = current_time_nano();
//	std::cout
//		<< "mutex=" << typeid(Mutex).name()
//		<< ", int_type=" << typeid(Integer).name()
//		<< ", time=" << t1-t0 << "ns"
//		<< std::endl;
//}
