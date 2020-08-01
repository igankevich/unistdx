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
#include <string>
#include <unistdx/fs/canonical_path>
#include <unordered_set>

TEST(CanonicalPath, Hash) {
    std::unordered_set<sys::canonical_path> s;
    for (int i=0; i<10; ++i) {
        s.emplace("/tmp");
    }
    EXPECT_EQ(1, s.size());
}

TEST(CanonicalPath, EqualsCurrentWorkingDirectory) {
    sys::canonical_path dir1(".");
    char buf[4096*4] = {0};
    EXPECT_NE(nullptr, ::getcwd(buf, sizeof(buf)));
    sys::canonical_path dir2(buf);
    EXPECT_EQ(dir1, dir2);
    sys::canonical_path dir3 = sys::this_process::workdir();
    EXPECT_EQ(dir2, dir3);
}

TEST(CanonicalPath, Assign) {
    sys::canonical_path dir1(".");
    sys::canonical_path dir2("..");
    dir1 = dir1.dirname();
    EXPECT_EQ(dir1, dir2);
}

TEST(CanonicalPath, Dirname) {
    sys::canonical_path dir1("/usr");
    sys::canonical_path dir2("/");
    EXPECT_EQ(dir1.dirname(), dir2);
    EXPECT_EQ(dir2.dirname(), dir2);
    EXPECT_EQ(dir2.basename(), dir2);
}

TEST(CanonicalPath, TwoArgConstructor) {
    sys::canonical_path cwd(".");
    EXPECT_EQ(sys::canonical_path(cwd.dirname(), cwd.basename()), cwd);
}

TEST(CanonicalPath, NonExistent) {
    sys::canonical_path root("/");
    sys::canonical_path tmp;
    EXPECT_THROW(
        {
            sys::canonical_path nonexistent(root, "non-existent-directory");
        },
        sys::bad_call
    );
}

TEST(CanonicalPath, FromString) {
    std::string str = "/";
    sys::canonical_path root(str);
}
