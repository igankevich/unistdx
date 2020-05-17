/*
UNISTDX — C++ library for Linux system calls.
© 2017, 2018, 2019, 2020 Ivan Gankevich

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

#include <gtest/gtest.h>

#include <set>

#include <unistdx/bits/for_each_file_descriptor>
#include <unistdx/io/fildes>
#include <unistdx/io/pipe>

#include <unistdx/test/temporary_file>

TEST(ForEachFileDescriptor, Pipe) {
    sys::pipe p;
    std::set<sys::fd_type> expected{p.in().fd(), p.out().fd()}, actual;
    sys::bits::for_each_file_descriptor(
        [&] (const sys::poll_event& rhs) {
            const auto fd = rhs.fd();
            if (fd == p.in().fd() || fd == p.out().fd()) {
                actual.emplace(fd);
            }
        }
    );
    EXPECT_EQ(expected, actual);
}

TEST(fildes, duplicate) {
    sys::fildes f(UNISTDX_TMPFILE, sys::open_flag::create, 0644);
    sys::fildes tmp1(f);
    sys::fildes tmp2(1000);
    tmp2 = f;
}

TEST(fildes, basic) {
    sys::fildes a, b;
    EXPECT_FALSE(a);
    EXPECT_FALSE(b);
    EXPECT_EQ(a, b);
    EXPECT_EQ(a, b.fd());
    EXPECT_EQ(a.fd(), b);
    EXPECT_NO_THROW(b.open(UNISTDX_TMPFILE, sys::open_flag::create, 0644));
    EXPECT_TRUE(static_cast<bool>(b));
    EXPECT_NE(a, b);
    EXPECT_NE(a.fd(), b);
    EXPECT_NE(a, b.fd());
}

TEST(fildes, traits) {
    typedef sys::streambuf_traits<sys::fildes> traits_type;
    sys::fildes a;
    char buf[1024] = {0};
    EXPECT_THROW(traits_type::read(a, buf, 1024), sys::bad_call);
    EXPECT_THROW(traits_type::write(a, buf, 1024), sys::bad_call);
}

TEST(fd_type, traits) {
    typedef sys::streambuf_traits<sys::fd_type> traits_type;
    sys::fildes a;
    char buf[1024] = {0};
    EXPECT_THROW(traits_type::read(a.fd(), buf, 1024), sys::bad_call);
    EXPECT_THROW(traits_type::write(a.fd(), buf, 1024), sys::bad_call);
    a.open("/dev/null");
    EXPECT_TRUE(static_cast<bool>(a));
    EXPECT_NO_THROW(traits_type::read(a.fd(), buf, 1024));
}
