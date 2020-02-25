#include <iostream>
#include <sstream>
#include <string>

#include <gtest/gtest.h>

#if defined(UNISTDX_HAVE_INT128)
#include <unistdx/base/uint128>
#endif

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
