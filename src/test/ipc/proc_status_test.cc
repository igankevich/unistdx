#include <chrono>
#include <thread>

#include <unistdx/ipc/process>

#include <unistdx/test/operator>

TEST(proc_info, exit) {
	sys::process child {
		[] () {
			return 0;
		}
	};
	sys::proc_info status = child.wait();
	EXPECT_TRUE(status.exited());
	EXPECT_FALSE(status.killed());
	EXPECT_FALSE(status.stopped());
	EXPECT_FALSE(status.core_dumped());
	EXPECT_FALSE(status.trapped());
	EXPECT_FALSE(status.continued());
	EXPECT_EQ(0, status.exit_code());
	EXPECT_STREQ("exited", status.status_string());
	test::stream_insert_contains("status=exited,exit_code=0", status);
}

TEST(proc_info, abort) {
	sys::process child {
		[] () [[noreturn]] -> int {
			std::abort();
		}
	};
	sys::proc_info status = child.wait();
	EXPECT_FALSE(status.exited());
	EXPECT_FALSE(status.stopped());
	EXPECT_TRUE(status.killed() || status.core_dumped());
	EXPECT_FALSE(status.trapped());
	EXPECT_FALSE(status.continued());
	EXPECT_EQ(sys::signal::abort, status.term_signal());
	EXPECT_STREQ("core_dumped", status.status_string());
	test::stream_insert_contains("signal=abort", status);
}

void
test_print(const char* str, int si_code) {
	sys::siginfo_type s{};
	s.si_code = si_code;
	sys::proc_info status(s);
	test::stream_insert_contains(str, status);
}

TEST(proc_info, print) {
	test_print("status=stopped", CLD_STOPPED);
	test_print("status=continued", CLD_CONTINUED);
	test_print("status=trapped", CLD_TRAPPED);
}
