#include <gtest/gtest.h>

#include <set>
#include <unistdx/bits/for_each_file_descriptor>
#include <unistdx/io/pipe>

TEST(Fildes, ForEachFileDescriptor) {
	std::set<sys::fd_type> expected{0,1,2}, actual;
	sys::bits::for_each_file_descriptor(
		[&] (const sys::poll_event& rhs) {
			actual.emplace(rhs.fd());
		}
	);
	EXPECT_EQ(expected, actual);
}

TEST(Fildes, ForEachFileDescriptorPipe) {
	sys::pipe p;
	std::set<sys::fd_type> expected{0,1,2,3,4}, actual;
	sys::bits::for_each_file_descriptor(
		[&] (const sys::poll_event& rhs) {
			actual.emplace(rhs.fd());
		}
	);
	EXPECT_EQ(expected, actual);
}
