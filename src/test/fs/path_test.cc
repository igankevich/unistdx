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
#include <string>
#include <unistdx/fs/path>
#include <unordered_set>

TEST(Path, Hash) {
    std::unordered_set<sys::path> s;
    for (int i=0; i<10; ++i) {
        s.emplace("/tmp");
    }
    EXPECT_EQ(1u, s.size());
}

TEST(Path, Equals) {
    sys::path p("/tmp");
    sys::path q("/tmpx");
    // ==
    EXPECT_EQ(p, p);
    EXPECT_EQ("/tmp", p);
    EXPECT_EQ(p, "/tmp");
    EXPECT_EQ(std::string("/tmp"), p);
    EXPECT_EQ(p, std::string("/tmp"));
    // !=
    EXPECT_NE(q, p);
    EXPECT_NE(p, q);
    EXPECT_NE("/tmpx", p);
    EXPECT_NE(p, "/tmpx");
    EXPECT_NE(std::string("/tmpx"), p);
    EXPECT_NE(p, std::string("/tmpx"));
}

TEST(Path, VariadicConstructor) {
    EXPECT_EQ("a", sys::path("a"));
    EXPECT_EQ("a/b", sys::path("a", "b"));
    EXPECT_EQ("a/b/c", sys::path("a", "b", "c"));
    EXPECT_EQ("a/b/c/d", sys::path("a", "b", "c", "d"));
}
