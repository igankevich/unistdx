#include <gtest/gtest.h>

#include <unistdx/base/log_message>

TEST(log_message, bad_number_of_arguments) {
	EXPECT_NO_THROW(sys::log_message("tst", "_", 1, 2));
	EXPECT_NO_THROW(sys::log_message("tst", "no arguments", 1));
}

TEST(log_message, manual_output) {
	sys::log_message msg("tst");
	msg.out() << std::setw(20) << 1234;
}

