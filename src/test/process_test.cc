#include <gtest/gtest.h>
#include <unistdx/base/make_object>
#include <unistdx/ipc/argstream>
#include <unistdx/ipc/execute>
#include <unistdx/ipc/process>

TEST(Process, Fork) {
	sys::pid_type pid = sys::this_process::id();
	sys::process child {
		[pid] () {
			sys::pid_type parent_pid = sys::this_process::parent_id();
			sys::pid_type child_pid = sys::this_process::id();
			assert(child_pid != pid);
			assert(parent_pid == pid);
			return 0;
		}
	};
	sys::proc_status status = child.wait();
	EXPECT_EQ(0, status.exit_code());
}

TEST(Process, ForkExec) {
	sys::process child {
		[] () {
			return sys::this_process::execute_command("ls");
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
