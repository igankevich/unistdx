#include <gtest/gtest.h>

#include <unistdx/fs/file_stat>
#include <unistdx/fs/mkdirs>
#include <unistdx/test/tmpdir>

TEST(MakeDirectories, Full) {
	sys::path root(__func__);
	test::tmpdir tdir_h(root);
	EXPECT_NO_THROW(sys::mkdirs(sys::path(root, "1", "2", "3")));
	EXPECT_NO_THROW(sys::mkdirs(sys::path(root, "a", "b")));
	EXPECT_NO_THROW(sys::mkdirs(sys::path(root, "x")));
	EXPECT_TRUE(sys::file_stat(sys::path(root, "1", "2", "3")).exists());
}
