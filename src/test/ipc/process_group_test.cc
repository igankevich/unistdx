#include <mutex>

#include <unistdx/ipc/process_group>

#include <unistdx/test/operator>

TEST(process_group, wait_async) {
	sys::process_group g;
	test::stream_insert_starts_with("{gid=0", g);
	g.emplace([] () {
		return 0;
	}, sys::process_flag::wait_for_exec);
	g.emplace([] () {
		return 0;
	}, sys::process_flag::wait_for_exec);
	std::mutex mtx;
	g.wait(
		mtx,
		[] (const sys::process& proc, sys::process_status status) {
			EXPECT_TRUE(status.exited());
			EXPECT_FALSE(status.killed());
			EXPECT_FALSE(status.stopped());
			EXPECT_FALSE(status.core_dumped());
			EXPECT_FALSE(status.trapped());
			EXPECT_FALSE(status.continued());
			EXPECT_EQ(0, status.exit_code());
			EXPECT_STREQ("exited", status.status_string());
			EXPECT_NE("", test::stream_insert(status));
		}
	);
}

TEST(process_group, wait_sync) {
	sys::process_group g;
	test::stream_insert_starts_with("{gid=0", g);
	g.emplace([] () {
		return 0;
	}, sys::process_flag::wait_for_exec);
	g.emplace([] () {
		return 0;
	}, sys::process_flag::wait_for_exec);
	int ret = g.wait();
	EXPECT_EQ(0, ret);
}
