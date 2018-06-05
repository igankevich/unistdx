#include <gtest/gtest.h>

#include <unistdx/base/byte_buffer>
#include <unistdx/base/check>

TEST(byte_buffer, resize) {
	const size_t max0 = sys::byte_buffer::max_size();
	sys::byte_buffer buf(4096);
	EXPECT_THROW(buf.resize(max0), sys::bad_call);
	EXPECT_THROW(sys::byte_buffer b(max0), sys::bad_call);
}
