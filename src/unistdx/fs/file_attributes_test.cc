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

#include <unistdx/config>
#include <unistdx/fs/path>
#include <unistdx/fs/temporary_file>

#if defined(UNISTDX_HAVE_SYS_XATTR_H)
TEST(file_attributes, _) {
    try {
        sys::temporary_file file("/dev/shm/file_attributes");
        sys::path path(file.name());
        auto attrs = path.attributes();
        EXPECT_THROW(path.attribute("user.unistdx.x"), sys::bad_call);
        EXPECT_EQ(0, std::distance(attrs.begin(), attrs.end()));
        EXPECT_EQ(nullptr, attrs.data());
        EXPECT_EQ(0, attrs.size());
        path.attribute("user.unistdx.x", "1");
        attrs = path.attributes();
        EXPECT_NE(nullptr, attrs.data());
        EXPECT_NE(0, attrs.size());
        EXPECT_STREQ("1", path.attribute("user.unistdx.x").data());
        EXPECT_STREQ("user.unistdx.x", attrs.front());
        EXPECT_EQ(1, std::distance(attrs.begin(), attrs.end()));
        file.attribute("user.unistdx.x", "2");
        attrs = file.attributes();
        EXPECT_NE(nullptr, attrs.data());
        EXPECT_NE(0, attrs.size());
        EXPECT_STREQ("2", file.attribute("user.unistdx.x").data());
        EXPECT_STREQ("user.unistdx.x", attrs.front());
        EXPECT_EQ(1, std::distance(attrs.begin(), attrs.end()));
    } catch (const std::system_error& err) {
        if (std::errc(err.code().value()) == std::errc::not_supported) {
            std::exit(77);
        }
        throw;
    }
}
#endif

int main(int argc, char* argv[]) {
    #if !defined(UNISTDX_HAVE_SYS_XATTR_H)
    std::exit(77);
    #endif
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
