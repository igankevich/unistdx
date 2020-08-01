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

#include <gtest/gtest.h>

#include <unistdx/net/bytes>

#include <unistdx/test/operator>

TEST(bytes, members) {
    const sys::bytes<sys::u32> b(0xff);
    EXPECT_EQ(sizeof(sys::u32), b.size());
    EXPECT_EQ(size_t(b.end() - b.begin()), b.size());
    EXPECT_EQ(0xffu, b.value());
    sys::u32 v = b;
    EXPECT_EQ(0xffu, v);
}

TEST(bytes, front_begin) {
    sys::bytes<sys::u32> b;
    for (int i=0; i<4; ++i) {
        b[i] = i+1;
    }
    EXPECT_EQ(1, b.front());
    EXPECT_EQ(4, b.back());
    sys::bytes<sys::u32> copy(b);
    b.to_network_format();
    if (sys::is_network_byte_order()) {
        EXPECT_EQ(copy, b);
    } else {
        EXPECT_NE(copy, b);
        EXPECT_EQ(4, b[0]);
        EXPECT_EQ(3, b[1]);
        EXPECT_EQ(2, b[2]);
        EXPECT_EQ(1, b[3]);
    }
    b.to_host_format();
    EXPECT_EQ(copy, b);
    test::stream_insert_equals("01 02 03 04", b);
}

TEST(bytes, constructors) {
    sys::bytes<sys::u64> a{};
    sys::bytes<sys::u64> b(0);
    EXPECT_EQ(a, b);
    char data[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    sys::bytes<sys::u64> c(std::begin(data), std::end(data));
    EXPECT_EQ(1, c.front());
    EXPECT_EQ(8, c.back());
}
