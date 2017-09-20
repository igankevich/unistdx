#include "endian"
#include "byte_order"

#include <stdexcept>

sys::endian_guard::endian_guard() {
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
