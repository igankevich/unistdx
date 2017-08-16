#include <stdx/debug.hh>
#include <sys/process.hh>
#include <gtest/gtest.h>

TEST(Process, Fork) {
	sys::pid_type pid = sys::this_process::id();
	sys::process child{
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
	sys::process child{
		[] () {
			return sys::this_process::execute_command("ls");
		}
	};
	sys::proc_status status = child.wait();
	EXPECT_EQ(0, status.exit_code());
}
