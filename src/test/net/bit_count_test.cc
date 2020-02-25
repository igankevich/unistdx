#include <gtest/gtest.h>

#include <unistdx/base/types>
#include <unistdx/net/bit_count>

TEST(bit_count, long_types) {
    #if defined(UNISTDX_HAVE_LONG_LONG)
    std::bitset<8*sizeof(unsigned long long)> bits;
    #else
    std::bitset<8*sizeof(unsigned long)> bits;
    #endif
    bits.set(1);
    bits.set(2);
    bits.set(3);
    bits.set(bits.size()-1);
    #if defined(UNISTDX_HAVE_LONG_LONG)
    EXPECT_EQ(bits.count(), sys::bit_count<unsigned long long>(bits.to_ullong()));
    #else
    EXPECT_EQ(bits.count(), sys::bit_count<unsigned long>(bits.to_ulong()));
    #endif
}

TEST(bit_count, int_types) {
    EXPECT_EQ(1u, sys::bit_count<unsigned int>(1));
    EXPECT_EQ(1u, sys::bit_count<unsigned int>(2));
    EXPECT_EQ(2u, sys::bit_count<unsigned int>(3));
}
