#include <chrono>
#include <mutex>
#include <thread>

#include <unistdx/ipc/process_group>
#include <unistdx/test/config>
#include <unistdx/test/operator>

using f = sys::process_flag;
using std::this_thread::sleep_for;
using std::chrono::milliseconds;

TEST(process_group, wait_async) {
    sys::process_group g;
    g.emplace([] () { sleep_for(milliseconds(10)); return 0; });
    g.emplace([] () { sleep_for(milliseconds(10)); return 0; });
    std::mutex mtx;
    g.wait(
        mtx,
        [] (const sys::process&, sys::process_status status) {
            EXPECT_TRUE(status.exited());
            EXPECT_FALSE(status.killed());
            EXPECT_FALSE(status.stopped());
            EXPECT_FALSE(status.core_dumped());
            EXPECT_FALSE(status.trapped());
            EXPECT_FALSE(status.continued());
            EXPECT_EQ(0, status.exit_code());
            EXPECT_STREQ("exited", status.status_string());
            EXPECT_NE("", test::stream_insert(status));
        }
    );
}

TEST(process_group, wait_sync) {
    sys::process_group g;
    g.emplace([] () { sleep_for(milliseconds(10)); return 0; });
    g.emplace([] () { sleep_for(milliseconds(10)); return 0; });
    int ret = g.wait();
    EXPECT_EQ(0, ret);
}
