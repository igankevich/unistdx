/*
UNISTDX — C++ library for Linux system calls.
© 2017, 2018, 2020 Ivan Gankevich

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

#include <unistdx/test/basic_mutex_test>
#include <unistdx/test/make_types>
#include <unistdx/test/thread_mutex_test>

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
