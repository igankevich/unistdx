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

#include <iostream>
#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include <unistdx/base/uint128>
#include <unistdx/test/operator>

TEST(uint128, Limits) {
    using namespace sys::literals;
    using sys::u128;
    EXPECT_EQ(
        340282366920938463463374607431768211455_u128,
        std::numeric_limits<u128>::max()
    );
    EXPECT_EQ(0_u128, std::numeric_limits<u128>::min());
    EXPECT_EQ(128, std::numeric_limits<u128>::digits);
}

using sys::u128;
using sys::u64;

TEST(uint128, comparisons) {
    // one component
    EXPECT_EQ(u128(), u128(0));
    EXPECT_LT(u128(0), u128(1));
    EXPECT_LE(u128(0), u128(1));
    EXPECT_LE(u128(0), u128(0));
    EXPECT_GT(u128(1), u128(0));
    EXPECT_GE(u128(1), u128(0));
    EXPECT_GE(u128(0), u128(0));
    EXPECT_NE(u128(0), u128(1));
    // two components
    EXPECT_EQ(u128(), u128(0,0));
    EXPECT_LT(u128(0,1), u128(1,0));
    EXPECT_LE(u128(0,1), u128(1,0));
    EXPECT_LE(u128(1,0), u128(1,0));
    EXPECT_GT(u128(1,0), u128(0,1));
    EXPECT_GE(u128(1,0), u128(0,1));
    EXPECT_GE(u128(1,0), u128(1,0));
    EXPECT_NE(u128(1,0), u128(0,1));
}

TEST(uint128, operator_bool) {
    EXPECT_TRUE(bool(u128(0,1)));
    EXPECT_TRUE(bool(u128(1,0)));
    EXPECT_FALSE(bool(u128(0,0)));
    EXPECT_FALSE(!u128(0,1));
    EXPECT_FALSE(!u128(1,0));
    EXPECT_TRUE(!u128(0,0));
}

TEST(uint128, operator_tilde) {
    EXPECT_EQ(u128(0), ~std::numeric_limits<u128>::max());
    EXPECT_EQ(~u128(0), std::numeric_limits<u128>::max());
}

TEST(uint128, operator_minus) {
    EXPECT_EQ(-u64(0), u64(0));
    EXPECT_EQ(-u64(1), std::numeric_limits<u64>::max());
    EXPECT_EQ(-u64(2), std::numeric_limits<u64>::max()-1);
    EXPECT_EQ(-u128(0), u128(0));
    EXPECT_EQ(-u128(1), std::numeric_limits<u128>::max());
    EXPECT_EQ(-u128(2), std::numeric_limits<u128>::max()-1);
}

TEST(uint128, operator_bitwise_or) {
    EXPECT_EQ(u128(0), u128(0) | u128(0));
    EXPECT_EQ(u128(1), u128(0) | u128(1));
    EXPECT_EQ(u128(1), u128(1) | u128(0));
    EXPECT_EQ(u128(1), u128(1) | u128(1));
    u128 x;
    x = 0;
    EXPECT_EQ(u128(0), x |= 0);
    x = 0;
    EXPECT_EQ(u128(1), x |= 1);
    x = 1;
    EXPECT_EQ(u128(1), x |= 0);
    x = 1;
    EXPECT_EQ(u128(1), x |= 1);
}

TEST(uint128, operator_bitwise_and) {
    EXPECT_EQ(u128(0), u128(0) & u128(0));
    EXPECT_EQ(u128(0), u128(0) & u128(1));
    EXPECT_EQ(u128(0), u128(1) & u128(0));
    EXPECT_EQ(u128(1), u128(1) & u128(1));
    u128 x;
    x = 0;
    EXPECT_EQ(u128(0), x &= 0);
    x = 0;
    EXPECT_EQ(u128(0), x &= 1);
    x = 1;
    EXPECT_EQ(u128(0), x &= 0);
    x = 1;
    EXPECT_EQ(u128(1), x &= 1);
}

TEST(uint128, operator_bitwise_xor) {
    EXPECT_EQ(u128(0), u128(0) ^ u128(0));
    EXPECT_EQ(u128(1), u128(0) ^ u128(1));
    EXPECT_EQ(u128(1), u128(1) ^ u128(0));
    EXPECT_EQ(u128(0), u128(1) ^ u128(1));
    u128 x;
    x = 0;
    EXPECT_EQ(u128(0), x ^= 0);
    x = 0;
    EXPECT_EQ(u128(1), x ^= 1);
    x = 1;
    EXPECT_EQ(u128(1), x ^= 0);
    x = 1;
    EXPECT_EQ(u128(0), x ^= 1);
}

TEST(uint128, operator_shift_left) {
    for (int i=0; i<64; ++i) {
        u64 y = u64(1)<<u64(i);
        EXPECT_EQ(u128(0,y), u128(1)<<u128(i)) << "i=" << i;
        u128 x(1);
        x <<= u128(i);
        EXPECT_EQ(u128(0,y), x) << "i=" << i;
    }
    EXPECT_EQ(u128(1,0), u128(1)<<u128(64));
    for (int i=65; i<128; ++i) {
        u64 y = u64(1)<<u64(i-64);
        EXPECT_EQ(u128(y,0), u128(1)<<u128(i)) << "i=" << i;
        u128 x(1);
        x <<= u128(i);
        EXPECT_EQ(u128(y,0), x) << "i=" << i;
    }
}

TEST(uint128, operator_shift_right) {
    for (int i=0; i<64; ++i) {
        u64 y = u64(1)>>u64(i);
        EXPECT_EQ(u128(0,y), u128(1)>>u128(i)) << "i=" << i;
        u128 x(1);
        x >>= u128(i);
        EXPECT_EQ(u128(0,y), x) << "i=" << i;
    }
    EXPECT_EQ(u128(0,1), u128(1,0)>>u128(64));
    for (int i=65; i<128; ++i) {
        u64 y = u64(1)>>u64(i-64);
        EXPECT_EQ(u128(y,0), u128(1)>>u128(i)) << "i=" << i;
        u128 x(1);
        x >>= u128(i);
        EXPECT_EQ(u128(y,0), x) << "i=" << i;
    }
}

TEST(uint128, increment) {
    u128 x;
    x = 0;
    EXPECT_EQ(u128(0), x);
    EXPECT_EQ(u128(1), ++x);
    EXPECT_EQ(u128(1), x);
    x = 0;
    EXPECT_EQ(u128(0), x);
    EXPECT_EQ(u128(0), x++);
    EXPECT_EQ(u128(1), x);
    x = std::numeric_limits<u64>::max();
    EXPECT_EQ(u128(1,0), ++x);
    EXPECT_EQ(u128(1,0), x);
    x = std::numeric_limits<u64>::max();
    EXPECT_EQ(u128(0,std::numeric_limits<u64>::max()), x++);
    EXPECT_EQ(u128(1,0), x);
}

TEST(uint128, decrement) {
    u128 x;
    x = 1;
    EXPECT_EQ(u128(1), x);
    EXPECT_EQ(u128(0), --x);
    EXPECT_EQ(u128(0), x);
    x = 1;
    EXPECT_EQ(u128(1), x);
    EXPECT_EQ(u128(1), x--);
    EXPECT_EQ(u128(0), x);
    x = u128(1,0);
    EXPECT_EQ(u128(0,std::numeric_limits<u64>::max()), --x);
    EXPECT_EQ(u128(0,std::numeric_limits<u64>::max()), x);
    x = u128(1,0);
    EXPECT_EQ(u128(1,0), x--);
    EXPECT_EQ(u128(0,std::numeric_limits<u64>::max()), x);
}

TEST(uint128, add) {
    EXPECT_EQ(u128(1,0), u128(1) + u128(std::numeric_limits<u64>::max()));
    EXPECT_EQ(u128(2,0), u128(1,0) + u128(1,0));
    EXPECT_EQ(u128(1,1), u128(1,0) + u128(0,1));
    EXPECT_EQ(u128(1,1), u128(0,1) + u128(1,0));
    EXPECT_EQ(u128(1,0)<<1, u128(1,0) + u128(1,0));
    EXPECT_EQ(u128(0), std::numeric_limits<u128>::max()+1);
    EXPECT_EQ(u128(1), std::numeric_limits<u128>::max()+2);
    EXPECT_EQ(u128(1,1), u128(1,1) + std::numeric_limits<u128>::max()+1);
}

TEST(uint128, add_assign) {
    u128 x;
    x = 1;
    EXPECT_EQ(u128(1,0), x += u128(std::numeric_limits<u64>::max()));
    x = u128(1,0);
    EXPECT_EQ(u128(2,0), x += u128(1,0));
    x = u128(1,0);
    EXPECT_EQ(u128(1,1), x += u128(0,1));
    x = u128(0,1);
    EXPECT_EQ(u128(1,1), x += u128(1,0));
    x = u128(1,0);
    EXPECT_EQ(u128(1,0)<<1, x += u128(1,0));
    x = std::numeric_limits<u128>::max();
    EXPECT_EQ(u128(0), x+=1);
    x = std::numeric_limits<u128>::max();
    EXPECT_EQ(u128(1), x+=2);
    x = u128(1,1);
    x += std::numeric_limits<u128>::max();
    x += 1;
    EXPECT_EQ(u128(1,1), x);
}

TEST(uint128, subtract) {
    EXPECT_EQ(u128(std::numeric_limits<u64>::max()), u128(1,0) - 1);
    EXPECT_EQ(std::numeric_limits<u128>::max(), u128(0) - 1);
    EXPECT_EQ(-u128(1), u128(0) - 1);
    EXPECT_EQ(u128(2,0)>>1, u128(2,0) - u128(1,0));
    EXPECT_EQ(u128(0), u128(1,0) - u128(1,0));
    EXPECT_EQ(u128(1,1), u128(1,1) - std::numeric_limits<u128>::max()-1);
}

TEST(uint128, subtract_assign) {
    u128 x;
    x = u128(1,0);
    EXPECT_EQ(u128(std::numeric_limits<u64>::max()), x -= 1);
    x = u128(0);
    EXPECT_EQ(std::numeric_limits<u128>::max(), x -= 1);
    x = u128(0);
    EXPECT_EQ(-u128(1), x -= 1);
    x = u128(2,0);
    EXPECT_EQ(u128(2,0)>>1, x -= u128(1,0));
    x = u128(1,0);
    EXPECT_EQ(u128(0), x -= u128(1,0));
    x = u128(1,1);
    x -= std::numeric_limits<u128>::max();
    x -= 1;
    EXPECT_EQ(u128(1,1), x);
}

TEST(uint128, multiply) {
    EXPECT_EQ(u128(0,0), u128(1,1)*u128(0));
    EXPECT_EQ(u128(0,2), u128(0,1)*u128(2));
    EXPECT_EQ(u128(2,0), u128(1,0)*u128(2));
    EXPECT_EQ(
        u128(std::numeric_limits<u64>::max())+
        u128(std::numeric_limits<u64>::max()),
        u128(std::numeric_limits<u64>::max())*u128(2)
    );
}

TEST(uint128, multiply_assign) {
    u128 x;
    x = u128(1,1);
    EXPECT_EQ(u128(0,0), x *= u128(0));
    x = u128(0,1);
    EXPECT_EQ(u128(0,2), x *= u128(2));
    x = u128(1,0);
    EXPECT_EQ(u128(2,0), x *= u128(2));
    x = u128(std::numeric_limits<u64>::max());
    EXPECT_EQ(
        u128(std::numeric_limits<u64>::max())+
        u128(std::numeric_limits<u64>::max()), x*=u128(2));
}

TEST(uint128, divide) {
    EXPECT_EQ(u128(0,1), u128(1,1) / u128(1,1));
    EXPECT_EQ(u128(0,2), u128(0,2) / u128(0,1));
    EXPECT_EQ(u128(0,0), u128(0,9) % u128(0,3));
    EXPECT_EQ(u128(0,1), u128(0,10) % u128(0,3));
    EXPECT_EQ(u128(0,0), u128(0,1) / u128(0,3));
    EXPECT_EQ(u128(0,1), u128(0,1) % u128(0,3));
    EXPECT_EQ(u128(1,0), u128(2,0) / u128(0,2));
    EXPECT_EQ(u128(0,0), u128(2,0) % u128(0,2));
}

TEST(uint128, divide_assign) {
    u128 x;
    x = u128(1,1);
    EXPECT_EQ(u128(0,1), x / u128(1,1));
    x = u128(0,2);
    EXPECT_EQ(u128(0,2), x / u128(0,1));
    x = u128(0,9);
    EXPECT_EQ(u128(0,0), x % u128(0,3));
    x = u128(0,10);
    EXPECT_EQ(u128(0,1), x % u128(0,3));
    x = u128(0,1);
    EXPECT_EQ(u128(0,0), x / u128(0,3));
    x = u128(0,1);
    EXPECT_EQ(u128(0,1), x % u128(0,3));
    x = u128(2,0);
    EXPECT_EQ(u128(1,0), x / u128(0,2));
    x = u128(2,0);
    EXPECT_EQ(u128(0,0), x % u128(0,2));
}

TEST(uint128, to_string) {
    EXPECT_EQ("0", to_string(u128(0)));
    EXPECT_EQ("1", to_string(u128(1)));
    EXPECT_EQ("10", to_string(u128(10)));
    EXPECT_EQ("340282366920938463463374607431768211455",
              to_string(std::numeric_limits<u128>::max()));
}

TEST(uint128, stream_insert) {
    EXPECT_EQ("0", ([](){ std::stringstream s; s << u128(0); return s.str(); })());
    EXPECT_EQ("+0", ([](){
        std::stringstream s;
        s.flags(std::ios::showpos);
        s << u128(0);
        return s.str();
    })());
    EXPECT_EQ("0    ", ([](){
        std::stringstream s;
        s.width(5);
        s.flags(std::ios::left);
        s << u128(0);
        return s.str();
    })());
    EXPECT_EQ("    0", ([](){
        std::stringstream s;
        s.width(5);
        s.flags(std::ios::right);
        s << u128(0);
        return s.str();
    })());
    EXPECT_EQ("   +0", ([](){
        std::stringstream s;
        s.width(5);
        s.flags(std::ios::right | std::ios::showpos);
        s << u128(0);
        return s.str();
    })());
    EXPECT_EQ("ff", ([](){
        std::stringstream s;
        s.flags(std::ios::hex);
        s << u128(255);
        return s.str();
    })());
    EXPECT_EQ("77", ([](){
        std::stringstream s;
        s.flags(std::ios::oct);
        s << u128(63);
        return s.str();
    })());
    EXPECT_EQ("000000+777", ([](){
        std::stringstream s;
        s.flags(std::ios::oct | std::ios::showpos);
        s.width(10);
        s.fill('0');
        s << u128(512-1);
        return s.str();
    })());
}
