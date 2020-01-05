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
