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
#include <thread>

#include <unistdx/ipc/process>

#include <unistdx/test/operator>

TEST(process_status, exit) {
    sys::process child {
        [] () {
            return 0;
        }
    };
    sys::process_status status = child.wait();
    EXPECT_TRUE(status.exited());
    EXPECT_FALSE(status.killed());
    EXPECT_FALSE(status.stopped());
    EXPECT_FALSE(status.core_dumped());
    EXPECT_FALSE(status.trapped());
    EXPECT_FALSE(status.continued());
    EXPECT_EQ(0, status.exit_code());
    EXPECT_STREQ("exited", status.status_string());
    test::stream_insert_contains("status=exited,exit_code=0", status);
}

TEST(process_status, abort) {
    sys::process child {
        [] () -> int {
            std::abort();
        }
    };
    sys::process_status status = child.wait();
    EXPECT_FALSE(status.exited());
    EXPECT_FALSE(status.stopped());
    EXPECT_TRUE(status.killed() || status.core_dumped());
    EXPECT_FALSE(status.trapped());
    EXPECT_FALSE(status.continued());
    EXPECT_EQ(sys::signal::abort, status.term_signal());
//	EXPECT_STREQ("core_dumped", status.status_string());
    test::stream_insert_contains("signal=abort", status);
}

void
test_print(const char* str, int si_code) {
    sys::siginfo_type s{};
    s.si_code = si_code;
    sys::process_status status(s);
    test::stream_insert_contains(str, status);
}

TEST(process_status, print) {
    test_print("status=stopped", CLD_STOPPED);
    test_print("status=continued", CLD_CONTINUED);
    test_print("status=trapped", CLD_TRAPPED);
}
