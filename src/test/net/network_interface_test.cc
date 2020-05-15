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

TEST(veth_interface, _) {
    sys::veth_interface veth0("veth0", "veth1");
    using flags = sys::network_interface::flag;
    veth0.up();
    ASSERT_EQ(flags::up, (veth0.flags() & flags::up));
    veth0.down();
    ASSERT_EQ(flags{}, (veth0.flags() & flags::up));
    sys::test::print_flags(veth0.flags());
}

TEST(veth_interface, move) {
    std::vector<sys::veth_interface> veths;
    veths.emplace_back("zeth0", "zeth0x");
    veths.emplace_back("zeth1", "zeth1x");
    veths.emplace_back("zeth2", "zeth2x");
    veths.emplace_back("zeth3", "zeth3x");
    for (auto& v : veths) {
        std::clog << "v.index()=" << v.index() << std::endl;
        std::clog << "v.peer().index()=" << v.peer().index() << std::endl;
    }
    veths.resize(100);
}

int main(int argc, char* argv[]) {
    using f = sys::unshare_flag;
    sys::this_process::unshare(f::users | f::network);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
