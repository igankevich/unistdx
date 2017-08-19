#include <gtest/gtest.h>
#include <unordered_set>
#include <string>
#include <unistdx/fs/path>
#include <unistdx/fs/canonical_path>

TEST(Path, Hash) {
	std::unordered_set<sys::path> s;
	s.emplace("/tmp");
}

TEST(Path, Equals) {
	sys::path p("/tmp");
	sys::path q("/tmpx");
	// ==
	EXPECT_EQ(p, p);
	EXPECT_EQ("/tmp", p);
	EXPECT_EQ(p, "/tmp");
	EXPECT_EQ(std::string("/tmp"), p);
	EXPECT_EQ(p, std::string("/tmp"));
	// !=
	EXPECT_NE(q, p);
	EXPECT_NE(p, q);
	EXPECT_NE("/tmpx", p);
	EXPECT_NE(p, "/tmpx");
	EXPECT_NE(std::string("/tmpx"), p);
	EXPECT_NE(p, std::string("/tmpx"));
}

TEST(CanonicalPath, Assign) {
	sys::canonical_path dir1(".");
	sys::canonical_path dir2("..");
	dir1 = std::move(dir1.dirname());
	EXPECT_EQ(dir1, dir2);
}

TEST(CanonicalPath, Dirname) {
	sys::canonical_path dir1("/tmp");
	sys::canonical_path dir2("/");
	EXPECT_EQ(dir1.dirname(), dir2);
	EXPECT_EQ(dir2.dirname(), dir2);
	EXPECT_EQ(dir2.basename(), dir2);
}

TEST(CanonicalPath, TwoArgConstructor) {
	sys::canonical_path cwd(".");
	EXPECT_EQ(sys::canonical_path(cwd.dirname(), cwd.basename()), cwd);
}

TEST(CanonicalPath, NonExistent) {
	sys::canonical_path root("/");
	sys::canonical_path tmp;
	EXPECT_THROW(
		{
		    sys::canonical_path nonexistent(root, "non-existent-directory");
		},
		sys::bad_call
	);
}
