/*
UNISTDX — C++ library for Linux system calls.
© 2020 Ivan Gankevich

This file is part of UNISTDX.

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/

#include <unistdx/base/make_object>

#include <algorithm>
#include <cmath>
#include <condition_variable>
#include <mutex>
#include <numeric>
#include <queue>
#include <thread>
#include <vector>

#include <unistdx/base/delete_each>
#include <unistdx/ipc/semaphore>

#include <gtest/gtest.h>

#include <unistdx/test/basic_mutex_test>
#include <unistdx/test/make_types>
#include <unistdx/test/semaphore_wait_test>
#include <unistdx/test/thread_mutex_test>

using sys::u64;

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
        #if defined(UNISTDX_HAVE_SYS_SEM_H)
        , sys::sysv_semaphore
        #endif
        #if defined(UNISTDX_HAVE_SEMAPHORE_H)
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
    try {
        this->run([&] (unsigned nthreads, u64 max) {
            typedef u64 I;
            typedef Thread_pool<I, Mutex, Semaphore> Pool;
            std::vector<std::unique_ptr<Pool>> thread_pool;
            thread_pool.reserve(nthreads);
            for (unsigned i=0; i<nthreads; ++i) {
                thread_pool.emplace_back(new Pool);
            }
            I expected_sum = (max + I(1))*max/I(2);
            for (I i=1; i<=max; ++i) {
                thread_pool[i%thread_pool.size()]->submit(i);
            }
            for (auto& pool : thread_pool) { pool->submit(Pool::sval); }
            for (auto& pool : thread_pool) { pool->wait(); }
            I sum = 0;
            for (const auto& pool : thread_pool) { sum += pool->result(); }
        //	for (Pool* pool : thread_pool) {
        //		std::cout << pool->result() << std::endl;
        //	}
        //	std::cout << max << ": " << sum << std::endl;
            EXPECT_EQ(expected_sum, sum);
        });
    } catch (const sys::bad_call& err) {
        if (err.errc() != std::errc::function_not_supported) {
            throw;
        }
        std::exit(77);
    }
}

TYPED_TEST_CASE(
    SemaphoreWaitTest,
    MAKE_TYPES(
        std::condition_variable
        #if defined(UNISTDX_HAVE_SEMAPHORE_H)
        , posix_thread_semaphore
        #endif
        #if defined(UNISTDX_HAVE_SYS_SEM_H)
        , sys::sysv_semaphore
        #endif
    )
);

TYPED_TEST(SemaphoreWaitTest, WaitUntil) {
    try {
        this->test_wait_until();
    } catch (const sys::bad_call& err) {
        if (err.errc() != std::errc::function_not_supported) {
            throw;
        }
        std::exit(77);
    }
}

TYPED_TEST(SemaphoreWaitTest, ProducerConsumer) {
    this->test_producer_consumer_thread();
}

#if defined(UNISTDX_HAVE_SYS_SEM_H)
template <class T>
using SemaphoreProcessTest = SemaphoreWaitTest<T>;

TYPED_TEST_CASE(
    SemaphoreProcessTest,
    MAKE_TYPES(sys::sysv_semaphore)
);

TYPED_TEST(SemaphoreProcessTest, ProducerConsumer) {
    try {
        this->test_producer_consumer_process();
    } catch (const sys::bad_call& err) {
        if (err.errc() != std::errc::function_not_supported) {
            throw;
        }
        std::exit(77);
    }
}
#endif
