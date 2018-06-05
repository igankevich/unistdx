#include <gtest/gtest.h>

#include <set>

#include <unistdx/bits/for_each_file_descriptor>
#include <unistdx/io/pipe>
#include <unistdx/io/fildes>

#include <unistdx/test/temporary_file>

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

TEST(fildes, remap) {
	sys::fildes f(UNISTDX_TMPFILE, sys::open_flag::create, 0644);
	EXPECT_NO_THROW(f.remap());
	EXPECT_NO_THROW(f.remap(1000));
}
