#include <gtest/gtest.h>

#include <thread>

#include <unistdx/base/log_message>
#include <unistdx/base/make_object>
#include <unistdx/ipc/argstream>
#include <unistdx/ipc/execute>
#include <unistdx/ipc/identity>
#include <unistdx/ipc/process>

TEST(Process, Fork) {
	sys::pid_type pid = sys::this_process::id();
	sys::process child {
		[pid] () {
			sys::pid_type parent_pid = sys::this_process::parent_id();
			sys::pid_type child_pid = sys::this_process::id();
			int ret = 1;
			if (child_pid != pid && parent_pid == pid) {
				ret = 0;
			}
			return ret;
		}
	};
	sys::proc_status status = child.wait();
	EXPECT_EQ(0, status.exit_code());
}

TEST(Process, ForkExec) {
	sys::process child {
		[] () {
			sys::argstream args;
			args.append("ls");
			return sys::this_process::execute_command(args.argv());
		}
	};
	sys::proc_status status = child.wait();
	EXPECT_EQ(0, status.exit_code());
}

TEST(ArgStream, All) {
	const std::string arg0 = "Hello!!!";
	const std::string arg1 = "world";
	const int arg2 = 123;
	sys::argstream args;
	EXPECT_EQ(args.argc(), 0);
	args << arg0 << '\0' << arg1 << '\0' << arg2 << '\0';
	EXPECT_EQ(args.argc(), 3);
	EXPECT_EQ(args.argv()[0], arg0);
	EXPECT_EQ(args.argv()[1], arg1);
	EXPECT_EQ(args.argv()[2], std::to_string(arg2));
	EXPECT_EQ(args.argv()[args.argc()], (char*)nullptr);
	args.append(arg0, arg1, arg2);
	EXPECT_EQ(args.argc(), 6);
	EXPECT_EQ(args.argv()[3], arg0);
	EXPECT_EQ(args.argv()[4], arg1);
	EXPECT_EQ(args.argv()[5], std::to_string(arg2));
	EXPECT_EQ(args.argv()[args.argc()], (char*)nullptr);
}

TEST(SetIdentity, Exceptions) {
	using namespace sys::this_process;
	sys::uid_type other_uid = user() + 1;
	sys::gid_type other_gid = group() + 1;
	sys::uid_type old_uid = user();
	sys::gid_type old_gid = group();
	EXPECT_THROW(
		set_identity(other_uid, other_gid),
		std::system_error
	);
	EXPECT_EQ(old_uid, user());
	EXPECT_EQ(old_gid, group());
	EXPECT_THROW(
		set_identity(1000, other_gid),
		std::system_error
	);
	EXPECT_EQ(old_uid, user());
	EXPECT_EQ(old_gid, group());
	EXPECT_NO_THROW(set_identity(user(), group()));
	EXPECT_EQ(old_uid, user());
	EXPECT_EQ(old_gid, group());
}

/*
TEST(Process, LogMessage) {
	sys::process child {
		[] () {
			for (int i=0; i<1000; ++i) {
				sys::log_message("child", "message a=_, b=_, c=_", i, i, i);
			}
			return 0;
		}
	};
	std::thread thr([] () {
		for (int i=0; i<1000; ++i) {
			sys::log_message("thread", "message a=_, b=_, c=_", i, i, i);
		}
	});
	for (int i=0; i<1000; ++i) {
		sys::log_message("parent", "message a=_, b=_, c=_", i, i, i);
	}
	thr.join();
	sys::proc_status status = child.wait();
	EXPECT_EQ(0, status.exit_code());
}
*/
