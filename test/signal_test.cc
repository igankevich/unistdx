#include <gtest/gtest.h>
#include <sys/ipc/process>
#include <sys/ipc/signal>
#include <stdx/debug.hh>

volatile int caught = 0;

void
catch_signal(int) {
	caught = 1;
}

TEST(Signal, BindSignal) {
	using namespace sys::this_process;
	bind_signal(sys::signal::user_defined_1, catch_signal);
	send(sys::signal::user_defined_1);
	EXPECT_EQ(1, caught);
}
