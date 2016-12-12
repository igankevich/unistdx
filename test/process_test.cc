#include <stdx/debug.hh>
#include <sys/process.hh>
#include "test.hh"

void
test_fork() {
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
	assert(status.exit_code() == 0);
}

void
test_fork_exec(const char* cmd) {
	sys::process child{
		[cmd] () {
			return sys::this_process::execute_command(cmd);
		}
	};
	sys::proc_status status = child.wait();
	assert(status.exit_code() == 0);
}

int main() {
	test_fork();
	test_fork_exec("ls");
	return 0;
}
