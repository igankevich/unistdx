#include <gtest/gtest.h>

#include <unistdx/base/byte_buffer>
#include <unistdx/base/check>

TEST(byte_buffer, resize) {
	sys::byte_buffer buf(4096);
	EXPECT_THROW(buf.resize(sys::byte_buffer::max_size()), sys::bad_call);
	EXPECT_THROW(sys::byte_buffer(sys::byte_buffer::max_size()), sys::bad_call);
}
