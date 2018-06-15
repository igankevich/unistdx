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
	sys::user u1, u2, u3;
	bool success;
	success = sys::find_user(sys::this_process::user(), u1);
	EXPECT_TRUE(success);
	EXPECT_EQ(sys::this_process::user(), u1.id());
	EXPECT_EQ(sys::this_process::group(), u1.group_id());
	EXPECT_STREQ(std::getenv("HOME"), u1.home());
	EXPECT_STREQ(std::getenv("SHELL"), u1.shell());
	success = sys::find_user(u1.name(), u2);
	EXPECT_TRUE(success);
	EXPECT_EQ(u1, u2);
	EXPECT_STREQ(u1.password(), u2.password());
	EXPECT_STREQ(u1.real_name(), u2.real_name());
	sys::user copy(std::move(u2));
	EXPECT_EQ(u1, copy);
	test::stream_insert_starts_with(u1.name(), u1);
	success = sys::find_user("a", u3);
	EXPECT_FALSE(success);
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
