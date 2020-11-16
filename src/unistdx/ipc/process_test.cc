/*
UNISTDX — C++ library for Linux system calls.
© 2016, 2017, 2018, 2020 Ivan Gankevich

This file is part of UNISTDX.

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/

#include <gtest/gtest.h>

#include <thread>

#include <unistdx/base/log_message>
#include <unistdx/io/pipe>
#include <unistdx/ipc/argstream>
#include <unistdx/ipc/execute>
#include <unistdx/ipc/identity>
#include <unistdx/ipc/process>
#include <unistdx/test/config>

#if defined(UNISTDX_TEST_HAVE_VALGRIND_H)
#include <valgrind.h>
#endif

TEST(process, basic) {
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
    sys::process_status status = child.wait();
    EXPECT_EQ(0, status.exit_code());
}

struct process_exec_params {
    sys::process_flag flags;
    std::string cmd;
    bool success;
};

std::ostream& operator<<(std::ostream& out, const process_exec_params& rhs) {
    out << "Flags: \"" << int(rhs.flags) << "\"\n";
    out << "Cmd: \"" << rhs.cmd << "\"\n";
    out << "Success: \"" << rhs.success << "\"\n";
    return out;
}

struct process_exec_test:
    public ::testing::TestWithParam<process_exec_params> {};

using f = sys::process_flag;
std::vector<process_exec_params> all_params{
    {f::fork, "non-existent-file", false},
    {f::wait_for_exec|f::signal_parent, "non-existent-file", false},
    {f::fork, UNISTDX_TEST_EMPTY_EXE_PATH, true},
    {f::wait_for_exec|f::signal_parent, UNISTDX_TEST_EMPTY_EXE_PATH, true},
};

TEST_P(process_exec_test, return_int) {
    auto param = GetParam();
    sys::process_flag flags = param.flags;
    std::string cmd = param.cmd;
    bool success = param.success;
    sys::process child {
        [&] () {
            sys::argstream args;
            args.append(cmd);
            sys::this_process::execute_command(args.argv());
            return 0;
        },
        flags
    };
    sys::process_status status = child.wait();
    if (success) {
        EXPECT_EQ(0, status.exit_code());
    } else {
        EXPECT_NE(0, status.exit_code());
    }
}

TEST_P(process_exec_test, return_void) {
    auto param = GetParam();
    sys::process_flag flags = param.flags;
    std::string cmd = param.cmd;
    bool success = param.success;
    sys::process child {
        [&] () {
            sys::argstream args;
            args.append(cmd);
            sys::this_process::execute_command(args.argv());
        },
        flags
    };
    sys::process_status status = child.wait();
    if (success) {
        EXPECT_EQ(0, status.exit_code());
    } else {
        EXPECT_NE(0, status.exit_code());
    }
}

INSTANTIATE_TEST_CASE_P(
    _,
    process_exec_test,
    ::testing::ValuesIn(all_params)
);

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
    EXPECT_THROW(set_identity(other_uid, other_gid), sys::bad_call);
    EXPECT_EQ(old_uid, user());
    EXPECT_EQ(old_gid, group());
    EXPECT_THROW(set_identity(1000, other_gid), sys::bad_call);
    EXPECT_EQ(old_uid, user());
    EXPECT_EQ(old_gid, group());
    EXPECT_NO_THROW(set_identity(user(), group()));
    EXPECT_EQ(old_uid, user());
    EXPECT_EQ(old_gid, group());
}

#if defined(UNISTDX_TEST_HAVE_UNSHARE)
TEST(unshare, hostname) {
    using namespace sys::this_process;
    auto status = sys::process{[] () {
        using f = sys::unshare_flag;
        unshare(f::users | f::hostname);
        hostname("unistdx");
        return (hostname() == "unistdx") ? 0 : 1;
    }}.wait();
    EXPECT_EQ(0, status.exit_code());
}

TEST(clone, users) {
    using namespace sys::this_process;
    using pf = sys::process_flag;
    sys::pipe pipe;
    pipe.in().unsetf(sys::open_flag::non_blocking);
    pipe.out().unsetf(sys::open_flag::non_blocking);
    sys::process child{[&pipe] () {
        char ch;
        pipe.in().read(&ch, 1);
        sys::log_message("clone-users", "_ _", user(), group());
        return (user() == 0 && group() == 0) ? 0 : 1;
    }, pf::signal_parent | pf::unshare_users};
    child.init_user_namespace();
    pipe.out().write("x", 1);
    auto status = child.wait();
    EXPECT_EQ(0, status.exit_code());
}

TEST(thread, _) {
    using pf = sys::process_flag;
    sys::process t{[] () -> int {
        return 0;
    }, pf::signal_parent | pf::share_memory};
    auto status = t.wait();
    EXPECT_EQ(0, status.exit_code());
}
#endif

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
    sys::process_status status = child.wait();
    EXPECT_EQ(0, status.exit_code());
}
*/

int main(int argc, char* argv[]) {
    #if defined(UNISTDX_TEST_HAVE_VALGRIND_H)
    if (RUNNING_ON_VALGRIND) { std::exit(77); }
    #endif
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
