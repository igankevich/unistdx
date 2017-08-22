#include <gtest/gtest.h>
#include <string>
#include <unistdx/fs/path>
#include <unordered_set>

TEST(Path, Hash) {
	std::unordered_set<sys::path> s;
	for (int i=0; i<10; ++i) {
		s.emplace("/tmp");
	}
	EXPECT_EQ(1, s.size());
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
