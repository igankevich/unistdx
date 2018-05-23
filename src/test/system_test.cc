#include <unistdx/io/terminal>
#include <unistdx/util/system>

#include <gtest/gtest.h>

#include <stdlib.h>

TEST(System, ThreadConcurrency) {
	::setenv("UNISTDX_CONCURRENCY", "123", 1);
	EXPECT_EQ(123, sys::thread_concurrency());
	::setenv("UNISTDX_CONCURRENCY", "1", 1);
	EXPECT_EQ(1, sys::thread_concurrency());
	::setenv("UNISTDX_CONCURRENCY", "0", 1);
	EXPECT_NE(0, sys::thread_concurrency());
	::setenv("UNISTDX_CONCURRENCY", "-123", 1);
	EXPECT_NE(-123, sys::thread_concurrency());
	::unsetenv("UNISTDX_CONCURRENCY");
}

TEST(System, IOConcurrency) {
	EXPECT_GT(sys::io_concurrency(), 0);
}

TEST(System, PageSize) {
	EXPECT_GT(sys::page_size(), 0);
}

TEST(System, Cache) {
	sys::cache cache;
	for (const auto& c : cache) {
		std::clog << "Cache level=" << c.level()
			<< ",size=" << c.size()
			<< ",line_size=" << c.line_size()
			<< ",assoc=" << c.associativity()
			<< std::endl;
	}
}
