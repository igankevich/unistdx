#include <gtest/gtest.h>

#include <unistdx/ipc/execute>

TEST(execute, errors) {
    sys::argstream args;
    args.append("non-existent-file");
    sys::argstream env;
    EXPECT_THROW(sys::this_process::execute(args), sys::bad_call);
    EXPECT_THROW(sys::this_process::execute_command(args), sys::bad_call);
    EXPECT_THROW(sys::this_process::execute(args, env), sys::bad_call);
    EXPECT_THROW(sys::this_process::execute_command(args, env), sys::bad_call);
}
