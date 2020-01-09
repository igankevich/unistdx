#include <gtest/gtest.h>

#include <unistdx/base/flag>

namespace sys {

    enum class My_flag {A=1, B=2, C=4};

    template <> struct is_flag<My_flag>: public std::true_type {};

}

TEST(flag, _) {
    using sys::My_flag;
    My_flag f = My_flag::A | My_flag::B;
    EXPECT_TRUE(bool(f & My_flag::A));
    EXPECT_TRUE(bool(f & My_flag::B));
    EXPECT_FALSE(bool(f & My_flag::C));
    f |= My_flag::C;
    EXPECT_TRUE(bool(f & My_flag::A));
    EXPECT_TRUE(bool(f & My_flag::B));
    EXPECT_TRUE(bool(f & My_flag::C));
    f &= ~My_flag::B;
    EXPECT_TRUE(bool(f & My_flag::A));
    EXPECT_FALSE(bool(f & My_flag::B));
    EXPECT_TRUE(bool(f & My_flag::C));
}
