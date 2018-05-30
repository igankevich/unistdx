#include <gtest/gtest.h>

#include <set>

#include <unistdx/bits/for_each_file_descriptor>
#include <unistdx/io/pipe>

TEST(ForEachFileDescriptor, Pipe) {
	sys::pipe p;
	std::set<sys::fd_type> expected{p.in().fd(), p.out().fd()}, actual;
	sys::bits::for_each_file_descriptor(
		[&] (const sys::poll_event& rhs) {
			const auto fd = rhs.fd();
			if (fd == p.in().fd() || fd == p.out().fd()) {
				actual.emplace(fd);
			}
		}
	);
	EXPECT_EQ(expected, actual);
}
