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

#include <algorithm>
#include <cmath>
#include <condition_variable>
#include <mutex>
#include <numeric>
#include <queue>
#include <thread>
#include <vector>

#include <unistdx/ipc/semaphore>

#include <gtest/gtest.h>

#include <unistdx/test/basic_mutex_test>
#include <unistdx/test/make_types>
#include <unistdx/test/semaphore_base>
#include <unistdx/test/semaphore_wait_test>
#include <unistdx/test/thread_mutex_test>

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
        #if defined(UNISTDX_HAVE_SEMAPHORE_H)
        , posix_process_semaphore
        #endif
    )
);

TYPED_TEST(SemaphoreTest, Semaphore) {
    using Semaphore = TypeParam;
    test_semaphore<Semaphore>();
}

TYPED_TEST_CASE(
    SemaphoreWaitTest,
    MAKE_TYPES(
        std::condition_variable
        #if defined(UNISTDX_HAVE_SEMAPHORE_H)
        , posix_thread_semaphore
        #endif
    )
);

TYPED_TEST(SemaphoreWaitTest, WaitUntil) {
    this->test_wait_until();
}

TYPED_TEST(SemaphoreWaitTest, ProducerConsumer) {
    this->test_producer_consumer_thread();
}

int main(int argc, char* argv[]) {
    test_semaphore_is_available<posix_thread_semaphore>();
    test_semaphore_is_available<posix_process_semaphore>();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
