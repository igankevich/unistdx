#include <iostream>
#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include <unistdx/base/uint128>

#if defined(UNISTDX_HAVE_INT128)
namespace std {
    constexpr int numeric_limits<sys::uint128_t>::digits;
}

TEST(Uint128Test, Limits) {
    using namespace sys::literals;
    using sys::uint128_t;
    EXPECT_EQ(
        340282366920938463463374607431768211455_u128,
        std::numeric_limits<uint128_t>::max()
    );
    EXPECT_EQ(0_u128, std::numeric_limits<uint128_t>::min());
    EXPECT_EQ(128, std::numeric_limits<uint128_t>::digits);
}
#endif

TEST(uint128, comparisons) {
    // one component
    EXPECT_EQ(sys::u128(), sys::u128(0));
    EXPECT_LT(sys::u128(0), sys::u128(1));
    EXPECT_LE(sys::u128(0), sys::u128(1));
    EXPECT_LE(sys::u128(0), sys::u128(0));
    EXPECT_GT(sys::u128(1), sys::u128(0));
    EXPECT_GE(sys::u128(1), sys::u128(0));
    EXPECT_GE(sys::u128(0), sys::u128(0));
    EXPECT_NE(sys::u128(0), sys::u128(1));
    // two components
    EXPECT_EQ(sys::u128(), sys::u128(0,0));
    EXPECT_LT(sys::u128(0,1), sys::u128(1,0));
    EXPECT_LE(sys::u128(0,1), sys::u128(1,0));
    EXPECT_LE(sys::u128(1,0), sys::u128(1,0));
    EXPECT_GT(sys::u128(1,0), sys::u128(0,1));
    EXPECT_GE(sys::u128(1,0), sys::u128(0,1));
    EXPECT_GE(sys::u128(1,0), sys::u128(1,0));
    EXPECT_NE(sys::u128(1,0), sys::u128(0,1));
}

TEST(uint128, operator_bool) {
    EXPECT_TRUE(bool(sys::u128(0,1)));
    EXPECT_TRUE(bool(sys::u128(1,0)));
    EXPECT_FALSE(bool(sys::u128(0,0)));
    EXPECT_FALSE(!sys::u128(0,1));
    EXPECT_FALSE(!sys::u128(1,0));
    EXPECT_TRUE(!sys::u128(0,0));
}

TEST(uint128, operator_tilde) {
    EXPECT_EQ(sys::u128(0), ~std::numeric_limits<sys::u128>::max());
    EXPECT_EQ(~sys::u128(0), std::numeric_limits<sys::u128>::max());
}

TEST(uint128, operator_minus) {
    EXPECT_EQ(-sys::u64(0), sys::u64(0));
    EXPECT_EQ(-sys::u64(1), std::numeric_limits<sys::u64>::max());
    EXPECT_EQ(-sys::u64(2), std::numeric_limits<sys::u64>::max()-1);
    EXPECT_EQ(-sys::u128(0), sys::u128(0));
    EXPECT_EQ(-sys::u128(1), std::numeric_limits<sys::u128>::max());
    EXPECT_EQ(-sys::u128(2), std::numeric_limits<sys::u128>::max()-1);
}

TEST(uint128, operator_bitwise_or) {
    EXPECT_EQ(sys::u128(0), sys::u128(0) | sys::u128(0));
    EXPECT_EQ(sys::u128(1), sys::u128(0) | sys::u128(1));
    EXPECT_EQ(sys::u128(1), sys::u128(1) | sys::u128(0));
    EXPECT_EQ(sys::u128(1), sys::u128(1) | sys::u128(1));
    sys::u128 x;
    x = 0;
    EXPECT_EQ(sys::u128(0), x |= 0);
    x = 0;
    EXPECT_EQ(sys::u128(1), x |= 1);
    x = 1;
    EXPECT_EQ(sys::u128(1), x |= 0);
    x = 1;
    EXPECT_EQ(sys::u128(1), x |= 1);
}

TEST(uint128, operator_bitwise_and) {
    EXPECT_EQ(sys::u128(0), sys::u128(0) & sys::u128(0));
    EXPECT_EQ(sys::u128(0), sys::u128(0) & sys::u128(1));
    EXPECT_EQ(sys::u128(0), sys::u128(1) & sys::u128(0));
    EXPECT_EQ(sys::u128(1), sys::u128(1) & sys::u128(1));
    sys::u128 x;
    x = 0;
    EXPECT_EQ(sys::u128(0), x &= 0);
    x = 0;
    EXPECT_EQ(sys::u128(0), x &= 1);
    x = 1;
    EXPECT_EQ(sys::u128(0), x &= 0);
    x = 1;
    EXPECT_EQ(sys::u128(1), x &= 1);
}

TEST(uint128, operator_bitwise_xor) {
    EXPECT_EQ(sys::u128(0), sys::u128(0) ^ sys::u128(0));
    EXPECT_EQ(sys::u128(1), sys::u128(0) ^ sys::u128(1));
    EXPECT_EQ(sys::u128(1), sys::u128(1) ^ sys::u128(0));
    EXPECT_EQ(sys::u128(0), sys::u128(1) ^ sys::u128(1));
    sys::u128 x;
    x = 0;
    EXPECT_EQ(sys::u128(0), x ^= 0);
    x = 0;
    EXPECT_EQ(sys::u128(1), x ^= 1);
    x = 1;
    EXPECT_EQ(sys::u128(1), x ^= 0);
    x = 1;
    EXPECT_EQ(sys::u128(0), x ^= 1);
}

TEST(uint128, operator_shift_left) {
    for (int i=0; i<64; ++i) {
        sys::u64 y = sys::u64(1)<<sys::u64(i);
        EXPECT_EQ(sys::u128(0,y), sys::u128(1)<<sys::u128(i)) << "i=" << i;
        sys::u128 x(1);
        x <<= sys::u128(i);
        EXPECT_EQ(sys::u128(0,y), x) << "i=" << i;
    }
    EXPECT_EQ(sys::u128(1,0), sys::u128(1)<<sys::u128(64));
    for (int i=65; i<128; ++i) {
        sys::u64 y = sys::u64(1)<<sys::u64(i-64);
        EXPECT_EQ(sys::u128(y,0), sys::u128(1)<<sys::u128(i)) << "i=" << i;
        sys::u128 x(1);
        x <<= sys::u128(i);
        EXPECT_EQ(sys::u128(y,0), x) << "i=" << i;
    }
}

TEST(uint128, operator_shift_right) {
    for (int i=0; i<64; ++i) {
        sys::u64 y = sys::u64(1)>>sys::u64(i);
        EXPECT_EQ(sys::u128(0,y), sys::u128(1)>>sys::u128(i)) << "i=" << i;
        sys::u128 x(1);
        x >>= sys::u128(i);
        EXPECT_EQ(sys::u128(0,y), x) << "i=" << i;
    }
    EXPECT_EQ(sys::u128(0,1), sys::u128(1,0)>>sys::u128(64));
    for (int i=65; i<128; ++i) {
        sys::u64 y = sys::u64(1)>>sys::u64(i-64);
        EXPECT_EQ(sys::u128(y,0), sys::u128(1)>>sys::u128(i)) << "i=" << i;
        sys::u128 x(1);
        x >>= sys::u128(i);
        EXPECT_EQ(sys::u128(y,0), x) << "i=" << i;
    }
}

TEST(uint128, increment) {
    sys::u128 x;
    x = 0;
    EXPECT_EQ(sys::u128(0), x);
    EXPECT_EQ(sys::u128(1), ++x);
    EXPECT_EQ(sys::u128(1), x);
    x = 0;
    EXPECT_EQ(sys::u128(0), x);
    EXPECT_EQ(sys::u128(0), x++);
    EXPECT_EQ(sys::u128(1), x);
    x = std::numeric_limits<sys::u64>::max();
    EXPECT_EQ(sys::u128(1,0), ++x);
    EXPECT_EQ(sys::u128(1,0), x);
    x = std::numeric_limits<sys::u64>::max();
    EXPECT_EQ(sys::u128(0,std::numeric_limits<sys::u64>::max()), x++);
    EXPECT_EQ(sys::u128(1,0), x);
}

TEST(uint128, decrement) {
    sys::u128 x;
    x = 1;
    EXPECT_EQ(sys::u128(1), x);
    EXPECT_EQ(sys::u128(0), --x);
    EXPECT_EQ(sys::u128(0), x);
    x = 1;
    EXPECT_EQ(sys::u128(1), x);
    EXPECT_EQ(sys::u128(1), x--);
    EXPECT_EQ(sys::u128(0), x);
    x = sys::u128(1,0);
    EXPECT_EQ(sys::u128(0,std::numeric_limits<sys::u64>::max()), --x);
    EXPECT_EQ(sys::u128(0,std::numeric_limits<sys::u64>::max()), x);
    x = sys::u128(1,0);
    EXPECT_EQ(sys::u128(1,0), x--);
    EXPECT_EQ(sys::u128(0,std::numeric_limits<sys::u64>::max()), x);
}

TEST(uint128, add) {
    EXPECT_EQ(sys::u128(1,0), sys::u128(1) + sys::u128(std::numeric_limits<sys::u64>::max()));
    EXPECT_EQ(sys::u128(2,0), sys::u128(1,0) + sys::u128(1,0));
    EXPECT_EQ(sys::u128(1,1), sys::u128(1,0) + sys::u128(0,1));
    EXPECT_EQ(sys::u128(1,1), sys::u128(0,1) + sys::u128(1,0));
    EXPECT_EQ(sys::u128(1,0)<<1, sys::u128(1,0) + sys::u128(1,0));
    EXPECT_EQ(sys::u128(0), std::numeric_limits<sys::u128>::max()+1);
    EXPECT_EQ(sys::u128(1), std::numeric_limits<sys::u128>::max()+2);
    EXPECT_EQ(sys::u128(1,1), sys::u128(1,1) + std::numeric_limits<sys::u128>::max()+1);
}

TEST(uint128, add_assign) {
    sys::u128 x;
    x = 1;
    EXPECT_EQ(sys::u128(1,0), x += sys::u128(std::numeric_limits<sys::u64>::max()));
    x = sys::u128(1,0);
    EXPECT_EQ(sys::u128(2,0), x += sys::u128(1,0));
    x = sys::u128(1,0);
    EXPECT_EQ(sys::u128(1,1), x += sys::u128(0,1));
    x = sys::u128(0,1);
    EXPECT_EQ(sys::u128(1,1), x += sys::u128(1,0));
    x = sys::u128(1,0);
    EXPECT_EQ(sys::u128(1,0)<<1, x += sys::u128(1,0));
    x = std::numeric_limits<sys::u128>::max();
    EXPECT_EQ(sys::u128(0), x+=1);
    x = std::numeric_limits<sys::u128>::max();
    EXPECT_EQ(sys::u128(1), x+=2);
    x = sys::u128(1,1);
    x += std::numeric_limits<sys::u128>::max();
    x += 1;
    EXPECT_EQ(sys::u128(1,1), x);
}

TEST(uint128, subtract) {
    EXPECT_EQ(sys::u128(std::numeric_limits<sys::u64>::max()), sys::u128(1,0) - 1);
    EXPECT_EQ(std::numeric_limits<sys::u128>::max(), sys::u128(0) - 1);
    EXPECT_EQ(-sys::u128(1), sys::u128(0) - 1);
    EXPECT_EQ(sys::u128(2,0)>>1, sys::u128(2,0) - sys::u128(1,0));
    EXPECT_EQ(sys::u128(0), sys::u128(1,0) - sys::u128(1,0));
    EXPECT_EQ(sys::u128(1,1), sys::u128(1,1) - std::numeric_limits<sys::u128>::max()-1);
}

TEST(uint128, subtract_assign) {
    sys::u128 x;
    x = sys::u128(1,0);
    EXPECT_EQ(sys::u128(std::numeric_limits<sys::u64>::max()), x -= 1);
    x = sys::u128(0);
    EXPECT_EQ(std::numeric_limits<sys::u128>::max(), x -= 1);
    x = sys::u128(0);
    EXPECT_EQ(-sys::u128(1), x -= 1);
    x = sys::u128(2,0);
    EXPECT_EQ(sys::u128(2,0)>>1, x -= sys::u128(1,0));
    x = sys::u128(1,0);
    EXPECT_EQ(sys::u128(0), x -= sys::u128(1,0));
    x = sys::u128(1,1);
    x -= std::numeric_limits<sys::u128>::max();
    x -= 1;
    EXPECT_EQ(sys::u128(1,1), x);
}
