#include <gtest/gtest.h>

#include <unistdx/fs/canonical_path>
#include <unistdx/fs/file_status>
#include <unistdx/fs/mkdirs>
#include <unistdx/test/temporary_file>

TEST(MakeDirectories, Full) {
	sys::path root(UNISTDX_TMPDIR);
	test::tmpdir tdir_h(root);
	EXPECT_NO_THROW(sys::mkdirs(sys::path(root, "1", "2", "3")));
	EXPECT_NO_THROW(sys::mkdirs(sys::path(root, "a", "b")));
	EXPECT_NO_THROW(sys::mkdirs(sys::path(root, "x")));
	EXPECT_NO_THROW(sys::mkdirs(sys::path(root, "x")));
	EXPECT_NO_THROW(sys::mkdirs(sys::path(sys::canonical_path(root), "y")));
	EXPECT_NO_THROW(sys::mkdirs(sys::path("")));
	EXPECT_TRUE(sys::file_status(sys::path(root, "1", "2", "3")).exists());
}

TEST(mkdirs, file_permissions) {
	sys::path root(UNISTDX_TMPDIR);
	sys::path x(root, "x");
	test::tmpdir tdir_h(root);
	EXPECT_NO_THROW(sys::mkdirs(x));
	UNISTDX_CHECK(::chmod(x, 0));
	try {
		sys::mkdirs(x);
		FAIL();
	} catch (const sys::bad_call& err) {
		EXPECT_EQ(std::errc::permission_denied, err.errc()) << "err=" << err;
	}
	UNISTDX_CHECK(::chmod(x, 0644));
}

TEST(mkdirs, directory_permissions) {
	sys::path root(UNISTDX_TMPDIR);
	test::tmpdir tdir_h(root);
	UNISTDX_CHECK(::chmod(root, 0));
	try {
		sys::mkdirs(sys::path(root, "z"));
		FAIL();
	} catch (const sys::bad_call& err) {
		EXPECT_EQ(std::errc::permission_denied, err.errc()) << "err=" << err;
	}
	UNISTDX_CHECK(::chmod(root, 0755));
}

TEST(mkdirs, file_in_path) {
	sys::path root(UNISTDX_TMPFILE);
	test::temporary_file tmp(root);
	try {
		sys::mkdirs(sys::path(root, "x"));
		FAIL();
	} catch (const sys::bad_call& err) {
		EXPECT_EQ(std::errc::not_a_directory, err.errc()) << "err=" << err;
	}
	try {
		sys::mkdirs(sys::path(root, "x", "y"));
		FAIL();
	} catch (const sys::bad_call& err) {
		EXPECT_EQ(std::errc::not_a_directory, err.errc()) << "err=" << err;
	}
	try {
		sys::mkdirs(sys::path(root));
		FAIL();
	} catch (const sys::bad_call& err) {
		EXPECT_EQ(std::errc::not_a_directory, err.errc()) << "err=" << err;
	}
}
