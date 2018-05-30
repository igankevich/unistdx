#include <gtest/gtest.h>

#include <unistdx/fs/file_stat>
#include <unistdx/fs/mkdirs>

#include "tmpdir.hh"

TEST(MakeDirectories, Full) {
	sys::path root(test::current_test_name(), "mkdirs-1");
	test::tmpdir tdir_h(root);
	EXPECT_NO_THROW(sys::mkdirs(sys::path(root, "1", "2", "3")));
	EXPECT_NO_THROW(sys::mkdirs(sys::path(root, "a", "b")));
	EXPECT_NO_THROW(sys::mkdirs(sys::path(root, "x")));
	EXPECT_TRUE(sys::file_stat(sys::path(root, "1", "2", "3")).exists());
}
