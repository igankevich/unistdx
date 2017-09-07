#include <gtest/gtest.h>
#include <unistdx/ipc/identity>

TEST(SetIdentity, ExceptionsRunAsRoot) {
	using namespace sys::this_process;
	if (user() != sys::superuser()) {
		std::exit(77);
	}
	sys::uid_type old_uid = user();
	sys::gid_type old_gid = group();
	EXPECT_NO_THROW(set_identity(1000, 1000));
	EXPECT_EQ(1000, user());
	EXPECT_EQ(1000, group());
	EXPECT_EQ(1000, effective_user());
	EXPECT_EQ(1000, effective_group());
}
