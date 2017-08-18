#include <sys/net/endian>
#include <gtest/gtest.h>

TEST(EndianGuard, NoThrow) {
	EXPECT_NO_THROW({ sys::endian_guard guard; });
}
