#include <stdx/debug.hh>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>

#include <stdx/mutex.hh>
#include <stdx/algorithm.hh>
#include <sys/semaphore.hh>

#include "test.hh"

template<class Testname, class I=unsigned>
struct Parametric_test: public test::Test<Testname> {

	typedef I int_type;

	Parametric_test(I minthr, I maxpow):
	_minthreads(minthr),
	_maxpower(maxpow)
	{}

	void xrun() override {
		const I max_threads = std::max(I(std::thread::hardware_concurrency()), I(2)*_minthreads);
		for (I j=_minthreads; j<=max_threads; ++j) {
			for (I i=0; i<=_maxpower; ++i) {
				parametric_run(j, I(1) << i);
			}
		}
	}

	virtual void parametric_run(int_type nthreads, int_type increment) = 0;

private:
	I _minthreads;
	I _maxpower;
};

template<class Mutex>
struct Test_mutex: public Parametric_test<Test_mutex<Mutex>> {

	Test_mutex(unsigned minthr, unsigned maxpow):
	Parametric_test<Test_mutex<Mutex>>(minthr, maxpow) {}

	void
	parametric_run(unsigned nthreads, unsigned increment) override {
		volatile unsigned counter = 0;
		Mutex m;
		std::vector<std::thread> threads;
		for (unsigned i=0; i<nthreads; ++i) {
			threads.push_back(std::thread([&counter, increment, &m] () {
				for (unsigned j=0; j<increment; ++j) {
					std::lock_guard<Mutex> lock(m);
					++counter;
				}
			}));
		}
		for (std::thread& t : threads) {
			t.join();
		}
		unsigned good_counter = nthreads*increment;
		test::equal(counter, good_counter, "bad counter");
	}
};

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
				std::lock_guard<Mutex> lock(mtx);
				cv.wait(mtx, [this] () { return stopped || !queue.empty(); });
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

template<class Mutex, class Semaphore=std::condition_variable, class I=unsigned>
struct Test_semaphore: public Parametric_test<Test_semaphore<Mutex,Semaphore,I>> {

	typedef Test_semaphore<Mutex, Semaphore, I> this_type;

	Test_semaphore(I nthreads_, I max_):
	Parametric_test<this_type>(nthreads_, max_) {}

	void parametric_run(I nthreads, I max) override {
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
		stdx::delete_each(thread_pool.begin(), thread_pool.end());
		test::equal(sum, expected_sum, "bad sum");
	}
};

//template<class Integer>
//void test_perf_x(Integer m) {
//	Time t0 = current_time_nano();
//	run_multiple_times<Integer>(Test_semaphore<Integer, stdx::spin_mutex>, 1, m);
//	run_multiple_times<Integer>(Test_semaphore<Integer, stdx::spin_mutex>, 1, m);
//	Time t1 = current_time_nano();
//	run_multiple_times<Integer>(Test_semaphore<Integer, std::mutex>, 1, m);
//	run_multiple_times<Integer>(Test_semaphore<Integer, std::mutex>, 1, m);
//	Time t2 = current_time_nano();
//	std::cout << "Time(stdx::spin_mutex, " << m << ") = " << t1-t0 << "ns" << std::endl;
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

int main() {
//	sys::init_signal_semaphore init(SIGUSR1);
	return test::Test_suite{
		new Test_mutex<stdx::spin_mutex>(2, 10),
		new Test_semaphore<std::mutex,sys::sysv_semaphore>(1, 10)
	}.run();
}
