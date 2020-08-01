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

#include <unistdx/io/two_way_pipe>

#include <unistdx/test/operator>

TEST(two_way_pipe, print) {
    sys::two_way_pipe p;
    EXPECT_NE("", test::stream_insert(p));
}

TEST(two_way_pipe, close_in_parent) {
    sys::two_way_pipe p;
    EXPECT_NO_THROW(p.validate());
    p.close_in_parent();
    EXPECT_NO_THROW(p.validate());
}

TEST(two_way_pipe, close_unused) {
    sys::two_way_pipe p;
    EXPECT_NO_THROW(p.validate());
    p.close_unused();
    EXPECT_NO_THROW(p.validate());
}

TEST(two_way_pipe, close) {
    sys::two_way_pipe p;
    EXPECT_NO_THROW(p.validate());
    p.close();
    EXPECT_THROW(p.validate(), sys::bad_call);
}

TEST(two_way_pipe, close_in_child) {
    sys::two_way_pipe p;
    EXPECT_NO_THROW(p.validate());
    p.close_in_child();
    EXPECT_THROW(p.validate(), sys::bad_call);
}

TEST(two_way_pipe, child_close_in_child) {
    sys::two_way_pipe p;
    EXPECT_NO_THROW(p.validate());
    sys::process child{
        [&p] () {
            int ret = 0;
            try {
                p.close_in_child();
            } catch (...) {
                ++ret;
            }
            try {
                p.validate();
            } catch (...) {
                ++ret;
            }
            return ret;
        }
    };
    p.close_in_parent();
    EXPECT_NO_THROW(p.validate());
    sys::process_status status = child.wait();
    EXPECT_EQ(0, status.exit_code());
}

TEST(two_way_pipe, child_close_unused) {
    sys::two_way_pipe p;
    EXPECT_NO_THROW(p.validate());
    sys::process child{
        [&p] () {
            int ret = 0;
            try {
                p.close_unused();
            } catch (...) {
                ++ret;
            }
            try {
                p.validate();
            } catch (...) {
                ++ret;
            }
            return ret;
        }
    };
    p.close_in_parent();
    EXPECT_NO_THROW(p.validate());
    sys::process_status status = child.wait();
    EXPECT_EQ(0, status.exit_code());
}

TEST(two_way_pipe, open) {
    sys::two_way_pipe p;
    EXPECT_NO_THROW(p.validate());
    p.open();
    EXPECT_NO_THROW(p.validate());
}
