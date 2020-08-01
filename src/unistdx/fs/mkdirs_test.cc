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

#include <unistdx/fs/canonical_path>
#include <unistdx/fs/file_status>
#include <unistdx/fs/mkdirs>
#include <unistdx/test/temporary_file>

TEST(MakeDirectories, Full) {
    sys::path root(UNISTDX_TMPDIR);
    test::tmpdir tdir_h(root);
    EXPECT_NO_THROW(sys::mkdirs(sys::path(root, "1", "2", "3")));
    EXPECT_NO_THROW(sys::mkdirs(sys::path(root, "a", "b")));
    EXPECT_NO_THROW(sys::mkdirs(sys::path(root, "x")));
    EXPECT_NO_THROW(sys::mkdirs(sys::path(root, "x")));
    EXPECT_NO_THROW(sys::mkdirs(sys::path(sys::canonical_path(root), "y")));
    EXPECT_NO_THROW(sys::mkdirs(sys::path("")));
    EXPECT_TRUE(sys::file_status(sys::path(root, "1", "2", "3")).exists());
}

TEST(mkdirs, directory_permissions) {
    sys::path root(UNISTDX_TMPDIR);
    test::tmpdir tdir_h(root);
    UNISTDX_CHECK(::chmod(root, 0));
    try {
        sys::mkdirs(sys::path(root, "z"));
        FAIL();
    } catch (const sys::bad_call& err) {
        EXPECT_EQ(std::errc::permission_denied, err.errc()) << "err=" << err;
    }
    UNISTDX_CHECK(::chmod(root, 0755));
}

TEST(mkdirs, file_in_path) {
    sys::path root(UNISTDX_TMPFILE);
    test::temporary_file tmp(root);
    try {
        sys::mkdirs(sys::path(root, "x"));
        FAIL();
    } catch (const sys::bad_call& err) {
        EXPECT_EQ(std::errc::not_a_directory, err.errc()) << "err=" << err;
    }
    try {
        sys::mkdirs(sys::path(root, "x", "y"));
        FAIL();
    } catch (const sys::bad_call& err) {
        EXPECT_EQ(std::errc::not_a_directory, err.errc()) << "err=" << err;
    }
    try {
        sys::mkdirs(sys::path(root));
        FAIL();
    } catch (const sys::bad_call& err) {
        EXPECT_EQ(std::errc::not_a_directory, err.errc()) << "err=" << err;
    }
}
