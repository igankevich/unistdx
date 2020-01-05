#include <unistdx/ipc/execute>

/**
\unistdx_example{execute.cc}
\details
The following code executes \c errno to display description
of error code 42 in Russian locale.
*/
int main() {
    /// [Execute child process]
    sys::argstream args;
    args.append("/bin/errno");
    args.append(42);
    sys::argstream env;
    env.append("LANG=ru_RU.UTF-8");
    sys::this_process::execute(args, env);
    /// [Execute child process]
    return 0;
}
