#include <unistdx/test/basic_mutex_test>
#include <unistdx/test/thread_mutex_test>
#include <unistdx/test/make_types>

#include <mutex>
#include <unistdx/base/recursive_spin_mutex>
#include <unistdx/base/spin_mutex>

using sys::u64;

template <class T>
struct MutexTest: public BasicMutexTest<T> {};

TYPED_TEST_CASE(MutexTest, MAKE_TYPES(sys::spin_mutex));

TYPED_TEST(MutexTest, Mutex) {
    typedef TypeParam Mutex;
    this->run(
        [&] (unsigned nthreads, u64 increment) {
            volatile unsigned counter = 0;
            Mutex m;
            std::vector<std::thread> threads;
            for (unsigned i=0; i<nthreads; ++i) {
                threads.emplace_back(
                    [&counter, increment, &m] () {
                        for (unsigned j=0; j<increment; ++j) {
                            std::lock_guard<Mutex> lock(m);
                            ++counter;
                        }
                    }
                );
            }
            for (std::thread& t : threads) {
                t.join();
            }
            unsigned good_counter = nthreads*increment;
            EXPECT_EQ(counter, good_counter);
        }
    );
}

TYPED_TEST_CASE(ThreadMutexTest, MAKE_TYPES(std::mutex,sys::spin_mutex));

TYPED_TEST(ThreadMutexTest, SpinMutex) {
    this->test_thread();
}

TEST(RecursiveSpinMutex, LockLock) {
    sys::recursive_spin_mutex mtx;
    mtx.lock();
    mtx.lock();
    mtx.unlock();
    mtx.unlock();
}
