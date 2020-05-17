#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/wait.h>
#include <sched.h>
#include <signal.h>
#include <errno.h>
int child_main(void*) { return 0; }

int main() {
    char stack[4096];
    auto pid = clone(&child_main, stack+sizeof(stack), CLONE_VFORK | SIGCHLD, 0);
    if (pid == -1) { return errno; }
    int status = 0;
    int ret = waitpid(pid, &status, 0);
    if (ret == -1) { return errno; }
    return status;
}
