#include <gtest/gtest.h>

#include <unistdx/base/types>
#include <unistdx/net/bit_count>

TEST(bit_count, unsigned_long) {
    std::bitset<8*sizeof(unsigned long)> bits;
    bits.set(1);
    bits.set(2);
    bits.set(3);
    bits.set(bits.size()-1);
    EXPECT_EQ(bits.count(), sys::bit_count<unsigned long>(bits.to_ulong()));
}

#if defined(UNISTDX_HAVE_LONG_LONG)
TEST(bit_count, unsigned_long_long) {
    std::bitset<8*sizeof(unsigned long long)> bits;
    bits.set(1);
    bits.set(2);
    bits.set(3);
    bits.set(bits.size()-1);
    EXPECT_EQ(bits.count(), sys::bit_count<unsigned long long>(bits.to_ullong()));
}
#endif

TEST(bit_count, int_types) {
    EXPECT_EQ(1u, sys::bit_count<unsigned int>(1));
    EXPECT_EQ(1u, sys::bit_count<unsigned int>(2));
    EXPECT_EQ(2u, sys::bit_count<unsigned int>(3));
}
