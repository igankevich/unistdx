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

#include <unistdx/fs/file_status>

#include <unistdx/test/exception>
#include <unistdx/test/operator>

TEST(file_status, errors) {
    UNISTDX_EXPECT_ERROR(
        std::errc::no_such_file_or_directory,
        sys::file_status("non-existent-file")
    );
    sys::file_status st;
    UNISTDX_EXPECT_ERROR(
        std::errc::no_such_file_or_directory,
        st.update("non-existent-file")
    );
}

TEST(file_status, print) {
    test::stream_insert_starts_with("d", sys::file_status("src"));
}

TEST(file_status, members) {
    sys::file_status st("src");
    EXPECT_TRUE(st.is_directory());
    EXPECT_FALSE(st.is_regular());
    EXPECT_FALSE(st.is_block_device());
    EXPECT_FALSE(st.is_character_device());
    EXPECT_FALSE(st.is_socket());
    EXPECT_FALSE(st.is_pipe());
    EXPECT_FALSE(st.is_symbolic_link());
}
