#include <gtest/gtest.h>

#include <unistdx/base/make_object>
#include <unistdx/ipc/process>
#include <unistdx/ipc/signal>

#include <unistdx/test/operator>
#include <unistdx/test/exception>

volatile int caught = 0;

void
catch_signal(int) {
    caught = 1;
}

void
catch_signal_2(int) {
}

void
catch_sigaction(int,sys::siginfo_type*,void*) {
    caught = 1;
}

TEST(Signal, BindSignal) {
    using namespace sys::this_process;
    bind_signal(sys::signal::user_defined_1, catch_sigaction);
    sys::this_process::send(sys::signal::user_defined_1);
    EXPECT_EQ(1, caught);
}


TEST(signal, print) {
    test::stream_insert_starts_with("terminate", sys::signal::terminate);
    test::stream_insert_starts_with("unknown", sys::signal(-1));
    test::stream_insert_starts_with("unknown", sys::signal(0));
    test::stream_insert_starts_with(
        "unknown",
        sys::signal(std::numeric_limits<sys::signal_type>::max())
    );
}

TEST(signal, bind_bad_signal) {
    using namespace sys::this_process;
    UNISTDX_EXPECT_ERROR(
        std::errc::invalid_argument,
        bind_signal(sys::signal(-1), catch_signal_2)
    );
    UNISTDX_EXPECT_ERROR(
        std::errc::invalid_argument,
        bind_signal(sys::signal(-1), catch_sigaction)
    );
}
