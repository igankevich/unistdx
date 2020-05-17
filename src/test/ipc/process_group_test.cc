/*
UNISTDX — C++ library for Linux system calls.
© 2018, 2020 Ivan Gankevich

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

#include <mutex>

#include <unistdx/ipc/process_group>
#include <unistdx/test/config>
#include <unistdx/test/operator>

using f = sys::process_flag;

TEST(process_group, wait_async) {
    sys::process_group g;
    g.emplace([] () { return 0; }, f::wait_for_exec | f::signal_parent);
    g.emplace([] () { return 0; }, f::wait_for_exec | f::signal_parent);
    std::mutex mtx;
    g.wait(
        mtx,
        [] (const sys::process&, sys::process_status status) {
            EXPECT_TRUE(status.exited());
            EXPECT_FALSE(status.killed());
            EXPECT_FALSE(status.stopped());
            EXPECT_FALSE(status.core_dumped());
            EXPECT_FALSE(status.trapped());
            EXPECT_FALSE(status.continued());
            EXPECT_EQ(0, status.exit_code());
            EXPECT_STREQ("exited", status.status_string());
            EXPECT_NE("", test::stream_insert(status));
        }
    );
}

TEST(process_group, wait_sync) {
    sys::process_group g;
    g.emplace([] () { return 0; }, f::wait_for_exec | f::signal_parent);
    g.emplace([] () { return 0; }, f::wait_for_exec | f::signal_parent);
    int ret = g.wait();
    EXPECT_EQ(0, ret);
}

int main(int argc, char* argv[]) {
    #if !defined(UNISTDX_TEST_HAVE_VFORK)
    std::exit(77);
    #endif
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
