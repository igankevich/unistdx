#include <unistdx/ipc/execute>

/**
\example execute_command.cc
\details
The following code executes \c errno to display description
of error code 42 in Russian locale.
*/
int main() {
	/// [Execute child process]
	sys::argstream args;
	args.append("errno");
	args.append(42);
	sys::argstream env;
	env.append("LANG=ru_RU.UTF-8");
	sys::this_process::execute_command(args, env);
	/// [Execute child process]
	return 0;
}
