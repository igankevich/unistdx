/*
UNISTDX — C++ library for Linux system calls.
© 2018, 2019, 2020 Ivan Gankevich

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

#include <algorithm>
#include <iterator>
#include <set>
#include <vector>

#include <unistdx/fs/idirectory>
#include <unistdx/fs/odirectory>

#include <unistdx/test/operator>

#include <test/fs/base.hh>

TEST(idirectory, open_close) {
    std::vector<std::string> files {"a", "b", "c"};
    test::tmpdir tdir(UNISTDX_TMPDIR, files.begin(), files.end());
    sys::idirectory dir;
    test::test_open_close(dir, tdir);
}

TEST(idirectory, file_count) {
    typedef sys::idirectory_iterator<sys::directory_entry> iterator;
    std::vector<std::string> files {"a", "b", "c"};
    test::tmpdir tdir(UNISTDX_TMPDIR, files.begin(), files.end());
    test::test_file_count<sys::idirectory,iterator>(tdir, files);
}

TEST(idirectory, file_list) {
    typedef sys::idirectory_iterator<sys::directory_entry> iterator;
    std::vector<std::string> files {"a", "b", "c"};
    test::tmpdir tdir(UNISTDX_TMPDIR, files.begin(), files.end());
    test::test_file_list<sys::idirectory,iterator>(tdir, files);
}

TEST(idirectory, entries) {
    std::vector<std::string> files {"a", "b", "c"};
    test::tmpdir tdir(UNISTDX_TMPDIR, files.begin(), files.end());
    sys::idirectory dir(tdir);
    std::set<std::string> actual, expected(files.begin(), files.end());
    for (const auto& entry : dir) {
        actual.insert(entry.name());
    }
    EXPECT_EQ(expected, actual);
}

TEST(idirectory, insert) {
    std::vector<std::string> files {"a", "b", "c"};
    test::tmpdir tdir(UNISTDX_TMPDIR, files.begin(), files.end());
    sys::idirectory dir(tdir);
    test::tmpdir otdir(UNISTDX_TMPDIR_OUT);
    sys::odirectory odir(otdir);
    odir << dir;
    std::set<sys::directory_entry> orig;
    {
        sys::idirectory idir1(tdir);
        std::copy(
            sys::idirectory_iterator<sys::directory_entry>(idir1),
            sys::idirectory_iterator<sys::directory_entry>(),
            std::inserter(orig, orig.end())
        );
    }
    std::set<sys::directory_entry> copied;
    {
        sys::idirectory idir2(otdir);
        std::copy(
            sys::idirectory_iterator<sys::directory_entry>(idir2),
            sys::idirectory_iterator<sys::directory_entry>(),
            std::inserter(copied, copied.end())
        );
    }
    EXPECT_EQ(orig, copied)
        << "bad file copy from " << tdir.name()
        << " to " << otdir.name();
}

TEST(directory_entry, members) {
    std::vector<std::string> files {"a", "b", "c"};
    test::tmpdir tdir(UNISTDX_TMPDIR, files.begin(), files.end());
    sys::idirectory dir(tdir);
    sys::directory_entry ent1, ent2, ent3;
    EXPECT_TRUE(static_cast<bool>(dir));
    EXPECT_TRUE(dir.is_open());
    dir >> ent1;
    EXPECT_TRUE(static_cast<bool>(dir));
    EXPECT_EQ(ent1.name(), test::stream_insert(ent1));
    EXPECT_EQ(sys::file_type::regular, sys::get_file_type(dir.getpath(), ent1));
    EXPECT_NE(0u, ent1.inode());
    EXPECT_FALSE(ent1.is_parent_dir());
    EXPECT_FALSE(ent1.is_working_dir());
    dir >> ent2;
    EXPECT_TRUE(static_cast<bool>(dir));
    EXPECT_EQ(ent2.name(), test::stream_insert(ent2));
    EXPECT_EQ(sys::file_type::regular, sys::get_file_type(dir.getpath(), ent2));
    EXPECT_STRNE(ent1.name(), ent2.name());
    EXPECT_NE(ent1, ent2);
    ent3 = ent1;
    EXPECT_EQ(ent1, ent3);
    sys::directory_entry ent4(ent2);
    EXPECT_EQ(ent2, ent4);
    sys::directory_entry ent5(std::move(ent4));
    EXPECT_EQ(ent5, ent4);
}
