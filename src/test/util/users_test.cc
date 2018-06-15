#include <algorithm>
#include <gtest/gtest.h>
#include <iostream>
#include <iterator>

#include <unistdx/ipc/identity>
#include <unistdx/util/groupstream>
#include <unistdx/util/user>
#include <unistdx/util/userstream>

#include <unistdx/test/operator>

TEST(User, Enumerate) {
	{
		sys::userstream users;
		const ptrdiff_t dist =
			std::distance(
				sys::user_iterator(users),
				sys::user_iterator()
			);
		EXPECT_GE(dist, 1);
	}
	if (::testing::Test::HasFailure()) {
		std::cout << "all users" << std::endl;
		sys::userstream users;
		std::copy(
			sys::user_iterator(users),
			sys::user_iterator(),
			std::ostream_iterator<sys::user>(std::cout, "\n")
		);
	}
}

TEST(Group, Enumerate) {
	{
		sys::groupstream groups;
		const ptrdiff_t dist =
			std::distance(
				sys::group_iterator(groups),
				sys::group_iterator()
			);
		EXPECT_GE(dist, 1);
	}
	if (::testing::Test::HasFailure()) {
		std::cout << "all groups" << std::endl;
		sys::groupstream groups;
		std::copy(
			sys::group_iterator(groups),
			sys::group_iterator(),
			std::ostream_iterator<sys::group>(std::cout, "\n")
		);
	}
}

TEST(User, FindBy) {
	errno = 0;
	std::clog << 1 << std::endl;
	sys::user u1, u2, u3;
	std::clog << 2 << std::endl;
	bool success;
	std::clog << 3 << std::endl;
	success = sys::find_user(sys::this_process::user(), u1);
	std::clog << 4 << std::endl;
	EXPECT_TRUE(success);
	std::clog << 5 << std::endl;
	EXPECT_EQ(sys::this_process::user(), u1.id());
	std::clog << 6 << std::endl;
	EXPECT_EQ(sys::this_process::group(), u1.group_id());
	std::clog << 7 << std::endl;
	EXPECT_STREQ(std::getenv("HOME"), u1.home());
	std::clog << 8 << std::endl;
	EXPECT_STREQ(std::getenv("SHELL"), u1.shell());
	std::clog << 9 << std::endl;
	success = sys::find_user(u1.name(), u2);
	std::clog << 10 << std::endl;
	EXPECT_TRUE(success);
	std::clog << 11 << std::endl;
	EXPECT_EQ(u1, u2);
	std::clog << 12 << std::endl;
	EXPECT_STREQ(u1.password(), u2.password());
	std::clog << 13 << std::endl;
	EXPECT_STREQ(u1.real_name(), u2.real_name());
	std::clog << 14 << std::endl;
	sys::user copy(std::move(u2));
	std::clog << 15 << std::endl;
	EXPECT_EQ(u1, copy);
	std::clog << 16 << std::endl;
	test::stream_insert_starts_with(u1.name(), u1);
	std::clog << 17 << std::endl;
	success = sys::find_user("a", u3);
	std::clog << 18 << std::endl;
	EXPECT_FALSE(success);
	std::clog << 19 << std::endl;
}

TEST(Group, FindBy) {
	sys::group g1, g2;
	bool success;
	success = sys::find_group(sys::this_process::group(), g1);
	EXPECT_TRUE(success);
	EXPECT_EQ(sys::this_process::group(), g1.id());
	success = sys::find_group(g1.name(), g2);
	EXPECT_TRUE(success);
	EXPECT_EQ(g1, g2);
	EXPECT_EQ(g1.size(), g2.size());
	EXPECT_STREQ(g1.name(), g2.name());
	EXPECT_STREQ(g1.password(), g2.password());
	sys::group copy(std::move(g2));
	EXPECT_EQ(g1, copy);
	test::stream_insert_starts_with(g1.name(), g1);
}
