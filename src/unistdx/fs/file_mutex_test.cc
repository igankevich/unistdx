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

#include <chrono>
#include <cstdlib>
#include <mutex>
#include <thread>

#include <gtest/gtest.h>

#include <unistdx/fs/file_mutex>
#include <unistdx/ipc/process>
#include <unistdx/test/temporary_file>

TEST(FileMutex, Check) {
    typedef sys::file_mutex mutex_type;
    typedef std::lock_guard<mutex_type> lock_type;
    test::temporary_file tmp(UNISTDX_TMPFILE);
    mutex_type mtx(tmp.path(), 0600);
    EXPECT_TRUE(static_cast<bool>(mtx));
    sys::process child([&tmp] () {
        mutex_type mtx2(tmp.path(), 0600);
        lock_type lock(mtx2);
        ::pause();
        return 0;
    });
    using namespace std::chrono;
    using namespace std::this_thread;
    sleep_for(milliseconds(500));
    EXPECT_FALSE(mtx.try_lock());
    child.terminate();
    child.join();
    EXPECT_TRUE(mtx.try_lock());
    EXPECT_NO_THROW(mtx.lock());
    EXPECT_NO_THROW(mtx.unlock());
}

TEST(file_mutex, bad_lock) {
    sys::file_mutex mtx;
    EXPECT_NO_THROW(mtx.open(UNISTDX_TMPFILE, 600));
    EXPECT_NO_THROW(mtx.close());
    EXPECT_NO_THROW(sys::remove(UNISTDX_TMPFILE));
    EXPECT_THROW(mtx.try_lock(), sys::bad_call);
    EXPECT_THROW(mtx.lock(), sys::bad_call);
    EXPECT_THROW(mtx.unlock(), sys::bad_call);
}
