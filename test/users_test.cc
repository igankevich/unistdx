#include <algorithm>
#include <gtest/gtest.h>
#include <iostream>
#include <iterator>
#include <sys/ipc/identity>
#include <sys/util/groupstream>
#include <sys/util/user>
#include <sys/util/userstream>

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
	sys::user u1, u2;
	bool success;
	success = sys::find_user(sys::this_process::user(), u1);
	EXPECT_TRUE(success);
	success = sys::find_user(u1.name(), u2);
	EXPECT_TRUE(success);
}

struct args_and_count {

	size_t count;
	std::vector<const char*> args;

	inline const char**
	get_args() const noexcept {
		return const_cast<const char**>(args.data());
	}

};

class CSrtingIteratorTest: public ::testing::TestWithParam<args_and_count> {};

TEST_P(CSrtingIteratorTest, All) {
	const args_and_count& param = GetParam();
	const size_t cnt =
		std::distance(
			sys::cstring_iterator<const char*>(param.get_args()),
			sys::cstring_iterator<const char*>()
		);
	EXPECT_EQ(param.count, cnt);
}

INSTANTIATE_TEST_CASE_P(
	AllSizes,
	CSrtingIteratorTest,
	::testing::Values(
		args_and_count{2, {"1", "2", 0}},
		args_and_count{1, {"1", 0}},
		args_and_count{0, {0}}
	)
);

TEST(Group, FindBy) {
	sys::group g1, g2;
	bool success;
	success = sys::find_group(sys::this_process::group(), g1);
	EXPECT_TRUE(success);
	success = sys::find_group(g1.name(), g2);
	EXPECT_TRUE(success);
}
