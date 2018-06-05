#include <gtest/gtest.h>

#include <unistdx/fs/file_mode>

TEST(file_mode, operators) {
	sys::file_mode a;
	sys::file_mode b(0755);
	sys::file_mode c(0644);
	sys::file_mode d(0755);
	sys::file_mode e(c);
	EXPECT_EQ(a, a);
	EXPECT_EQ(b, b);
	EXPECT_EQ(c, c);
	EXPECT_NE(a, b);
	EXPECT_NE(a, c);
	EXPECT_NE(a, d);
	EXPECT_EQ(b, d);
	EXPECT_EQ(c, e);
	EXPECT_EQ(c, 0644);
	EXPECT_EQ(c.mode(), 0644);
	EXPECT_NE(c, 0600);
	c = 0600;
	EXPECT_EQ(c, 0600);
}

TEST(file_mode, bits) {
	sys::file_mode a(0752);
	sys::file_mode b(0147);
	sys::file_mode c(0640);
	sys::file_mode e(01640);
	sys::file_mode d(a.user() | b.group() | c.other() | e.special());
	EXPECT_EQ(sys::file_mode(01740), d);
}

void
expect_eq(const char* s, sys::file_mode m) {
	std::stringstream str;
	str << m;
	EXPECT_EQ(s, str.str());
}

TEST(file_mode, print) {
	expect_eq("--trwxrw-r--", sys::file_mode(01764));
	expect_eq("ug-rwxrw-r--", sys::file_mode(06764));
	expect_eq("------rw-r--", sys::file_mode(0064));
	expect_eq("---r----xr--", sys::file_mode(0414));
	expect_eq("---r----x-wx", sys::file_mode(0413));
}
