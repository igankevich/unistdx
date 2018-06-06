#include <gtest/gtest.h>
#include <unistdx/ipc/identity>

TEST(SetIdentity, ExceptionsRunAsRoot) {
	using namespace sys::this_process;
	if (user() != sys::superuser()) {
		return;
	}
	EXPECT_NE(1000, user());
	EXPECT_NE(1000, group());
	EXPECT_NO_THROW(set_identity(1000, 1000));
	EXPECT_EQ(1000, user());
	EXPECT_EQ(1000, group());
	EXPECT_EQ(1000, effective_user());
	EXPECT_EQ(1000, effective_group());
}

TEST(identity, basic) {
	using namespace sys::this_process;
	EXPECT_EQ(user(), effective_user());
	EXPECT_EQ(group(), effective_group());
	EXPECT_EQ(0, sys::supergroup());
	EXPECT_EQ(0, sys::superuser());
}

TEST(identity, set_unpriviledged) {
	using namespace sys::this_process;
	if (user() == sys::superuser()) {
		return;
	}
	sys::uid_type olduser = user();
	sys::gid_type oldgroup = group();
	sys::uid_type newuid = user() + 1;
	sys::gid_type newgid = group() + 1;
	EXPECT_THROW(set_identity(newuid, newgid), sys::bad_call);
	EXPECT_EQ(olduser, user());
	EXPECT_EQ(oldgroup, group());
}
