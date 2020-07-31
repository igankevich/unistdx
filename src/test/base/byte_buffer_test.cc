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

#include <gtest/gtest.h>

#include <unistdx/base/byte_buffer>
#include <unistdx/base/check>

TEST(byte_buffer, resize) {
    const size_t max0 = sys::byte_buffer::max_size();
    sys::byte_buffer buf(4096);
    EXPECT_THROW(buf.resize(max0), sys::bad_call);
    EXPECT_THROW(sys::byte_buffer b(max0), sys::bad_call);
}

TEST(byte_buffer, resize_empty) {
    sys::byte_buffer buf(0);
    buf.resize(4096);
    EXPECT_EQ(4096u, buf.size());
}

TEST(byte_buffer, read) {
    sys::byte_buffer buf(0);
    char tmp[16];
    EXPECT_THROW(buf.read(tmp, sizeof(tmp)), std::range_error);
    buf.resize(4096);
    EXPECT_NO_THROW(buf.read(tmp, sizeof(tmp)));
}

TEST(byte_buffer, peek) {
    sys::byte_buffer buf(0);
    char tmp[16];
    EXPECT_THROW(buf.peek(tmp, sizeof(tmp)), std::range_error);
    buf.resize(4096);
    EXPECT_NO_THROW(buf.peek(tmp, sizeof(tmp)));
}

TEST(byte_buffer, copy) {
    sys::byte_buffer a(4096);
    a.write(1);
    a.write(2);
    a.write(3);
    sys::byte_buffer b(a);
    EXPECT_TRUE(std::equal(a.data(), a.data()+a.size(), b.data()));
    sys::byte_buffer c(4096);
    c.write(4);
    c.write(5);
    c.write(6);
    c = a;
    EXPECT_TRUE(std::equal(a.data(), a.data()+a.size(), c.data()));
}
