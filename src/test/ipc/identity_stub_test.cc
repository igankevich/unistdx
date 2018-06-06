#include <gtest/gtest.h>
#include <unistdx/ipc/identity>

TEST(identity, set_stub) {
	using namespace sys::this_process;
	if (user() == sys::superuser()) {
		return;
	}
	sys::uid_type olduser = user();
	sys::gid_type oldgroup = group();
	sys::uid_type newuid = user()+ 1;
	sys::gid_type newgid = group() + 1;
	EXPECT_THROW(set_identity(newuid, newgid), sys::bad_call);
	EXPECT_EQ(olduser, user());
	EXPECT_EQ(oldgroup, group());
}
