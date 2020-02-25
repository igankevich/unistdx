#include <unistdx/net/byte_swap>

#include <gtest/gtest.h>

TEST(ByteSwap, All) {
    #define UINT128_C(x) x ## _u128
    using namespace sys;
    EXPECT_EQ(byte_swap<u16>(UINT16_C(0xABCD)), UINT16_C(0xCDAB));
    EXPECT_EQ(byte_swap<u32>(UINT32_C(0xABCDDCBA)), UINT32_C(0xBADCCDAB));
    EXPECT_EQ(
        byte_swap<u64>(UINT64_C(0xABCDDCBA12344321)),
        UINT64_C(0x21433412BADCCDAB)
    );
    #if defined(UNISTDX_HAVE_INT128)
    using namespace sys::literals;
    EXPECT_EQ(
        byte_swap<u128>(UINT128_C(0xABCDDCBA12344321ae15826154cdef25)),
        UINT128_C(0x25efcd54618215ae21433412badccdab)
    );
    #endif
}
