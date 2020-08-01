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
#include <unistdx/ipc/identity>

TEST(SetIdentity, ExceptionsRunAsRoot) {
    using namespace sys::this_process;
    if (user() != sys::superuser()) {
        return;
    }
    EXPECT_NE(1000u, user());
    EXPECT_NE(1000u, group());
    EXPECT_NO_THROW(set_identity(1000, 1000));
    EXPECT_EQ(1000u, user());
    EXPECT_EQ(1000u, group());
    EXPECT_EQ(1000u, effective_user());
    EXPECT_EQ(1000u, effective_group());
}

TEST(identity, basic) {
    using namespace sys::this_process;
    EXPECT_EQ(user(), effective_user());
    EXPECT_EQ(group(), effective_group());
    EXPECT_EQ(0u, sys::supergroup());
    EXPECT_EQ(0u, sys::superuser());
}

TEST(identity, set_unpriviledged) {
    using namespace sys::this_process;
    if (user() == sys::superuser()) {
        return;
    }
    sys::uid_type olduser = user();
    sys::gid_type oldgroup = group();
    sys::uid_type newuid = user() + 1;
    sys::gid_type newgid = group() + 1;
    EXPECT_THROW(set_identity(newuid, newgid), sys::bad_call);
    EXPECT_EQ(olduser, user());
    EXPECT_EQ(oldgroup, group());
}
