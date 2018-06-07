#include <gtest/gtest.h>

#include <stdexcept>

#include <unistdx/net/byte_order>

void
endian_guard() {
	union Endian {
		constexpr Endian() {}
		u32 i = UINT32_C(1);
		u8 b[4];
	} endian;
	if ((is_network_byte_order() && endian.b[0] != 0)
		|| (!is_network_byte_order() && endian.b[0] != 1)) {
		throw std::runtime_error(
			"endiannes was not correctly determined at compile time"
		);
	}
}

TEST(EndianGuard, NoThrow) {
	EXPECT_NO_THROW(endian_guard());
}
