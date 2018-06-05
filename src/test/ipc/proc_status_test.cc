#include <chrono>
#include <thread>

#include <unistdx/ipc/process>

#include <unistdx/test/operator>

TEST(proc_status, exit) {
	sys::process child {
		[] () {
			return 0;
		}
	};
	sys::proc_status status = child.wait();
	EXPECT_TRUE(status.exited());
	EXPECT_FALSE(status.killed());
	EXPECT_FALSE(status.stopped());
	EXPECT_FALSE(status.core_dumped());
	EXPECT_FALSE(status.trapped());
	EXPECT_FALSE(status.continued());
	EXPECT_EQ(0, status.exit_code());
	EXPECT_STREQ("exited", status.status_string());
	test::stream_insert_equals("{status=exited,exit_code=0}", status);
}

TEST(proc_status, abort) {
	sys::process child {
		[] () [[noreturn]] -> int {
			std::abort();
		}
	};
	sys::proc_status status = child.wait();
	EXPECT_FALSE(status.exited());
	EXPECT_TRUE(status.killed());
	EXPECT_FALSE(status.stopped());
	EXPECT_TRUE(status.core_dumped());
	EXPECT_FALSE(status.trapped());
	EXPECT_FALSE(status.continued());
	EXPECT_EQ(sys::signal::abort, status.term_signal());
	EXPECT_STREQ("killed", status.status_string());
	test::stream_insert_equals("{status=killed,term_signal=abort(6)}", status);
}
