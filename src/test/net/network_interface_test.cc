#include <gtest/gtest.h>

#include <unistdx/ipc/process>
#include <unistdx/net/bridge_interface>
#include <unistdx/net/network_interface>
#include <unistdx/net/netlink_poller>
#include <unistdx/net/veth_interface>
#include <unistdx/test/print_flags>

TEST(network_interface, flags) {
    using f = sys::network_interface::flag;
    sys::network_interface lo("lo");
    ASSERT_EQ(f{}, (lo.flags() & f::up));
    lo.flags(lo.flags() | f::up);
    ASSERT_EQ(f::up, (lo.flags() & f::up));
    lo.unsetf(f::up);
    ASSERT_EQ(f{}, (lo.flags() & f::up));
    lo.setf(f::up);
    ASSERT_EQ(f::up, (lo.flags() & f::up));
}

TEST(network_interface, index) {
    sys::network_interface lo("lo");
    auto index = lo.index();
    sys::network_interface lo2(index);
    EXPECT_EQ(index, lo2.index());
}

TEST(bridge_interface, add) {
    using f = sys::network_interface::flag;
    sys::bridge_interface br("br0");
    br.up();
    ASSERT_EQ(f::up, (br.flags() & f::up));
    br.down();
    ASSERT_EQ(f{}, (br.flags() & f::up));
}

int main(int argc, char* argv[]) {
    using f = sys::unshare_flag;
    sys::this_process::unshare(f::users | f::network);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
